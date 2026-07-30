/**
  ******************************************************************************
  * @file    w25qxx.c
  * @author  embedfire
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
  
#include "bsp_w25qxx.h"
#include "w25qxx.h"
#include "data_test.h"
#include "los_task.h"

uint32_t w25q_sector_count;
uint32_t w25q_use_4_byte_addr;


/**
  * @brief  SPI2 发送并接收一个字节
  * @param  byte: 要发送的字节
  * @retval 接收到的字节
  */
uint8_t W25Q_Write_Read_Byte(uint8_t byte)
{
    /* 等待发送缓冲区为空 */
    while (spi_i2s_flag_get(SPI2, SPI_FLAG_TBE) == RESET);
    
    /* 发送数据 */
    spi_i2s_data_transmit(SPI2, byte);
    
    /* 等待接收缓冲区非空 */
    while (spi_i2s_flag_get(SPI2, SPI_FLAG_RBNE) == RESET);
    
    /* 返回接收到的数据 */
    return spi_i2s_data_receive(SPI2);
}

/**
  * @brief  SPI2 发送并接收多个字节 (收发缓冲区为同一数组)
  * @param  array: 数据缓冲区指针
  * @param  size: 数据大小
  * @retval 0: 成功, 1: 失败
  */
uint8_t W25Q_Write_Read_Multibyte(uint8_t* array, uint16_t size)
{
    uint16_t i;
    for (i = 0; i < size; i++)
    {
        /* 等待发送缓冲区为空 */
        while (spi_i2s_flag_get(SPI2, SPI_FLAG_TBE) == RESET);
        
        /* 发送数据 */
        spi_i2s_data_transmit(SPI2, array[i]);
        
        /* 等待接收缓冲区非空 */
        while (spi_i2s_flag_get(SPI2, SPI_FLAG_RBNE) == RESET);
        
        /* 读取数据并存回原数组 */
        array[i] = spi_i2s_data_receive(SPI2);
    }
    return 0;
}

/**
  * @brief  SPI2 发送多个字节
  * @param  array: 数据缓冲区指针
  * @param  size: 数据大小
  * @retval 0: 成功, 1: 失败
  */
uint8_t W25Q_Write_Multibyte(uint8_t* array, uint16_t size)
{
    uint16_t i;
    for (i = 0; i < size; i++)
    {
        /* 等待发送缓冲区为空 */
        while (spi_i2s_flag_get(SPI2, SPI_FLAG_TBE) == RESET);
        
        /* 发送数据 */
        spi_i2s_data_transmit(SPI2, array[i]);
        
        /* 等待接收完成 (等待发送缓冲区再次变空，表示数据已移出移位寄存器) */
        while (spi_i2s_flag_get(SPI2, SPI_FLAG_TBE) == RESET);
    }
    return 0;
}
 


uint8_t  W25Q_Init(void)
{
    uint8_t res = 0;
    
    uint16_t id = W25Q_ReadDeviceID();
    
    switch(id)
    {
        case W25Q64_DEVICE_ID:
        {
            w25q_sector_count = W25Q64_SECTOR_COUNT;
            break;
        }
        
        case W25Q128_DEVICE_ID:
        {
            w25q_sector_count = W25Q128_SECTOR_COUNT;
            break;
        }

        case W25Q256_DEVICE_ID:
        {
            w25q_sector_count = W25Q256_SECTOR_COUNT;           
            W25Q_Set_4ByteAddr();
            break;
        }        
        
        default:
           res = 1;      
    }  

    return res;
}



uint16_t  W25Q_ReadDeviceID(void)
{       
    W25QXX_CS_LOW();

    uint8_t cmd_buf[6] = {0};    
    cmd_buf[0] = W25Q_READ_DEVICE_ID;
    W25Q_Write_Read_Multibyte(cmd_buf,sizeof(cmd_buf));  
        
    uint16_t id = 0;    
    id |= cmd_buf[4] << 8;
    id |= cmd_buf[5]; 

    W25QXX_CS_HIGH();  

    return id;
}
    

void  W25Q_WriteEnable(void)
{
    W25QXX_CS_LOW();
    W25Q_Write_Read_Byte(W25Q_WRITE_ENABLE);     
    W25QXX_CS_HIGH();    
}



