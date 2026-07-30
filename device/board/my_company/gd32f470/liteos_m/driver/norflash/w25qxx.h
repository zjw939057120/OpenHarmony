#ifndef __W25QXX_H
#define __W25QXX_H

#include <stdint.h>

#define   W25Q_DEFAULT_PAGE_SIZE      256 

#define   W25Q64_DEVICE_ID            0xEF16      
#define   W25Q128_DEVICE_ID           0xEF17  	  
#define   W25Q256_DEVICE_ID           0xEF18    

#define   W25Q64_SECTOR_COUNT         2048// 128*16    
#define   W25Q128_SECTOR_COUNT        4096// 256*16  	  
#define   W25Q256_SECTOR_COUNT        8192// 512*16    


#define   W25Q_WRITE_ENABLE           0x06
#define   W25Q_WRITE_DISABLE          0x04
#define   W25Q_READ_DEVICE_ID         0x90
#define   W25Q_READ_DATA              0x03
#define   W25Q_FAST_READ              0x0B
#define   W25Q_PAGE_PROGRAM           0x02
#define   W25Q_SECTOR_ERASE           0x20
#define   W25Q_BLOCK_ERASE_64K        0xD8
#define   W25Q_CHIP_ERASE             0xC7
#define   W25Q_READ_STATUS_1          0x05
#define   W25Q_WRITE_STATUS_1         0x01
#define   W25Q_READ_STATUS_2          0x35
#define   W25Q_WRITE_STATUS_2         0x31
#define   W25Q_READ_STATUS_3          0x15
#define   W25Q_WRITE_STATUS_3         0x11
#define   W25Q_READ_SFDP              0x5A
#define   W25Q_POWER_DOWN             0xB9
#define   W25Q_ENABLE_RESET           0x66
#define   W25Q_RESET_DEVICE           0x99
#define   W25Q_ENTER_4BYTE_ADDR       0xB7
#define   W25Q_EXIT_4BYTE_ADDR        0xE9



#define   STATUS_1_WIP_Flag           0x01 
#define   DUMMY_BYTE                  0xFF


uint8_t   W25Q_Init(void);
uint16_t  W25Q_ReadDeviceID(void);

void      W25Q_WriteEnable(void);
uint8_t   W25Q_SectorErase(uint32_t addr);
uint8_t   W25Q_ChipErase(void);

uint8_t   W25Q_PageWrite(uint32_t addr,uint8_t* data, uint16_t size);
uint8_t   W25Q_ReadData(uint32_t  addr,uint8_t* data, uint16_t size);
uint8_t   W25Q_FastRead(uint32_t  addr,uint8_t* data, uint16_t size);

uint8_t   W25Q_WaitForWriteEnd(void);
uint8_t   W25Q_Get_Status(uint8_t cmd);
uint8_t   W25Q_Set_4ByteAddr(void);
uint16_t  W25Q_Get_SectorCount(void);

uint8_t   W25Q_BufferRead(uint32_t  addr, uint8_t* buffer, uint16_t size);
uint8_t   W25Q_BufferWrite(uint32_t addr, uint8_t* buffer, uint16_t size);

uint8_t   W25Q_Chip_Test(void);

#endif //__W25QXX_H

