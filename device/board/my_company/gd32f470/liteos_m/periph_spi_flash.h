#ifndef _PERIPH_SPI_FLASH_H_
#define _PERIPH_SPI_FLASH_H_

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "gd32f4xx.h"
#include "utils.h"
#include "driver/norflash/bsp_w25qxx.h"
#include "driver/norflash/w25qxx.h"

void periph_spi_flash_init(void);

void spi_flash_test_1(void);

void spi_flash_test_2(void);

#endif
