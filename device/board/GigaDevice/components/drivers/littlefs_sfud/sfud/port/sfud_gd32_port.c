/*
 * This file is part of the Serial Flash Universal Driver Library.
 *
 * Copyright (c) 2016-2018, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: Portable interface for each platform.
 * Created on: 2016-04-23
 */

#include <sfud.h>
#include <stdarg.h>

/*=============================外部接口==============================*/
#include "gd32f4xx.h"
#include "gd32f4xx_spi.h"
#include "gd32f4xx_gpio.h"
#include "gd32f4xx_rcu.h"

#include "los_task.h"
#include "los_mux.h"

#include <assert.h>
#include "systick.h"

#define  SPI_FLASH_CS_LOW()         gpio_bit_reset(GPIOA,GPIO_PIN_4)
#define  SPI_FLASH_CS_HIGH()        gpio_bit_set(GPIOA,GPIO_PIN_4)
#define  WRITE                      0x02     /* write to memory instruction */
#define  QUADWRITE                  0x32     /* quad write to memory instruction */
#define  WRSR                       0x01     /* write status register instruction */
#define  WREN                       0x06     /* write enable instruction */

#define  READ                       0x03     /* read from memory instruction */
#define  QUADREAD                   0x6B     /* read from memory instruction */
#define  RDSR                       0x05     /* read status register instruction */
#define  RDID                       0x9F     /* read identification */
#define  SE                         0x20     /* sector erase instruction */
#define  BE                         0xC7     /* bulk erase instruction */

#define  WTSR                       0x05     /* write status register instruction */

#define  WIP_FLAG                   0x01     /* write in progress(wip) flag */
#define  DUMMY_BYTE                 0xA5

/*用户数据*/
uint32_t spi_userData;

/*============================= 信号量锁 ==============================*/
static UINT32 *spiLock;
static void spi_Lock_Init(void)
{
    LOS_MuxCreate(&spiLock);
    if(spiLock == NULL)
    {
        printf("mux lock create fail\r\n");
    }
}
static void spi_lock(const struct __sfud_spi *spi)
{
    LOS_MuxPend(spiLock, LOS_WAIT_FOREVER);
}
static void spi_unlock(const struct __sfud_spi *spi)
{
    LOS_MuxPost(spiLock);
}
/*============================= 信号量锁 ==============================*/

/*============================= 延时和debug ==============================*/
void sfud_log_debug(const char *file, const long line, const char *format, ...) {
    va_list args;
    char log_buf[256];

    /* args point to the first variable parameter */
    va_start(args, format);
    printf("[SFUD](%s:%ld) ", file, line);
    /* must use vprintf to print */
    vsnprintf(log_buf, sizeof(log_buf), format, args);
    printf("%s\r\n", log_buf);
    va_end(args);
}
void sfud_log_info(const char *format, ...) {
    va_list args;
    char log_buf[256];

    /* args point to the first variable parameter */
    va_start(args, format);
    printf("[SFUD]");
    /* must use vprintf to print */
    vsnprintf(log_buf, sizeof(log_buf), format, args);
    printf("%s\r\n", log_buf);
    va_end(args);
}
/*============================= 延时和debug ==============================*/


/*============================= spi 操作接口 ==============================*/
static uint8_t spi_flash_send_byte(uint8_t byte)
{
    /* loop while data register in not emplty */
    while(RESET == spi_i2s_flag_get(SPI0,SPI_FLAG_TBE));

    /* send byte through the SPI0 peripheral */
    spi_i2s_data_transmit(SPI0,byte);

    /* wait to receive a byte */
    while(RESET == spi_i2s_flag_get(SPI0,SPI_FLAG_RBNE));

    /* return the byte read from the SPI bus */
    return(spi_i2s_data_receive(SPI0));
}

static void spi_flash_page_write(const uint8_t* pbuffer, uint16_t num_byte_to_write)
{
    /* select the flash: chip select low */
    SPI_FLASH_CS_LOW();

    /* while there is data to be written on the flash */
    while(num_byte_to_write--){
        /* send the current byte */
        spi_flash_send_byte(*pbuffer);
        /* point on the next byte to be written */
        pbuffer++;
    }
}

