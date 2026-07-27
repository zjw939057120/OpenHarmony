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
#include <string.h>
#include "los_arch_interrupt.h"
#include "los_interrupt.h"
#include "los_sem.h"
#include "los_task.h"
#include "los_event.h"
#include "gd32f4xx.h"
#include "uart_drv.h"
#include "shell_port.h"
#include "periph_gpio.h"
#include "periph_uart.h"

#if (CONFIG_CONSOLE_UART == 0)
#define UART_NR 				USART0
/* uart clock */
#define UART_RCU_GPIO_CLOCK		RCU_GPIOA
#define UART_RCU_UART_CLOCK 	RCU_USART0
/* uart tx */
#define UART_TX_GPIO			GPIOA
#define UART_TX_GPIO_PIN 		GPIO_PIN_9
#define UART_TX_GPIO_AF 		GPIO_AF_7
/* uart rx */
#define UART_RX_GPIO			GPIOA
#define UART_RX_GPIO_PIN 		GPIO_PIN_10
#define UART_RX_GPIO_AF 		GPIO_AF_7
/* uart irq */
#define UART_IRQ_NR				USART0_IRQn

#elif (CONFIG_CONSOLE_UART == 1)
// RS485_1_COM USART0 PB6,PB7,AP7
#define UART_NR 				RS485_1_COM_NR
/* uart clock */
#define UART_RCU_GPIO_CLOCK		RS485_1_COM_RCU_GPIO_CLOCK
#define UART_RCU_UART_CLOCK 	RS485_1_COM_RCU_UART_CLOCK
/* uart tx */
#define UART_TX_GPIO			RS485_1_COM_TX_GPIO
#define UART_TX_GPIO_PIN 		RS485_1_COM_TX_GPIO_PIN
#define UART_TX_GPIO_AF 		GPIO_AF_7
/* uart rx */
#define UART_RX_GPIO			RS485_1_COM_RX_GPIO
#define UART_RX_GPIO_PIN 		RS485_1_COM_RX_GPIO_PIN
#define UART_RX_GPIO_AF 		RS485_1_COM_RX_GPIO_AF
/* uart irq */
#define UART_IRQ_NR				RS485_1_COM_IRQ_NR

#elif (CONFIG_CONSOLE_UART == 2)
// RS485_2_COM USART2 PB10,PB11,AP7
#define UART_NR 				RS485_2_COM_NR
/* uart clock */
#define UART_RCU_GPIO_CLOCK		RS485_2_COM_RCU_GPIO_CLOCK
#define UART_RCU_UART_CLOCK 	RS485_2_COM_RCU_UART_CLOCK
/* uart tx */
#define UART_TX_GPIO			RS485_2_COM_TX_GPIO
#define UART_TX_GPIO_PIN 		RS485_2_COM_TX_GPIO_PIN
#define UART_TX_GPIO_AF 		GPIO_AF_7
/* uart rx */
#define UART_RX_GPIO			RS485_2_COM_RX_GPIO
#define UART_RX_GPIO_PIN 		RS485_2_COM_RX_GPIO_PIN
#define UART_RX_GPIO_AF 		RS485_2_COM_RX_GPIO_AF
/* uart irq */
#define UART_IRQ_NR				RS485_2_COM_IRQ_NR

#elif (CONFIG_CONSOLE_UART == 3)
//RS485_3_COM PC6,PC7,AF8
#define UART_NR 				RS485_3_COM_NR
/* uart clock */
#define UART_RCU_GPIO_CLOCK		RS485_3_COM_RCU_GPIO_CLOCK
#define UART_RCU_UART_CLOCK 	RS485_3_COM_RCU_UART_CLOCK
/* uart tx */
#define UART_TX_GPIO			RS485_3_COM_TX_GPIO
#define UART_TX_GPIO_PIN 		RS485_3_COM_TX_GPIO_PIN
#define UART_TX_GPIO_AF 		RS485_3_COM_TX_GPIO_AF
/* uart rx */
#define UART_RX_GPIO			RS485_3_COM_RX_GPIO
#define UART_RX_GPIO_PIN 		RS485_3_COM_RX_GPIO_PIN
#define UART_RX_GPIO_AF 		RS485_3_COM_RX_GPIO_AF
/* uart irq */
#define UART_IRQ_NR				RS485_3_COM_IRQ_NR
#endif


/* ring buffer */
#define RING_BUFFER_SIZE			(1024*4) /* must be 2^n */
#define RING_POS_NEXT(x)			(((x) + 1) & (RING_BUFFER_SIZE - 1))

typedef struct {
	const char *name;
	char buffer[RING_BUFFER_SIZE];
	int in;
	int out;

    unsigned int is_full: 1;
} ringbuffer_t;

static ringbuffer_t rx_ringbuffer = {.name = "rx"};
static ringbuffer_t tx_ringbuffer = {.name = "tx"};
static UINT32 g_tx_semId;
static int txTask_is_inited;

static int ringbuffer_write(ringbuffer_t *rb, const char c)
{
	if (rb->is_full) {
		return -1;
    }

	rb->buffer[rb->in] = c;
    rb->in = RING_POS_NEXT(rb->in);

	if (rb->in == rb->out)
        rb->is_full = 1;

	return 0;
}

static int ringbuffer_read(ringbuffer_t *rb, char *c)
{
	unsigned int intSave;

	intSave = LOS_IntLock();

	if (rb->in == rb->out && !rb->is_full) {
		LOS_IntRestore(intSave);
		return -1;
    }

	*c = rb->buffer[rb->out];
	rb->out = RING_POS_NEXT(rb->out);
	rb->is_full = 0;

	LOS_IntRestore(intSave);

	return 0;
}

