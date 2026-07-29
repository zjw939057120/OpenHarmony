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

static void *thread_di_task(unsigned int arg)
{
	while(1) {
		uint8_t di1 = di_read(DI1_INDEX);
		uint8_t di2 = di_read(DI2_INDEX);
		uint8_t di3 = di_read(DI3_INDEX);
		uint8_t di4 = di_read(DI4_INDEX);
		uint8_t di5 = di_read(DI5_INDEX);
		uint8_t di6 = di_read(DI6_INDEX);
		uint8_t di = di1 | (di2  << 1) | (di3 << 2) | (di4 << 3) | (di5 << 4) | (di6 << 5);
		printf("di: %d\r\n", di);
		LOS_TaskDelay(1000);
	}

    return NULL;
}

UINT32 di_task_init(void)
{
	UINT32 diTaskID;
	TSK_INIT_PARAM_S stTask = {
		.pfnTaskEntry = thread_di_task,
		.uwStackSize = 0x1000,
		.pcName = "diTask",
		.usTaskPrio = 6,
	};
	if (LOS_TaskCreate(&diTaskID, &stTask) != LOS_OK) {
		return -1;
	}
	return diTaskID;
}