uint8_t  W25Q_SectorErase(uint32_t addr)
{
    uint8_t  res = 0;
    
    W25Q_WriteEnable();
    
    W25QXX_CS_LOW();
    if(w25q_use_4_byte_addr == 0)
    {
       uint8_t cmd_buf[4] = {0};    
       cmd_buf[0] = W25Q_SECTOR_ERASE;
       cmd_buf[1] = (addr & 0xFF0000) >> 16;
       cmd_buf[2] = (addr & 0xFF00) >> 8;
       cmd_buf[3] =  addr & 0xFF;
       W25Q_Write_Read_Multibyte(cmd_buf,sizeof(cmd_buf));        
    }
    else
    {
       uint8_t cmd_buf[5] = {0};    
       cmd_buf[0] = W25Q_SECTOR_ERASE;
       cmd_buf[1] = (addr & 0xFF000000) >> 24;
       cmd_buf[2] = (addr & 0xFF0000) >> 16;
       cmd_buf[3] = (addr & 0xFF00) >> 8;
       cmd_buf[4] =  addr & 0xFF;
       W25Q_Write_Read_Multibyte(cmd_buf,sizeof(cmd_buf));               
    }
    W25QXX_CS_HIGH();
    
    res = W25Q_WaitForWriteEnd();
    
    return res;     
}



uint8_t  W25Q_ChipErase(void)
{  
    uint8_t  res = 0;
    
    W25Q_WriteEnable();
    
    W25QXX_CS_LOW();
    W25Q_Write_Read_Byte(W25Q_CHIP_ERASE);   
    W25QXX_CS_HIGH();
  
    res = W25Q_WaitForWriteEnd();

    return res;             
}



uint8_t  W25Q_PageWrite(uint32_t addr, uint8_t* data, uint16_t size)
{
    uint8_t  res = 0;
           
    if(size > W25Q_DEFAULT_PAGE_SIZE)
    {
        res = 1;
        return res;
    }
    
    W25Q_WriteEnable();
    
    W25QXX_CS_LOW();
    if(w25q_use_4_byte_addr == 0)
    {
       uint8_t cmd_buf[4] = {0};    
       cmd_buf[0] = W25Q_PAGE_PROGRAM;
       cmd_buf[1] = (addr & 0xFF0000) >> 16;
       cmd_buf[2] = (addr & 0xFF00) >> 8;
       cmd_buf[3] =  addr & 0xFF;
       W25Q_Write_Read_Multibyte(cmd_buf,sizeof(cmd_buf));        
    }
    else
    {
       uint8_t cmd_buf[5] = {0};    
       cmd_buf[0] = W25Q_PAGE_PROGRAM;
       cmd_buf[1] = (addr & 0xFF000000) >> 24;
       cmd_buf[2] = (addr & 0xFF0000) >> 16;
       cmd_buf[3] = (addr & 0xFF00) >> 8;
       cmd_buf[4] =  addr & 0xFF;
       W25Q_Write_Read_Multibyte(cmd_buf,sizeof(cmd_buf));               
    }
     
    W25Q_Write_Multibyte(data,size);
    
    W25QXX_CS_HIGH();   
    
    res = W25Q_WaitForWriteEnd();
      
    return res;              
}



uint8_t  W25Q_ReadData(uint32_t  addr,uint8_t* data, uint16_t size)
{
    W25QXX_CS_LOW();
    
    uint8_t  res = 0;          
        
    if(w25q_use_4_byte_addr == 0)
    {
       uint8_t cmd_buf[4] = {0};    
       cmd_buf[0] = W25Q_READ_DATA;
       cmd_buf[1] = (addr & 0xFF0000) >> 16;
       cmd_buf[2] = (addr & 0xFF00) >> 8;
       cmd_buf[3] =  addr & 0xFF;
       W25Q_Write_Read_Multibyte(cmd_buf,sizeof(cmd_buf));        
    }
    else
    {
       uint8_t cmd_buf[5] = {0};    
       cmd_buf[0] = W25Q_READ_DATA;
       cmd_buf[1] = (addr & 0xFF000000) >> 24;
       cmd_buf[2] = (addr & 0xFF0000) >> 16;
       cmd_buf[3] = (addr & 0xFF00) >> 8;
       cmd_buf[4] =  addr & 0xFF;
       W25Q_Write_Read_Multibyte(cmd_buf,sizeof(cmd_buf));               
    }
       
    W25Q_Write_Read_Multibyte(data,size);
      
    W25QXX_CS_HIGH();
    
    return res;         
}



