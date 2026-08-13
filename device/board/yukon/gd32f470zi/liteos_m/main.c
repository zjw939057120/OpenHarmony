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
#include "main.h"
#include "main_task.h"

int main(void)
{
	SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_TRIM);
    SEGGER_RTT_WriteString(0, "SEGGER Real-Time-Terminal Sample\r\n");
	// 初始化SysTick
	systick_config();
	// 初始化LiteOS
	if (LOS_KernelInit() != LOS_OK) {
		return -1;
	}
	// 初始化主任务
	main_task_init();
	// 启动任务
	LOS_Start();

    return 0;
}
