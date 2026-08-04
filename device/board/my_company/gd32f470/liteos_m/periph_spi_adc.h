#ifndef _PERIPH_SPI_ADC_H_
#define _PERIPH_SPI_ADC_H_

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "gd32f4xx.h"


// TLC2543 片选引脚 (PF11)
#define TLC2543_CS_LOW()    gpio_bit_reset(GPIOF, GPIO_PIN_11)
#define TLC2543_CS_HIGH()   gpio_bit_set(GPIOF, GPIO_PIN_11)
// AD5318 片选引脚 (PB2)
#define AD5318_SYNC_LOW()   gpio_bit_reset(GPIOB, GPIO_PIN_2)
#define AD5318_SYNC_HIGH()  gpio_bit_set(GPIOB, GPIO_PIN_2)

void tlc2543_spi_init(void);

uint16_t tlc2543_read_adc(uint8_t chan);

void ad5318_spi_init(void);

void ad5318_write_dac(uint8_t channel, uint16_t data);

#endif
