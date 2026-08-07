#ifndef __BSP_W25QXX_H
#define	__BSP_W25QXX_H

#include "gd32f4xx.h"

/************************************************************/

#define W25QXX_SPI_SCK_PIN                    GPIO_PIN_10
#define W25QXX_SPI_SCK_GPIO_PORT              GPIOC

#define W25QXX_SPI_MISO_PIN                   GPIO_PIN_11
#define W25QXX_SPI_MISO_GPIO_PORT             GPIOC

#define W25QXX_SPI_MOSI_PIN                   GPIO_PIN_12
#define W25QXX_SPI_MOSI_GPIO_PORT             GPIOC

#define W25QXX_CS_PIN                   GPIO_PIN_2
#define W25QXX_CS_GPIO_PORT             GPIOD


//需要根据手册时序来控制拉高拉低，而不是单纯使用芯片时拉高拉低
#define W25QXX_CS_LOW()                 gpio_bit_write(W25QXX_CS_GPIO_PORT,W25QXX_CS_PIN,RESET);
#define W25QXX_CS_HIGH()                gpio_bit_write(W25QXX_CS_GPIO_PORT,W25QXX_CS_PIN,SET);


#define SPI2_DEFAULT_TIMEOUT             1000


void    SPI_Flash_Config(void);


#endif /* __BSP_W25QXX_H */
