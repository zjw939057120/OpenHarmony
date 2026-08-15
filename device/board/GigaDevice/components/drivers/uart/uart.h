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

#ifndef __UART_H__
#define __UART_H__

#include <stdio.h>
#include "gd32f4xx.h"
#include "gd32f4xx_usart.h"

#include "los_compiler.h"
#include "los_event.h"
#include "los_interrupt.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UART_NUMBER                     USART5
#define UART_RCU                        RCU_USART5
#define UART_IRQ                        USART5_IRQn
#define UART_BAUD                       (9600U)

#define GPIO_RXD_RCU                    RCU_GPIOC
#define GPIO_RXD_PORT                   GPIOC
#define GPIO_RXD_PIN                    GPIO_PIN_7
#define GPIO_RXD_AF                     GPIO_AF_8

#define GPIO_TXD_RCU                    RCU_GPIOC
#define GPIO_TXD_PORT                   GPIOC
#define GPIO_TXD_PIN                    GPIO_PIN_6
#define GPIO_TXD_AF                     GPIO_AF_8

#define UART_EN_RCU                     RCU_GPIOD
#define UART_EN_PORT                    GPIOD
#define UART_EN_PIN                     GPIO_PIN_7

#define UART_LED_RCU                    RCU_GPIOA
#define UART_LED_PORT                   GPIOA
#define UART_LED_PIN                    GPIO_PIN_11

void uart_init(void);
void uart_irq_register(void);

void uart0_send_data(uint8_t *data, int len);

extern INT32 UartGetc(VOID);
extern EVENT_CB_S g_shellInputEvent;

void uart_en_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */
