#include "los_task.h"
#include "gd32f4xx.h"
#include "utils.h"
#include "periph_spi_flash.h"
#include "driver/norflash/bsp_w25qxx.h"
#include "driver/norflash/w25qxx.h"


void periph_spi_flash_init(void)
{
    SPI_Flash_Config();
    if(W25Q_Init() == 0)
    {
       SEGGER_RTT_printf(0, "W25Q_GetSectorCount %d \r\n", W25Q_Get_SectorCount()); 
    }
    else
    {
       SEGGER_RTT_printf(0, "W25Q_Init Error \r\n");   
    }
    spi_flash_test_1();
}

void spi_flash_test_1(void)
{
   SEGGER_RTT_printf(0, "W25Q_READ_STATUS_1 0x%x \r\n", W25Q_Get_Status(W25Q_READ_STATUS_1));      
   SEGGER_RTT_printf(0, "W25Q_READ_STATUS_2 0x%x \r\n", W25Q_Get_Status(W25Q_READ_STATUS_2));        
   SEGGER_RTT_printf(0, "W25Q_READ_STATUS_3 0x%x \r\n", W25Q_Get_Status(W25Q_READ_STATUS_3));   
}

#define  NORFLASH_TEST_ADDR   0

void spi_flash_test_2(void)
{
   char write_Buffer[512];
   char read_Buffer[512];
   W25Q_SectorErase(NORFLASH_TEST_ADDR);
   char temp_char1[] = "https://doc.embedfire.com/";
   memcpy(write_Buffer,temp_char1,sizeof(temp_char1));   
   W25Q_BufferWrite(NORFLASH_TEST_ADDR,write_Buffer,sizeof(temp_char1));
   W25Q_BufferRead(NORFLASH_TEST_ADDR,read_Buffer,sizeof(temp_char1));
   SEGGER_RTT_printf(0, "read_Buffer %s \r\n", read_Buffer);  
}