static void _uartPutc(char c)
{
	while (RESET == usart_flag_get(UART_NR, USART_FLAG_TBE)) ;
	usart_data_transmit(UART_NR, (uint8_t)c);
}

static void uartPutc(char c)
{
	if (c == '\n')
		_uartPutc('\r');
	_uartPutc(c);
}

int uartPuts(const char *s)
{
	if (!txTask_is_inited) {
		unsigned int intSave;
		intSave = LOS_IntLock();
		while (*s) {
			uartPutc(*s);
			s++;
		}
		LOS_IntRestore(intSave);
	} else {
		unsigned int intSave;
		intSave = LOS_IntLock();
		while (*s) {
			if (ringbuffer_write(&tx_ringbuffer, *s) < 0) {
				const char *msg = "** ringbuffer_write: 'tx' buffer is full\n";
				const char *p = msg;
				while (*p) {
					uartPutc(*p);
					p++;
				}
				break;
			}
			s++;
		}
		LOS_IntRestore(intSave);
		LOS_SemPost(g_tx_semId);
	}

	return 0;
}

int uartGetc(char *c)
{
    if (ringbuffer_read(&rx_ringbuffer, c) == 0)
        return 0;
    return -1;
}

void uartInit(void)
{
	/* enable GPIO clock */
	rcu_periph_clock_enable(UART_RCU_GPIO_CLOCK);
	/* enable USART clock */
	rcu_periph_clock_enable(UART_RCU_UART_CLOCK);

	/* connect port to USARTx_Tx */
	gpio_af_set(UART_TX_GPIO, UART_TX_GPIO_AF, UART_TX_GPIO_PIN);
	/* connect port to USARTx_Rx */
	gpio_af_set(UART_RX_GPIO, UART_RX_GPIO_AF, UART_RX_GPIO_PIN);

	/* configure USART Tx as alternate function push-pull */
	gpio_mode_set(UART_TX_GPIO, GPIO_MODE_AF, GPIO_PUPD_PULLUP, UART_TX_GPIO_PIN);
	gpio_output_options_set(UART_TX_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, UART_TX_GPIO_PIN);
	/* configure USART Rx as alternate function push-pull */
	gpio_mode_set(UART_RX_GPIO, GPIO_MODE_AF, GPIO_PUPD_PULLUP, UART_RX_GPIO_PIN);
	gpio_output_options_set(UART_RX_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, UART_RX_GPIO_PIN);

	/* USART configure */
	usart_deinit(UART_NR);
	usart_baudrate_set(UART_NR, 9600U);
	usart_receive_config(UART_NR, USART_RECEIVE_ENABLE);
	usart_transmit_config(UART_NR, USART_TRANSMIT_ENABLE);
	usart_enable(UART_NR);
}

static void uartReceiveHandler(void)
{
	if ((RESET != usart_interrupt_flag_get(UART_NR, USART_INT_FLAG_RBNE)) &&
		(RESET != usart_flag_get(UART_NR, USART_FLAG_RBNE))) {
		/* receive data */
		char c = (char)usart_data_receive(UART_NR);
		if (ringbuffer_write(&rx_ringbuffer, c) < 0) 
			printf("** ringbuffer_write: 'rx' buffer is full\n");

#if IS_ENABLED(LOSCFG_SHELL)
#if IS_ENABLED(CONFIG_USE_LETTER_SHELL)
		wakeupShellRecvTask();
#else
		extern EVENT_CB_S g_shellInputEvent;
		LOS_EventWrite(&g_shellInputEvent, 0x1);
#endif
#endif

		/* clear interrupt flag and flag */
		usart_interrupt_flag_clear(UART_NR, USART_INT_FLAG_RBNE);
		usart_flag_clear(UART_NR, USART_FLAG_RBNE);
	}

	if (RESET != usart_flag_get(UART_NR, USART_FLAG_ORERR)) {
		printf("** uartReceiveHandler: overrun error\n");
		usart_flag_clear(UART_NR, USART_FLAG_ORERR);
	}
}

void uartRxIrqRegister(void)
{
	uint32_t ret = LOS_HwiCreate(UART_IRQ_NR, OS_HWI_PRIO_LOWEST, 0, (HWI_PROC_FUNC)uartReceiveHandler, 0);
	if (ret != LOS_OK) {
        printf("** uartRxIrqRegister failed, ret = 0x%x\n", ret);
        return;
    }

	nvic_irq_enable(UART_IRQ_NR, 0, 0);
	usart_interrupt_enable(UART_NR, USART_INT_RBNE);
}

static void *thread_uart_tx(unsigned int arg)
{
	txTask_is_inited = 1;
	while (1) {
		if (LOS_SemPend(g_tx_semId, LOS_WAIT_FOREVER) == LOS_OK) {
            char c;
			while (ringbuffer_read(&tx_ringbuffer, &c) == 0)
				uartPutc(c);
		}
	}
	return NULL;
}

void initUartTxTask(void)
{
	LOS_SemCreate(0, &g_tx_semId);

	UINT32 uartTxTaskID;
	TSK_INIT_PARAM_S stTask = {
		.pfnTaskEntry = thread_uart_tx,
		.uwStackSize = 0x1000,
		.pcName = "uartTxTask",
		.usTaskPrio = 6,
	};
	if (LOS_TaskCreate(&uartTxTaskID, &stTask) != LOS_OK) {
		printf("** LOS_TaskCreate uartTxTask failed!\n");
	}
}
