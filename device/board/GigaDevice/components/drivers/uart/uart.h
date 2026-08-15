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

#define UART_NUMBER                     USART0
#define UART_RCU                        RCU_USART0
#define UART_IRQ                        USART0_IRQn
#define UART_BAUD                       (115200U)

#define GPIO_RXD_RCU                    RCU_GPIOA
#define GPIO_RXD_PORT                   GPIOA
#define GPIO_RXD_PIN                    GPIO_PIN_10
#define GPIO_RXD_AF                     GPIO_AF_7

#define GPIO_TXD_RCU                    RCU_GPIOA
#define GPIO_TXD_PORT                   GPIOA
#define GPIO_TXD_PIN                    GPIO_PIN_9
#define GPIO_TXD_AF                     GPIO_AF_7

void uart_init(void);
void uart_irq_register(void);

void uart0_send_data(uint8_t *data, int len);

extern INT32 UartGetc(VOID);
extern EVENT_CB_S g_shellInputEvent;

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */
