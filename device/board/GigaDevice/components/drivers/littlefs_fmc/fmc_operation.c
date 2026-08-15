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

#include "fmc_operation.h"

#ifdef GD32F470
#define APP_LOADED_ADDR                     ADDR_FMC_SECTOR_4                               /* BOOT 结束地址 */
#define IS_PROTECTED_AREA(addr)             (unsigned char)(((addr >= ADDR_FMC_SECTOR_0) && \
                                            (addr < (APP_LOADED_ADDR)))? 1U : 0U)           /* 确保写入的数据不在bootloader里 */
#endif

#ifdef GD32F425
#define APP_LOADED_ADDR                     ADDR_FMC_SECTOR_3                               /* BOOT 结束地址 */
#define IS_PROTECTED_AREA(addr)             (unsigned char)(((addr >= ADDR_FMC_SECTOR_0) && \
                                            (addr < (APP_LOADED_ADDR)))? 1U : 0U)           /* 确保写入的数据不在bootloader里 */
#endif

/* sector size */
#define SIZE_16KB                           ((unsigned int)0x00004000U)        /*!< size of 16KB*/
#define SIZE_64KB                           ((unsigned int)0x00010000U)        /*!< size of 64KB*/
#define SIZE_128KB                          ((unsigned int)0x00020000U)        /*!< size of 128KB*/
#define SIZE_256KB                          ((unsigned int)0x00040000U)        /*!< size of 256KB*/

/* FMC BANK addr */
#define FMC_START_ADDRESS                   FLASH_BASE                               /*!< FMC start addr */
#define FMC_BANK0_START_ADDRESS             FMC_START_ADDRESS                        /*!< FMC BANK0 start addr */
#define FMC_BANK1_START_ADDRESS             ((unsigned int)0x08100000U)              /*!< FMC BANK1 start addr */
#define FMC_SIZE                            (*(unsigned short int *)0x1FFF7A22U)     /*!< FMC SIZE */
#define FMC_END_ADDRESS                     (FLASH_BASE + (FMC_SIZE * 1024) - 1)     /*!< FMC end addr */
#define FMC_MAX_END_ADDRESS                 ((unsigned int)0x08300000U)              /*!< FMC maximum end addr */

/* FMC error message */
#define FMC_WRONG_SECTOR_NAME               ((unsigned int)0xFFFFFFFFU)        /*!< wrong sector name*/
#define FMC_WRONG_SECTOR_NUM                ((unsigned int)0xFFFFFFFFU)        /*!< wrong sector number*/
#define FMC_INVALID_SIZE                    ((unsigned int)0xFFFFFFFFU)        /*!< invalid sector size*/
#define FMC_INVALID_ADDR                    ((unsigned int)0xFFFFFFFFU)        /*!< invalid sector addr*/

typedef struct
{
    unsigned int sector_name;                                         /*!< the name of the sector */
    unsigned int sector_num;                                          /*!< the number of the sector */
    unsigned int sector_size;                                         /*!< the size of the sector */
    unsigned int sector_start_addr;                                   /*!< the start addr of the sector */
    unsigned int sector_end_addr;                                     /*!< the end addr of the sector */
} fmc_sector_info_struct;

