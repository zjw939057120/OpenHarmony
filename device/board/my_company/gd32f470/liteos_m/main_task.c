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
#include "los_task.h"
#include "gd32f4xx.h"
#include "utils.h"
#include "main_task.h"

static void *thread_main_task(unsigned int arg)
{
	while(1) {
		led_on(RUNSTA_LED_INDEX);
        LOS_TaskDelay(200);
		led_off(RUNSTA_LED_INDEX);
        LOS_TaskDelay(200);
		led_on(RUNSTA_LED_INDEX);
        LOS_TaskDelay(200);
		led_off(RUNSTA_LED_INDEX);
		LOS_TaskDelay(1000);
	}

    return NULL;
}

UINT32 main_task_init(void)
{
	UINT32 taskID;
	TSK_INIT_PARAM_S stTask = {
		.pfnTaskEntry = thread_main_task,
		.uwStackSize = 0x1000,
		.pcName = "mainTask",
		.usTaskPrio = 6,
	};
	if (LOS_TaskCreate(&taskID, &stTask) != LOS_OK) {
		return -1;
	}
	return taskID;
}
