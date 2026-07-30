/**
  ******************************************************************************
  * @file    bsp_spi2.c
  * @author  embedfire
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */


#include "bsp_w25qxx.h"

void SPI_Flash_Config()
{
    spi_parameter_struct spi_init_struct;

    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_SPI2);

    /* SPI2_CLK(PC10), SPI2_MISO(PC11), SPI2_MOSI(PC12) GPIO pin configuration */
    gpio_af_set(GPIOC, GPIO_AF_6, GPIO_PIN_10|GPIO_PIN_11| GPIO_PIN_12);
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_10|GPIO_PIN_11| GPIO_PIN_12);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO_PIN_10|GPIO_PIN_11| GPIO_PIN_12);

    /* SPI2_CS(PD2) GPIO pin configuration */
    gpio_mode_set(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_2);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);

    /* chip select invalid */
    W25QXX_CS_HIGH();

    /* SPI2 parameter config */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_32;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI2, &spi_init_struct);

    /* enable SPI2 */
    spi_enable(SPI2);
}


/*********************************************END OF FILE**********************/
