#include <stdio.h>
#include <string.h>
#include "los_arch_interrupt.h"
#include "los_interrupt.h"
#include "los_sem.h"
#include "los_task.h"
#include "los_event.h"
#include "gd32f4xx.h"
#include "utils.h"
#include "periph_spi_adc.h"


void tlc2543_spi_init(void)
{
    spi_parameter_struct spi_init_struct;

    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_SPI1);

    /* SPI1_CLK(PB13), SPI1_MISO(PB14), SPI1_MOSI(PB15) GPIO pin configuration */
    gpio_af_set(GPIOB, GPIO_AF_5, GPIO_PIN_13|GPIO_PIN_14| GPIO_PIN_15);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_13|GPIO_PIN_14| GPIO_PIN_15);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO_PIN_13|GPIO_PIN_14| GPIO_PIN_15);

    /* SPI1_CS(PF11) GPIO pin configuration */
    rcu_periph_clock_enable(RCU_GPIOF);
    gpio_mode_set(GPIOF, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_11);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11);

    /* chip select invalid */
    TLC2543_CS_HIGH();

    /* SPI1 parameter config */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_16BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_64;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI1, &spi_init_struct);

    /* enable SPI1 */
    spi_enable(SPI1);
}

/**
 * @brief 读取 TLC2543 指定通道的 ADC 值
 * @param chan: 通道号 (0 ~ 10)
 * @return 16位原始数据 (高12位有效)
 */
uint16_t tlc2543_read_adc(uint8_t chan)
{
    uint16_t control_word = 0;
    uint16_t adc_data = 0;

    // 拼接控制字：
    // 位 15-12: 通道选择 (0000 ~ 1010)
    // 位 11-10: 输出数据长度 (11 = 16位)
    // 位 9-8  : 数据格式 (00 = 单极性)
    // 位 7-4  : 保留
    // 位 3-0  : 内部测试模式等 (0000 = 正常工作)
    control_word = ((uint16_t)(chan & 0x0F) << 12) | 0x0C00; 

    // 1. 拉低片选
    TLC2543_CS_LOW();

    // 2. 发送控制字，同时接收上一次转换的结果
    spi_i2s_data_transmit(SPI1, control_word);
    while(RESET == spi_i2s_flag_get(SPI1, SPI_FLAG_TBE));     // 等待发送缓冲区空
    while(RESET == spi_i2s_flag_get(SPI1, SPI_FLAG_RBNE));    // 等待接收缓冲区非空
    adc_data = spi_i2s_data_receive(SPI1);                    // 读取数据

    // 3. 拉高片选，结束本次通讯
    TLC2543_CS_HIGH();

    // 4. 返回高12位有效数据
    return adc_data >> 4;
}

/**
 * @brief 初始化 AD5318 所需的 SPI1 及 GPIO
 */
void ad5318_spi_init(void)
{
    spi_parameter_struct spi_init_struct;

    /* 1. 开启时钟 */
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_SPI1);

    /* 2. 配置 SPI1 引脚 PB13(SCK), PB14(MISO), PB15(MOSI) 为复用推挽输出 (AF5) */
    gpio_af_set(GPIOB, GPIO_AF_5, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13 | GPIO_PIN_15);
    
    /* 3. 配置 AD5318 的 SYNC 引脚 (PB2) 为普通 GPIO 输出 */
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_2);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);
    AD5318_SYNC_HIGH(); // 初始状态拉高，不选中

    /* 4. 配置 SPI1 参数 */
    spi_struct_para_init(&spi_init_struct);
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_16BIT;     // 16位数据帧
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;   // CPOL=0, CPHA=0
    spi_init_struct.nss                  = SPI_NSS_SOFT;            // 软件控制 NSS
    spi_init_struct.prescale             = SPI_PSC_32;              // 预分频
    spi_init_struct.endian               = SPI_ENDIAN_MSB;          // MSB 优先
    
    spi_init(SPI1, &spi_init_struct);
    spi_enable(SPI1);
}

/**
 * @brief 向 AD5318 指定通道写入数据
 * @param channel: 通道号 (0~7，对应 OUT A ~ OUT H)
 * @param data: 10位 DAC 数据 (0 ~ 1023)
 */
void ad5318_write_dac(uint8_t channel, uint16_t data)
{
    uint16_t control_word = 0;

    // 【关键】数据限幅保护，防止传入越界数据导致输出异常
    if (data > 1023) {
        data = 1023; 
    }

    // 拼接 16位控制字
    // Bit 15:    0 (DAC Write 命令)
    // Bit 14-12: 通道号 (000 ~ 111)
    // Bit 11-2:  10位数据 (D9 ~ D0)
    // Bit 1-0:   00 (忽略位，补0)
    control_word = ((channel & 0x07) << 12) | ((data & 0x03FF) << 2);

    // 1. 拉低 SYNC，开始通讯
    AD5318_SYNC_LOW();

    // 2. 发送 16位数据
    spi_i2s_data_transmit(SPI1, control_word);
    while(RESET == spi_i2s_flag_get(SPI1, SPI_FLAG_TBE));     // 等待发送缓冲区空
    
    // 必须等待 SPI 彻底不忙（移位寄存器排空）
    while(SET == spi_i2s_flag_get(SPI1, SPI_FLAG_TRANS));     // 等待传输完成
    
    // 3. 清空接收缓冲区标志（全双工模式下防止溢出）
    if(spi_i2s_flag_get(SPI1, SPI_FLAG_RBNE) != RESET) {
        spi_i2s_data_receive(SPI1); 
    }

    // 4. 拉高 SYNC，结束本次通讯
    AD5318_SYNC_HIGH();
}