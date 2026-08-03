#ifndef _PERIPH_SPI_ADC_H_
#define _PERIPH_SPI_ADC_H_

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "gd32f4xx.h"
#include "utils.h"

#define TLC2543_CS_LOW()    gpio_bit_reset(GPIOF, GPIO_PIN_11)
#define TLC2543_CS_HIGH()   gpio_bit_set(GPIOF, GPIO_PIN_11)

void tlc2543_spi_init(void);

uint16_t tlc2543_read_adc(uint8_t chan);

#endif