static void spi_flash_buffer_read(uint8_t* pbuffer, uint16_t num_byte_to_read)
{
    /* select the flash: chip slect low */
    SPI_FLASH_CS_LOW();

    /* while there is data to be read */
    while(num_byte_to_read--){
        /* read a byte from the flash */
        *pbuffer = spi_flash_send_byte(DUMMY_BYTE);
        /* point to the next location where the byte read will be saved */
        pbuffer++;
    }
}

static sfud_err spi_write_read(const sfud_spi *spi, const uint8_t *write_buf, size_t write_size, uint8_t *read_buf,
        size_t read_size) {
    sfud_err result = SFUD_SUCCESS;

    /**
     * add your spi write and read code
     */
    if (write_size)
    {
        SFUD_ASSERT(write_buf);
    }
    if (read_size)
    {
        SFUD_ASSERT(read_buf);
    }
	spi_flash_page_write(write_buf, write_size);
	spi_flash_buffer_read(read_buf, read_size);
		
	/* deselect the flash: chip select high */
    SPI_FLASH_CS_HIGH();
	return result;
}

static void spi_flash_init(void)
{
    spi_parameter_struct spi_init_struct;

    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_SPI0);

    gpio_af_set(GPIOA, GPIO_AF_5, GPIO_PIN_5|GPIO_PIN_6);
    gpio_af_set(GPIOB, GPIO_AF_5, GPIO_PIN_5);

    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5|GPIO_PIN_6);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5);

    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO_PIN_5|GPIO_PIN_6);

    /* SPI5_CS(GPIO pin configuration */
    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_4);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4);

    /* chip select invalid */
    SPI_FLASH_CS_HIGH();

    /* SPI0 parameter config */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_32;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI0, &spi_init_struct);

    /* set crc polynomial */
    spi_crc_polynomial_set(SPI0,7);

    /* quad wire SPI_IO2 and SPI_IO3 pin output enable */
    // qspi_io23_output_enable(SPI0);

    /* enable SPI0 */
    spi_enable(SPI0);
}


#ifdef SFUD_USING_QSPI
// static uint16_t spi_flash_send_halfword(uint16_t half_word)
// {
//     /* loop while data register in not emplty */
//     while(RESET == spi_i2s_flag_get(SPI0,SPI_FLAG_TBE));

//     /* send half word through the SPI0 peripheral */
//     spi_i2s_data_transmit(SPI0,half_word);

//     /* wait to receive a half word */
//     while(RESET == spi_i2s_flag_get(SPI0,SPI_FLAG_RBNE));

