#ifndef _PERIPH_UART_H_
#define _PERIPH_UART_H_

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "gd32f4xx.h"

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

// 串口接收队列节点大小
#define QUEUE_MAX_NODE_SIZE 128
// 串口接收队列最大长度
#define QUEUE_MAX_LEN 10

void init_periph_uart();

void rs485_com_1_init();

void rs485_com_2_init();

void rs485_com_1_RxIrqRegister();

void rs485_com_2_RxIrqRegister();

#endif
