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
#include "gd32f4xx.h"
#include "los_task.h"
#include "log_printf.h"

#define CLOCK_STABLE_TIMEOUT 0x10000

static void *thread_wdg_task(unsigned int arg)
{
	while (1) {
		fwdgt_counter_reload(); //喂狗操作
        LOS_TaskDelay(3000);
	}
    return NULL;
}

void initWatchDog(void)
{
	uint32_t timeout = CLOCK_STABLE_TIMEOUT;

	rcu_osci_on(RCU_IRC32K);
	while ((SUCCESS != rcu_osci_stab_wait(RCU_IRC32K)) && (timeout--));
	if (timeout == 0) 
		log_warning("** rcu_osci_stab_wait timeout!\n");

	fwdgt_write_enable();
	fwdgt_config(2500, FWDGT_PSC_DIV64); //重装载值: 范围0-4095, reload_value = (目标时间 × 时钟频率) / 预分频值
	fwdgt_write_disable();
	fwdgt_enable();

	UINT32 wdgTaskID;
	TSK_INIT_PARAM_S stTask = {
		.pfnTaskEntry = thread_wdg_task,
		.uwStackSize = 0x400,
		.pcName = "wdgTask",
		.usTaskPrio = 3,
	};
	if (LOS_TaskCreate(&wdgTaskID, &stTask) != LOS_OK)
		printf("** LOS_TaskCreate wdgTask failed!\n");
}
