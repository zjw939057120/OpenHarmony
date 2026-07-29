/*
 * Copyright (c) 2026 zhangyao
 *
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
#include <stdio.h>
#include "systick.h"
#include "los_task.h"
#include "gd32f4xx.h"
#include "uart_drv.h"
#include "shell_port.h"
#include "rtc_drv.h"
#include "wdg_drv.h"
#include "periph_gpio.h"
#include "periph_uart.h"
#include "main_task.h"
#include "di_task.h"

void init_hw(void)
{
	// 初始化GPIO
	init_periph_gpio();
	// 初始化按键
	init_periph_key();
	// 初始化UART
	init_periph_uart();
	
}

int main(void)
{
	// 初始化SysTick
	systick_config();
	// 初始化UART
	uartInit();

	if (LOS_KernelInit() != LOS_OK) {
		return -1;
	}

	// 初始化看门狗
	initWatchDog();
	// 初始化RTC
	initRtc();
	// 初始化硬件
	init_hw();

#if IS_ENABLED(LOSCFG_SHELL)
#if IS_ENABLED(CONFIG_USE_LETTER_SHELL)
	initLetterShell();
#else
	extern unsigned int LosShellInit(void);
	unsigned int ret = LosShellInit();
	if (ret != LOS_OK) {
		printf("LosShellInit failed! ERROR: 0x%x\n", ret);
	}
#endif
#endif

	initUartTxTask();
	uartRxIrqRegister();
	
	// 初始化主任务
	main_task_init();
	// 初始化DI任务
	di_task_init();

	LOS_Start();

    return 0;
}