uint8_t  W25Q_FastRead(uint32_t  addr,uint8_t* data, uint16_t size)
{
    W25QXX_CS_LOW();
    
    uint8_t  res = 0;          
        
    if(w25q_use_4_byte_addr == 0)
    {
       uint8_t cmd_buf[5] = {0};    
       cmd_buf[0] = W25Q_FAST_READ;
       cmd_buf[1] = (addr & 0xFF0000) >> 16;
       cmd_buf[2] = (addr & 0xFF00) >> 8;
       cmd_buf[3] =  addr & 0xFF;
       cmd_buf[4] =  DUMMY_BYTE;
       W25Q_Write_Read_Multibyte(cmd_buf,sizeof(cmd_buf));        
    }
    else
    {
       uint8_t cmd_buf[6] = {0};    
       cmd_buf[0] = W25Q_FAST_READ;
       cmd_buf[1] = (addr & 0xFF000000) >> 24;
       cmd_buf[2] = (addr & 0xFF0000) >> 16;
       cmd_buf[3] = (addr & 0xFF00) >> 8;
       cmd_buf[4] =  addr & 0xFF;
       cmd_buf[5] =  DUMMY_BYTE;
       W25Q_Write_Read_Multibyte(cmd_buf,sizeof(cmd_buf));               
    }
       
    W25Q_Write_Read_Multibyte(data,size);
      
    W25QXX_CS_HIGH();
    
    return res;         
}



uint8_t  W25Q_WaitForWriteEnd(void)
{
    W25QXX_CS_LOW();
    
    uint32_t timeout = 1000;  
    uint8_t  reg_status_1 = 0;
    uint8_t  res = 0;
    
    W25Q_Write_Read_Byte(W25Q_READ_STATUS_1);

    do
    {
        reg_status_1 = W25Q_Write_Read_Byte(DUMMY_BYTE);	 
                    
        if(timeout == 0) 
        {
            printf("W25Q_WaitForWriteEnd  timeout\r\n"); //调好后删掉
            res = 1;
            return res;
        }
        timeout--;
        
        LOS_TaskDelay(2);
 
    }while(reg_status_1 & STATUS_1_WIP_Flag); 

    W25QXX_CS_HIGH()
    
    return res;   
}



uint8_t  W25Q_Get_Status(uint8_t cmd)
{   
   W25QXX_CS_LOW();
    
   W25Q_Write_Read_Byte(cmd); 
    
   uint8_t status = W25Q_Write_Read_Byte(DUMMY_BYTE); 
    
   W25QXX_CS_HIGH();
    
   return  status;  
}


uint8_t   W25Q_Set_4ByteAddr(void)
{
    W25QXX_CS_LOW();
    
    uint8_t res = 0;
       
    res = W25Q_Write_Read_Byte(W25Q_ENTER_4BYTE_ADDR); 
       
    w25q_use_4_byte_addr = 1;
    
    W25QXX_CS_HIGH();
    
    return res;
}


uint16_t  W25Q_Get_SectorCount(void)
{
    return w25q_sector_count;  
}



uint8_t   W25Q_BufferRead(uint32_t  addr, uint8_t* buffer, uint16_t size)
{
//   return  W25Q_ReadData(addr,buffer,size);   
   /* or */  
   return  W25Q_FastRead(addr,buffer,size);
}



uint8_t   W25Q_BufferWrite(uint32_t addr, uint8_t* buffer, uint16_t size)
{
    //=== 1. 参数与边界检查 ===//
    if (size == 0) // 如果写入长度为0，直接返回成功
        return 0;
//    if (addr + size > W25Q_MAX_ADDR) // 防止越界写入（根据芯片容量设置）
//        return 1; // 超出地址范围，返回错误

    //=== 2. 初始化变量（使用更大的类型，防止溢出）===//
    uint32_t num_of_page = 0;   // 完整页的数量
    uint16_t num_of_single = 0; // 最后一页不足一页的字节数
    uint16_t count = 0;         // 当前页剩余的字节数
    uint16_t temp = 0;           // 跨页后剩余的字节数
    uint16_t temp_addr = 0;     // 当前地址在页内的偏移

    //=== 3. 计算对齐参数 ===//
    // 当前地址在页内的偏移（相当于addr % W25Q_DEFAULT_PAGE_SIZE）
    temp_addr = addr & (W25Q_DEFAULT_PAGE_SIZE - 1);
    // 当前页剩余的可写空间（如果temp_addr为0，则count为一整页大小）
    count = W25Q_DEFAULT_PAGE_SIZE - temp_addr;

    //=== 4. 判断写入模式 ===//
    //（1）如果地址正好对齐（temp_addr == 0），写入逻辑会更简单
    if (temp_addr == 0) // 地址对齐
    {
        // 计算需要写入的完整页数和剩余的尾部字节数
        num_of_page = size / W25Q_DEFAULT_PAGE_SIZE;
        num_of_single = size % W25Q_DEFAULT_PAGE_SIZE;

        //（1.1）写入完整的整数页
        while (num_of_page--)
        {
            W25Q_PageWrite(addr, buffer, W25Q_DEFAULT_PAGE_SIZE);
            addr += W25Q_DEFAULT_PAGE_SIZE;
            buffer += W25Q_DEFAULT_PAGE_SIZE;
        }

        //（1.2）写入不足一页的尾部数据（如果有的话）
        if (num_of_single != 0)
            W25Q_PageWrite(addr, buffer, num_of_single);
    }
    //（2）如果地址没有对齐（temp_addr != 0），需要先填满当前页
    else
    {
        // 计算写入后还剩余多少字节
        // （size > count时，size - count > 0；size <= count时，size - count <= 0）
        temp = (size > count) ? (size - count) : 0;

        //（2.1）先写满当前页
        W25Q_PageWrite(addr, buffer, (size > count) ? count : size);
        addr += count;
        buffer += count;

        //（2.2）如果还有剩余数据，则继续写入后面的完整页和尾部
        if (temp > 0)
        {
            // 计算剩余数据中完整的页数和尾部字节数
            num_of_page = temp / W25Q_DEFAULT_PAGE_SIZE;
            num_of_single = temp % W25Q_DEFAULT_PAGE_SIZE;

            // 写入完整的整数页
            while (num_of_page--)
            {
                W25Q_PageWrite(addr, buffer, W25Q_DEFAULT_PAGE_SIZE);
                addr += W25Q_DEFAULT_PAGE_SIZE;
                buffer += W25Q_DEFAULT_PAGE_SIZE;
            }

            // 写入不足一页的尾部数据（如果有的话）
            if (num_of_single != 0)
                W25Q_PageWrite(addr, buffer, num_of_single);
        }
    }

    return 0; // 写入成功
}


uint8_t   W25Q_Chip_Test(void)
{
    uint16_t sector_count = W25Q_Get_SectorCount();//获取扇区数
    uint8_t page_write[256] ={0};//定义写入数组
    uint8_t page_read[256] ={0};//定义读取数组
    uint32_t error_addr[500] ={0};//记录对比错误的地址
    uint16_t i = 0;//错误序号
    
    fill_sequential_byte_buffer(page_write,sizeof(page_write));//填充一页数据
    
    for(uint32_t addr = 0; addr < sector_count*16*256; addr += 0x1000)//总数：扇区*16页*256字节，每次一个扇区的移动
    {
        W25Q_SectorErase(addr);//扇区擦除4KB
        for(uint8_t page = 0; page < 16; page++)//每页写入并读取对比打印
        {
            uint32_t test_addr = addr+page*256;//每页256字节移动
            W25Q_PageWrite(test_addr,page_write,sizeof(page_write));//写入每页的256字节
            W25Q_BufferRead(test_addr,page_read,256);//读取出来256字节
            if(compare_bufferrs(page_write,page_read,sizeof(page_read)) == 1)//如果对比有问题记录地址
            {
                error_addr[i] = test_addr;
                i++;
                printf("ERROR Address:0x%x !!!\n\n",test_addr);
            }
            printf("Address testing:0x%x\n",test_addr);
            memset(page_read, 0x00, sizeof(page_read));//读取的数组置零
        }
    }
    if(i == 0)//没有错误
    {
        printf("测试结束，全片测试通过\n");
        return 0;
    }
    else
    {
        printf("测试结束，有测试失败地址%d个\n",i);
        for(uint16_t j = 0; j<=i ; j++)
        {
            printf("ERROR Address:0x%x\n",error_addr[j]);
        }
        return 1;
    }
    
    
}
/*********************************************END OF FILE**********************/
