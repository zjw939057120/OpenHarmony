/*
 * Copyright (c) 2022 PandaBoard.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _FMC_OPERATION_H_
#define _FMC_OPERATION_H_

#include "gd32f4xx_fmc.h"

/* base addr of the FMC sectors */
#define ADDR_FMC_SECTOR_0                   ((unsigned int)0x08000000) /*!< base addr of sector 0,  16 kbytes */
#define ADDR_FMC_SECTOR_1                   ((unsigned int)0x08004000) /*!< base addr of sector 1,  16 kbytes */
#define ADDR_FMC_SECTOR_2                   ((unsigned int)0x08008000) /*!< base addr of sector 2,  16 kbytes */
#define ADDR_FMC_SECTOR_3                   ((unsigned int)0x0800C000) /*!< base addr of sector 3,  16 kbytes */
#define ADDR_FMC_SECTOR_4                   ((unsigned int)0x08010000) /*!< base addr of sector 4,  64 kbytes */
#define ADDR_FMC_SECTOR_5                   ((unsigned int)0x08020000) /*!< base addr of sector 5, 128 kbytes */
#define ADDR_FMC_SECTOR_6                   ((unsigned int)0x08040000) /*!< base addr of sector 6, 128 kbytes */
#define ADDR_FMC_SECTOR_7                   ((unsigned int)0x08060000) /*!< base addr of sector 7, 128 kbytes */
#define ADDR_FMC_SECTOR_8                   ((unsigned int)0x08080000) /*!< base addr of sector 8, 128 kbytes */
#define ADDR_FMC_SECTOR_9                   ((unsigned int)0x080A0000) /*!< base addr of sector 9, 128 kbytes */
#define ADDR_FMC_SECTOR_10                  ((unsigned int)0x080C0000) /*!< base addr of sector 10,128 kbytes */
#define ADDR_FMC_SECTOR_11                  ((unsigned int)0x080E0000) /*!< base addr of sector 11,128 kbytes */ /* 1M */
#define ADDR_FMC_SECTOR_12                  ((unsigned int)0x08100000) /*!< base addr of sector 12, 16 kbytes */
#define ADDR_FMC_SECTOR_13                  ((unsigned int)0x08104000) /*!< base addr of sector 13, 16 kbytes */
#define ADDR_FMC_SECTOR_14                  ((unsigned int)0x08108000) /*!< base addr of sector 14, 16 kbytes */
#define ADDR_FMC_SECTOR_15                  ((unsigned int)0x0810C000) /*!< base addr of sector 15, 16 kbytes */
#define ADDR_FMC_SECTOR_16                  ((unsigned int)0x08110000) /*!< base addr of sector 16, 64 kbytes */
#define ADDR_FMC_SECTOR_17                  ((unsigned int)0x08120000) /*!< base addr of sector 17,128 kbytes */
#define ADDR_FMC_SECTOR_18                  ((unsigned int)0x08140000) /*!< base addr of sector 18,128 kbytes */
#define ADDR_FMC_SECTOR_19                  ((unsigned int)0x08160000) /*!< base addr of sector 19,128 kbytes */
#define ADDR_FMC_SECTOR_20                  ((unsigned int)0x08180000) /*!< base addr of sector 20,128 kbytes */
#define ADDR_FMC_SECTOR_21                  ((unsigned int)0x081A0000) /*!< base addr of sector 21,128 kbytes */
#define ADDR_FMC_SECTOR_22                  ((unsigned int)0x081C0000) /*!< base addr of sector 22,128 kbytes */
#define ADDR_FMC_SECTOR_23                  ((unsigned int)0x081E0000) /*!< base addr of sector 23,128 kbytes */ /* 2M */
#define ADDR_FMC_SECTOR_24                  ((unsigned int)0x08200000) /*!< base addr of sector 24,256 kbytes */
#define ADDR_FMC_SECTOR_25                  ((unsigned int)0x08240000) /*!< base addr of sector 25,256 kbytes */
#define ADDR_FMC_SECTOR_26                  ((unsigned int)0x08280000) /*!< base addr of sector 26,256 kbytes */
#define ADDR_FMC_SECTOR_27                  ((unsigned int)0x082C0000) /*!< base addr of sector 27,256 kbytes */ /* 3M */


/* 按地址进行擦除，擦除的是当前扇区，注意保存数据 */
void fmc_erase_address(unsigned int addr);

/* 自动计算要擦除的扇区，建议起始为扇区的起始，否则有擦除到有用数据的风险 */
void fmc_erase_address_lenght(unsigned int addr, unsigned int len);

void fmc_erase_page(unsigned int addr, unsigned int len);

/* 读取地址中数据 */
void fmc_read(unsigned int addr, unsigned int len, unsigned char *data);

/* 写入数据到地址中，前提是需要先擦除后写 */
void fmc_write(unsigned int addr, unsigned int len, unsigned char *data);

#endif /* _FMC_OPERATION_H_ */
