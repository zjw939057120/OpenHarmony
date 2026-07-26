#ifndef _PERIPH_UART_H_
#define _PERIPH_UART_H_

#include <stdio.h>
#include <stdbool.h>
#include "systick.h"
#include "los_task.h"
#include "gd32f4xx.h"
#include "uart_drv.h"
#include "shell_port.h"
#include "rtc_drv.h"
#include "wdg_drv.h"

// RS485_1_EN PB9
#define RS485_1_EN_INDEX                  0U
#define RS485_1_EN_GPIO_RCU_CLOCK         RCU_GPIOB
#define RS485_1_EN_GPIO_PORT              GPIOB
#define RS485_1_EN_GPIO_PIN				  GPIO_PIN_9
// RS485_2_EN PB8
#define RS485_2_EN_INDEX                  1U
#define RS485_2_EN_GPIO_RCU_CLOCK         RCU_GPIOB
#define RS485_2_EN_GPIO_PORT              GPIOB
#define RS485_2_EN_GPIO_PIN				  GPIO_PIN_8
// RS485_3_EN PD7
#define RS485_3_EN_INDEX                  2U
#define RS485_3_EN_GPIO_RCU_CLOCK         RCU_GPIOD
#define RS485_3_EN_GPIO_PORT              GPIOD
#define RS485_3_EN_GPIO_PIN				  GPIO_PIN_7


// RS485_1_COM USART0 PB6,PB7,AP7
#define RS485_1_COM_NR 				USART0
/* uart clock */
#define RS485_1_COM_RCU_GPIO_CLOCK		RCU_GPIOB
#define RS485_1_COM_RCU_UART_CLOCK 	RCU_USART0
/* uart tx */
#define RS485_1_COM_TX_GPIO			GPIOB
#define RS485_1_COM_TX_GPIO_PIN 		GPIO_PIN_6
#define RS485_1_COM_TX_GPIO_AF 		GPIO_AF_7
/* uart rx */
#define RS485_1_COM_RX_GPIO			GPIOB
#define RS485_1_COM_RX_GPIO_PIN 		GPIO_PIN_7
#define RS485_1_COM_RX_GPIO_AF 		GPIO_AF_7
/* uart irq */
#define RS485_1_COM_IRQ_NR				USART0_IRQn



// RS485_2_COM USART2 PB10,PB11,AP7
#define RS485_2_COM_NR 				USART2
/* uart clock */
#define RS485_2_COM_RCU_GPIO_CLOCK		RCU_GPIOB
#define RS485_2_COM_RCU_UART_CLOCK 	RCU_USART2
/* uart tx */
#define RS485_2_COM_TX_GPIO			GPIOB
#define RS485_2_COM_TX_GPIO_PIN 		GPIO_PIN_10
#define RS485_2_COM_TX_GPIO_AF 		GPIO_AF_7
/* uart rx */
#define RS485_2_COM_RX_GPIO			GPIOC
#define RS485_2_COM_RX_GPIO_PIN 		GPIO_PIN_11
#define RS485_2_COM_RX_GPIO_AF 		GPIO_AF_7
/* uart irq */
#define RS485_2_COM_IRQ_NR				USART2_IRQn



//RS485_3_COM PC6,PC7,AF8
#define RS485_3_COM_NR 				USART5
/* uart clock */
#define RS485_3_COM_RCU_GPIO_CLOCK		RCU_GPIOC
#define RS485_3_COM_RCU_UART_CLOCK 	RCU_USART5
/* uart tx */
#define RS485_3_COM_TX_GPIO			GPIOC
#define RS485_3_COM_TX_GPIO_PIN 		GPIO_PIN_6
#define RS485_3_COM_TX_GPIO_AF 		GPIO_AF_8
/* uart rx */
#define RS485_3_COM_RX_GPIO			GPIOC
#define RS485_3_COM_RX_GPIO_PIN 		GPIO_PIN_7
#define RS485_3_COM_RX_GPIO_AF 		GPIO_AF_8
/* uart irq */
#define RS485_3_COM_IRQ_NR				USART5_IRQn

void init_rs485_en(uint8_t index);

void rs485_en(uint8_t index, bool en);

void init_periph_uart();


void init_rs485_com_1();

void init_rs485_com_2();

void init_rs485_com_3();

void rs485_com_1_RxIrqRegister();

void rs485_com_2_RxIrqRegister();

void rs485_com_3_RxIrqRegister();

void rs485_com_1_ReceiveHandler(void);

void rs485_com_2_ReceiveHandler(void);

void rs485_com_3_ReceiveHandler(void);


#endif
