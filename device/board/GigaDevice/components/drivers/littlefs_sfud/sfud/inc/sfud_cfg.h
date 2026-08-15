/*
 * This file is part of the Serial Flash Universal Driver Library.
 *
 * Copyright (c) 2016-2018, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: It is the configure head file for this library.
 * Created on: 2016-04-23
 */

#ifndef _SFUD_CFG_H_
#define _SFUD_CFG_H_

#define SFUD_DEBUG_MODE		//使能SFUD的打印日志

// 如果Flash型号支持SFDP, 可以注释SFUD_USING_SFDP。可以减少需要编译的代码量。
#define SFUD_USING_SFDP		// 使能SFDP:JEDEC标准(JESD216)标准接口	注意：关闭后只会查询该库在 /sfud/inc/sfud_flash_def.h 中提供的 Flash 信息表。
							// 这样虽然会降低软件的适配性，但减少代码量。

#define SFUD_USING_FLASH_INFO_TABLE	// 是否使用该库自带的 Flash 参数信息表注意：关闭后该库只驱动支持 SFDP 规范的 Flash，也会适当的降低部分代码量。
									// 另外 2.3.2 及 2.3.3 这两个宏定义至少定义一种，也可以两种方式都选择

//支持多路外设
enum {
    SFUD_XXXX_DEVICE_INDEX = 0,
};

//把实际使用的硬件和外设接口对应配置好
#define SFUD_FLASH_DEVICE_TABLE                                                     \
{                                                                                   \
    [SFUD_XXXX_DEVICE_INDEX] = {.name = "SFUD_DRIVER_FALSH", .spi.name = "SPI0",},  \
}
//   | 名称 | 制造商 ID | 类型 ID | 容量 ID | 容量 | 写模式 | 擦除粒度（擦除的最小单位） | 擦除粒度对应的命令 |            
//  {"GD25Q64B", SFUD_MF_ID_GIGADEVICE, 0x40, 0x17, 8*1024*1024, SFUD_WM_PAGE_256B, 4096, 0x20},

// #define SFUD_USING_QSPI  

#endif /* _SFUD_CFG_H_ */