static fmc_sector_info_struct fmc_sector_info_get(unsigned int addr)
{
    fmc_sector_info_struct sector_info;
    unsigned int temp = 0x00000000U;
    if((FMC_START_ADDRESS <= addr)&&(FMC_END_ADDRESS >= addr)) {
        if ((FMC_BANK1_START_ADDRESS > addr)) {
            /* bank0 area */
            temp = (addr - FMC_BANK0_START_ADDRESS) / SIZE_16KB;
            if (4U > temp) {
                sector_info.sector_name = (unsigned int)temp;
                sector_info.sector_num = CTL_SN(temp);
                sector_info.sector_size = SIZE_16KB;
                sector_info.sector_start_addr = FMC_BANK0_START_ADDRESS + (SIZE_16KB * temp);
                sector_info.sector_end_addr = sector_info.sector_start_addr + SIZE_16KB - 1;
            } else if (8U > temp) {
                sector_info.sector_name = 0x00000004U;
                sector_info.sector_num = CTL_SN(4);
                sector_info.sector_size = SIZE_64KB;
                sector_info.sector_start_addr = 0x08010000U;
                sector_info.sector_end_addr = 0x0801FFFFU;
            } else {
                temp = (addr - FMC_BANK0_START_ADDRESS) / SIZE_128KB;
                sector_info.sector_name = (unsigned int)(temp + 4);
                sector_info.sector_num = CTL_SN(temp + 4);
                sector_info.sector_size = SIZE_128KB;
                sector_info.sector_start_addr = FMC_BANK0_START_ADDRESS + (SIZE_128KB * temp);
                sector_info.sector_end_addr = sector_info.sector_start_addr + SIZE_128KB - 1;
            }
        } else {
            /* bank1 area */
            temp = (addr - FMC_BANK1_START_ADDRESS) / SIZE_16KB;
            if (4U > temp) {
                sector_info.sector_name = (unsigned int)(temp + 12);
                sector_info.sector_num = CTL_SN(temp + 16);
                sector_info.sector_size = SIZE_16KB;
                sector_info.sector_start_addr = FMC_BANK0_START_ADDRESS + (SIZE_16KB * temp);
                sector_info.sector_end_addr = sector_info.sector_start_addr + SIZE_16KB - 1;
            } else if (8U > temp) {
                sector_info.sector_name = 0x00000010;
                sector_info.sector_num = CTL_SN(20);
                sector_info.sector_size = SIZE_64KB;
                sector_info.sector_start_addr = 0x08110000U;
                sector_info.sector_end_addr = 0x0811FFFFU;
            } else if (64U > temp){
                temp = (addr - FMC_BANK1_START_ADDRESS) / SIZE_128KB;
                sector_info.sector_name = (unsigned int)(temp + 16);
                sector_info.sector_num = CTL_SN(temp + 20);
                sector_info.sector_size = SIZE_128KB;
                sector_info.sector_start_addr = FMC_BANK1_START_ADDRESS + (SIZE_128KB * temp);
                sector_info.sector_end_addr = sector_info.sector_start_addr + SIZE_128KB - 1;
            } else {
                temp = (addr - FMC_BANK1_START_ADDRESS) / SIZE_256KB;
                sector_info.sector_name = (unsigned int)(temp + 20);
                sector_info.sector_num = CTL_SN(temp + 8);
                sector_info.sector_size = SIZE_256KB;
                sector_info.sector_start_addr = FMC_BANK1_START_ADDRESS + (SIZE_256KB * temp);
                sector_info.sector_end_addr = sector_info.sector_start_addr + SIZE_256KB - 1;
            }
        }
    } else {
        /* invalid addr */
        sector_info.sector_name = FMC_WRONG_SECTOR_NAME;
        sector_info.sector_num = FMC_WRONG_SECTOR_NUM;
        sector_info.sector_size = FMC_INVALID_SIZE;
        sector_info.sector_start_addr = FMC_INVALID_ADDR;
        sector_info.sector_end_addr = FMC_INVALID_ADDR;
    }
    return sector_info;
}
unsigned int sector_name_to_number(unsigned int sector_name)
{
    if(11 >= sector_name){
        return CTL_SN(sector_name);
    }else if(23 >= sector_name){
        return CTL_SN(sector_name + 4);
    }else if(27 >= sector_name){
        return CTL_SN(sector_name - 12);
    }else{
        while(1);
    }
}

void fmc_erase_address(unsigned int addr)
{
    fmc_sector_info_struct sector_info;
    sector_info = fmc_sector_info_get(addr);
    if(FMC_WRONG_SECTOR_NAME == sector_info.sector_name){
        while(1);
    }else{
        fmc_unlock(); 
        fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);
        if(FMC_READY != fmc_sector_erase(sector_info.sector_num)){
            while(1);
        }
        fmc_lock();       
    }
}

void fmc_erase_address_lenght(unsigned int addr, unsigned int len)
{
    fmc_sector_info_struct sector_start = fmc_sector_info_get(addr);;
    fmc_sector_info_struct sector_end = fmc_sector_info_get(addr+len);;

    for (fmc_sector_info_struct *i = &sector_start; i <= &sector_end; i++)
    {
        if(FMC_WRONG_SECTOR_NAME == i->sector_name){
            while(1);
        }else{
            fmc_unlock(); 
            fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);
            if(FMC_READY != fmc_sector_erase(i->sector_num)){
                while(1);
            }
            fmc_lock();       
        }
    }
}


void fmc_erase_page(unsigned int addr, unsigned int len)
{
    if ((addr%4096) == 0)
    {
        fmc_unlock(); 
        fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);
        if(FMC_READY != fmc_page_erase(addr)){
            while(1);
        }
        fmc_lock();
    }
}

void fmc_write(unsigned int addr, unsigned int len, unsigned char *data)
{
    /* 检查是否操作 boot 分区 */
    if (IS_PROTECTED_AREA(addr)) {
        return;
    }

    // fmc_sector_info_struct sector_info;
    // sector_info = fmc_sector_info_get(addr);
    // if (addr == sector_info.sector_start_addr)
    // {
    //     fmc_erase_address(addr);
    // }
    // if ((addr+len) >= sector_info.sector_end_addr)
    // {
    //     fmc_erase_address(addr+len);
    // }
    
    /* unlock the flash program erase controller */
    fmc_unlock();
    /* clear pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);

    /* write data_8 to the corresponding addr */
    for(int i=0; i<len; i++){
        // printf("0x%02x ", (unsigned char)data_8[i]);
        if(FMC_READY == fmc_byte_program(addr, (unsigned char)data[i])){
            addr = addr + 1;
        }else{
            while(1);
        }
    }
    /* lock the flash program erase controller */
    fmc_lock();
}

void fmc_read(unsigned int addr, unsigned int len, unsigned char *data)
{                           
    unsigned int i;
    for(i=0; i<len; i++){
        data[i] = *(__IO unsigned char*)addr;
        addr = addr + 1;
    }
}
