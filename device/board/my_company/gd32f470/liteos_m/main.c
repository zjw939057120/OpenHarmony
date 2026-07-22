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

#define LED_GPIO_RCU_CLOCK	RCU_GPIOD
#define LED_GPIO_PORT		GPIOD
#define LED_GPIO_PIN		GPIO_PIN_7

static inline void init_led(void)
{
	rcu_periph_clock_enable(LED_GPIO_RCU_CLOCK);
	gpio_mode_set(LED_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_GPIO_PIN);
	gpio_output_options_set(LED_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, LED_GPIO_PIN);
}

static inline void led_on(void)
{
	gpio_bit_reset(LED_GPIO_PORT, LED_GPIO_PIN);
}

static inline void led_off(void)
{
	gpio_bit_set(LED_GPIO_PORT, LED_GPIO_PIN);
}

static void *thread_main_task(unsigned int arg)
{
	init_led();

	while(1) {
		led_on();
        LOS_TaskDelay(1000);
		led_off();
        LOS_TaskDelay(1000);
	}

    return NULL;
}

int main(void)
{
	systick_config();
	uartInit();

	if (LOS_KernelInit() != LOS_OK) {
		return -1;
	}

	initWatchDog();
	initRtc();

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

	UINT32 mainTaskID;
	TSK_INIT_PARAM_S stTask = {
		.pfnTaskEntry = thread_main_task,
		.uwStackSize = 0x1000,
		.pcName = "mainTask",
		.usTaskPrio = 6,
	};
	if (LOS_TaskCreate(&mainTaskID, &stTask) != LOS_OK) {
		return -1;
	}

	LOS_Start();

    return 0;
}