//     /* return the half word read from the SPI bus */
//     return spi_i2s_data_receive(SPI0);
// }
void spi_flash_write_enable(void)
{
    /* select the flash: chip select low */
    SPI_FLASH_CS_LOW();

    /* send "write enable" instruction */
    spi_flash_send_byte(WREN);

    /* deselect the flash: chip select high */
    SPI_FLASH_CS_HIGH();
}
void spi_flash_wait_for_write_end(void)
{
    uint8_t flash_status = 0;

    /* select the flash: chip select low */
    SPI_FLASH_CS_LOW();

    /* send "read status register" instruction */
    spi_flash_send_byte(RDSR);

    /* loop as long as the memory is busy with a write cycle */
    do{
        /* send a dummy byte to generate the clock needed by the flash
        and put the value of the status register in flash_status variable */
        flash_status = spi_flash_send_byte(DUMMY_BYTE);
    }while(SET == (flash_status & WIP_FLAG));

    /* deselect the flash: chip select high */
    SPI_FLASH_CS_HIGH();
}
void qspi_flash_quad_enable(void)
{
    /* enable the write access to the flash */
    spi_flash_write_enable();
    /* select the flash: chip select low */
    SPI_FLASH_CS_LOW();
    /* send "write status register" instruction */
    spi_flash_send_byte(WRSR);
    
    spi_flash_send_byte(0x00);
    spi_flash_send_byte(0x02);
    /* deselect the flash: chip select high */
    SPI_FLASH_CS_HIGH(); 
    /* wait the end of flash writing */
    spi_flash_wait_for_write_end();
}
void qspi_flash_buffer_read(uint8_t* pbuffer, uint32_t read_addr, uint16_t num_byte_to_read)
{
	qspi_flash_quad_enable();
	
    /* select the flash: chip select low */
    SPI_FLASH_CS_LOW();

    /* enable the qspi */
    qspi_enable(SPI0); 
    /* enable the qspi read operation */
    qspi_read_enable(SPI0);
	
    /* send "quad fast read from memory " instruction */
    spi_flash_send_byte(QUADREAD);
      
    /* send read_addr high nibble address byte to read from */
    spi_flash_send_byte((read_addr & 0xFF0000) >> 16);
    /* send read_addr medium nibble address byte to read from */
    spi_flash_send_byte((read_addr & 0xFF00) >> 8);
    /* send read_addr low nibble address byte to read from */
    spi_flash_send_byte(read_addr & 0xFF);
    spi_flash_send_byte(0xA5);         
    /* while there is data to be read */
    while(num_byte_to_read--){
        /* read a byte from the flash */
        *pbuffer = spi_flash_send_byte(DUMMY_BYTE);
        /* point to the next location where the byte read will be saved */
        pbuffer++;
    }
    /* deselect the flash: chip select high */
    SPI_FLASH_CS_HIGH();
    /* disable the qspi */
    qspi_disable(SPI0);
		/* wait the end of flash writing */
    spi_flash_wait_for_write_end();
}
void qspi_flash_page_write(uint8_t* pbuffer, uint32_t write_addr, uint16_t num_byte_to_write)
{
    /* enable the flash quad mode */
    qspi_flash_quad_enable();
    /* enable the write access to the flash */
    spi_flash_write_enable();

    /* select the flash: chip select low */
    SPI_FLASH_CS_LOW();
    /* send "quad write to memory " instruction */
    spi_flash_send_byte(QUADWRITE);
    /* send writeaddr high nibble address byte to write to */
    spi_flash_send_byte((write_addr & 0xFF0000) >> 16);
    /* send writeaddr medium nibble address byte to write to */
    spi_flash_send_byte((write_addr & 0xFF00) >> 8);
    /* send writeaddr low nibble address byte to write to */
    spi_flash_send_byte(write_addr & 0xFF);
    /* enable the qspi */ 
    qspi_enable(SPI0); 
    /* enable the qspi write operation */
    qspi_write_enable(SPI0);

    /* while there is data to be written on the flash */
    while(num_byte_to_write--){
        /* send the current byte */
        spi_flash_send_byte(*pbuffer);
        /* point on the next byte to be written */
        pbuffer++;
    }

    /* deselect the flash: chip select high */
    SPI_FLASH_CS_HIGH();
    /* disable the qspi function */ 
    qspi_disable(SPI0); 
    /* wait the end of flash writing */
    spi_flash_wait_for_write_end();
}
#endif
/*============================= spi 操作接口 ==============================*/

#ifdef SFUD_USING_QSPI
/**
 * read flash data by QSPI
 */
static sfud_err qspi_read(const struct __sfud_spi *spi, uint32_t addr, sfud_qspi_read_cmd_format *qspi_read_cmd_format,
        uint8_t *read_buf, size_t read_size) {
    sfud_err result = SFUD_SUCCESS;

    /**
     * add your qspi read flash data code
     */
		qspi_flash_buffer_read(read_buf, addr, read_size);
    return result;
}
#endif /* SFUD_USING_QSPI */

sfud_err sfud_spi_port_init(sfud_flash *flash) {
    sfud_err result = SFUD_SUCCESS;

    /**
     * add your port spi bus and device object initialize code like this:
     * 1. rcc initialize
     * 2. gpio initialize
     * 3. spi device initialize
     * 4. flash->spi and flash->retry item initialize
     *    flash->spi.wr = spi_write_read; //Required
     *    flash->spi.qspi_read = qspi_read; //Required when QSPI mode enable
     *    flash->spi.lock = spi_lock;
     *    flash->spi.unlock = spi_unlock;
     *    flash->spi.user_data = &spix;
     *    flash->retry.delay = null;
     *    flash->retry.times = 10000; //Required
     */
    spi_flash_init();
    spi_Lock_Init();
#ifdef SFUD_USING_QSPI
    flash->spi.wr = qspi_read;
#else
    flash->spi.wr = spi_write_read;
#endif
    flash->spi.lock = spi_lock;
    flash->spi.unlock = spi_unlock;
    flash->spi.user_data = &spi_userData;
    flash->retry.delay = NULL;
    flash->retry.times = 60*10000; //60 Sec
 
    return result;
}



