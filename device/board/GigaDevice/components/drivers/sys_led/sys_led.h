/*
 * Copyright (c) 2022 PandaBoard.
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

#ifndef __SYS_LED_H__
#define __SYS_LED_H__

#include "gd32f4xx.h"
#include "gd32f4xx_gpio.h"
#include "gd32f4xx_rcu.h"
#include "gd32f4xx_timer.h"

#ifdef __cplusplus
extern "C" {
#endif

/* LED 定义 指示灯引脚 */
#if (defined GD32F470VI) || (defined GD32F407VE)
#define LED_PIN                         GPIO_PIN_8
#define LED_PORT                        GPIOA
#define LED_CLK                         RCU_GPIOA
#endif

void system_led_init(void);
void system_led_on(void);
void system_led_off(void);

void system_led_pwm_init(void);
void system_led_pwm_duty(int duty);

#ifdef __cplusplus
}
#endif

#endif /* __SYS_RUN_LED_H__ */
