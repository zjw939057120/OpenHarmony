#include <stdio.h>
#include <string.h>
#include "los_arch_interrupt.h"
#include "los_interrupt.h"
#include "los_sem.h"
#include "los_task.h"
#include "los_event.h"
#include "gd32f4xx.h"
#include "periph_gpio.h"
#include "periph_uart.h"


void init_periph_uart(){
	// rs485_com_1_init();
	// rs485_com_2_init();
}

void rs485_com_1_init()
{
	/* enable GPIO clock */
	rcu_periph_clock_enable(RS485_1_COM_RCU_GPIO_CLOCK);
	/* enable USART clock */
	rcu_periph_clock_enable(RS485_1_COM_RCU_UART_CLOCK);

	/* connect port to USARTx_Tx */
	gpio_af_set(RS485_1_COM_TX_GPIO, RS485_1_COM_TX_GPIO_AF, RS485_1_COM_TX_GPIO_PIN);
	/* connect port to USARTx_Rx */
	gpio_af_set(RS485_1_COM_RX_GPIO, RS485_1_COM_RX_GPIO_AF, RS485_1_COM_RX_GPIO_PIN);

	/* configure USART Tx as alternate function push-pull */
	gpio_mode_set(RS485_1_COM_TX_GPIO, GPIO_MODE_AF, GPIO_PUPD_PULLUP, RS485_1_COM_TX_GPIO_PIN);
	gpio_output_options_set(RS485_1_COM_TX_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, RS485_1_COM_TX_GPIO_PIN);
	/* configure USART Rx as alternate function push-pull */
	gpio_mode_set(RS485_1_COM_RX_GPIO, GPIO_MODE_AF, GPIO_PUPD_PULLUP, RS485_1_COM_RX_GPIO_PIN);
	gpio_output_options_set(RS485_1_COM_RX_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, RS485_1_COM_RX_GPIO_PIN);

	/* USART configure */
	usart_deinit(RS485_1_COM_NR);
	usart_baudrate_set( RS485_1_COM_NR, 9600U);
	usart_receive_config( RS485_1_COM_NR, USART_RECEIVE_ENABLE);
	usart_transmit_config( RS485_1_COM_NR, USART_TRANSMIT_ENABLE);
	usart_enable(RS485_1_COM_NR);

	rs485_com_1_RxIrqRegister();
}

void rs485_com_2_init()
{
	/* enable GPIO clock */
	rcu_periph_clock_enable(RS485_2_COM_RCU_GPIO_CLOCK);
	/* enable USART clock */
	rcu_periph_clock_enable(RS485_2_COM_RCU_UART_CLOCK);

	/* connect port to USARTx_Tx */
	gpio_af_set(RS485_2_COM_TX_GPIO, RS485_2_COM_TX_GPIO_AF, RS485_2_COM_TX_GPIO_PIN);
	/* connect port to USARTx_Rx */
	gpio_af_set(RS485_2_COM_RX_GPIO, RS485_2_COM_RX_GPIO_AF, RS485_2_COM_RX_GPIO_PIN);

	/* configure USART Tx as alternate function push-pull */
	gpio_mode_set(RS485_2_COM_TX_GPIO, GPIO_MODE_AF, GPIO_PUPD_PULLUP, RS485_2_COM_TX_GPIO_PIN);
	gpio_output_options_set(RS485_2_COM_TX_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, RS485_2_COM_TX_GPIO_PIN);
	/* configure USART Rx as alternate function push-pull */
	gpio_mode_set(RS485_2_COM_RX_GPIO, GPIO_MODE_AF, GPIO_PUPD_PULLUP, RS485_2_COM_RX_GPIO_PIN);
	gpio_output_options_set(RS485_2_COM_RX_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, RS485_2_COM_RX_GPIO_PIN);

	/* USART configure */
	usart_deinit(RS485_2_COM_NR);
	usart_baudrate_set( RS485_2_COM_NR, 9600U);
	usart_receive_config( RS485_2_COM_NR, USART_RECEIVE_ENABLE);
	usart_transmit_config( RS485_2_COM_NR, USART_TRANSMIT_ENABLE);
	usart_enable(RS485_2_COM_NR);

	rs485_com_2_RxIrqRegister();
}
void rs485_com_3_init()
{
	/* enable GPIO clock */
	rcu_periph_clock_enable(RS485_3_COM_RCU_GPIO_CLOCK);
	/* enable USART clock */
	rcu_periph_clock_enable(RS485_3_COM_RCU_UART_CLOCK);

	/* connect port to USARTx_Tx */
	gpio_af_set(RS485_3_COM_TX_GPIO, RS485_3_COM_TX_GPIO_AF, RS485_3_COM_TX_GPIO_PIN);
	/* connect port to USARTx_Rx */
	gpio_af_set(RS485_3_COM_RX_GPIO, RS485_3_COM_RX_GPIO_AF, RS485_3_COM_RX_GPIO_PIN);

	/* configure USART Tx as alternate function push-pull */
	gpio_mode_set(RS485_3_COM_TX_GPIO, GPIO_MODE_AF, GPIO_PUPD_PULLUP, RS485_3_COM_TX_GPIO_PIN);
	gpio_output_options_set(RS485_3_COM_TX_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, RS485_3_COM_TX_GPIO_PIN);
	/* configure USART Rx as alternate function push-pull */
	gpio_mode_set(RS485_3_COM_RX_GPIO, GPIO_MODE_AF, GPIO_PUPD_PULLUP, RS485_3_COM_RX_GPIO_PIN);
	gpio_output_options_set(RS485_1_COM_RX_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, RS485_1_COM_RX_GPIO_PIN);

	/* USART configure */
	usart_deinit(RS485_3_COM_NR);
	usart_baudrate_set( RS485_3_COM_NR, 9600U);
	usart_receive_config( RS485_3_COM_NR, USART_RECEIVE_ENABLE);
	usart_transmit_config( RS485_3_COM_NR, USART_TRANSMIT_ENABLE);
	usart_enable(RS485_3_COM_NR);

	rs485_com_3_RxIrqRegister();
}

void rs485_com_1_RxIrqRegister(void)
{
	uint32_t ret = LOS_HwiCreate(RS485_1_COM_IRQ_NR, OS_HWI_PRIO_LOWEST, 0, (HWI_PROC_FUNC)rs485_com_1_ReceiveHandler, 0);
	if (ret != LOS_OK) {
        // printf("** uartRxIrqRegister failed, ret = 0x\n", ret);
        return;
    }

	nvic_irq_enable(RS485_1_COM_IRQ_NR, 0, 0);
	usart_interrupt_enable(RS485_1_COM_NR, USART_INT_RBNE);
}

void rs485_com_2_RxIrqRegister(void)
{
	uint32_t ret = LOS_HwiCreate(RS485_2_COM_IRQ_NR, OS_HWI_PRIO_LOWEST, 0, (HWI_PROC_FUNC)rs485_com_2_ReceiveHandler, 0);
	if (ret != LOS_OK) {
        // printf("** uartRxIrqRegister failed, ret = 0x%x\n", ret);
        return;
    }

	nvic_irq_enable(RS485_2_COM_IRQ_NR, 0, 0);
	usart_interrupt_enable(RS485_2_COM_NR, USART_INT_RBNE);
}

void rs485_com_3_RxIrqRegister(void)
{
	uint32_t ret = LOS_HwiCreate(RS485_3_COM_IRQ_NR, OS_HWI_PRIO_LOWEST, 0, (HWI_PROC_FUNC)rs485_com_3_ReceiveHandler, 0);
	if (ret != LOS_OK) {
        // printf("** uartRxIrqRegister failed, ret = 0x%x\n", ret);
        return;
    }

	nvic_irq_enable(RS485_3_COM_IRQ_NR, 0, 0);
	usart_interrupt_enable(RS485_3_COM_NR, USART_INT_RBNE);
}

void rs485_com_1_ReceiveHandler(void)
{
	if ((RESET != usart_interrupt_flag_get(RS485_1_COM_NR, USART_INT_FLAG_RBNE)) &&
		(RESET != usart_flag_get(RS485_1_COM_NR, USART_FLAG_RBNE))) {
		/* receive data */
		char c = (char)usart_data_receive(RS485_1_COM_NR);

		/* clear interrupt flag and flag */
		usart_interrupt_flag_clear(RS485_1_COM_NR, USART_INT_FLAG_RBNE);
		usart_flag_clear(RS485_1_COM_NR, USART_FLAG_RBNE);
	}

	if (RESET != usart_flag_get(RS485_1_COM_NR, USART_FLAG_ORERR)) {
		usart_flag_clear(RS485_1_COM_NR, USART_FLAG_ORERR);
	}
}

void rs485_com_2_ReceiveHandler(void)
{
	if ((RESET != usart_interrupt_flag_get(RS485_2_COM_NR, USART_INT_FLAG_RBNE)) &&
		(RESET != usart_flag_get(RS485_2_COM_NR, USART_FLAG_RBNE))) {
		/* receive data */
		char c = (char)usart_data_receive(RS485_2_COM_NR);

		/* clear interrupt flag and flag */
		usart_interrupt_flag_clear(RS485_2_COM_NR, USART_INT_FLAG_RBNE);
		usart_flag_clear(RS485_2_COM_NR, USART_FLAG_RBNE);
	}

	if (RESET != usart_flag_get(RS485_2_COM_NR, USART_FLAG_ORERR)) {
		usart_flag_clear(RS485_2_COM_NR, USART_FLAG_ORERR);
	}
}

void rs485_com_3_ReceiveHandler(void)
{
	if ((RESET != usart_interrupt_flag_get(RS485_3_COM_NR, USART_INT_FLAG_RBNE)) &&
		(RESET != usart_flag_get(RS485_3_COM_NR, USART_FLAG_RBNE))) {
		/* receive data */
		char c = (char)usart_data_receive(RS485_3_COM_NR);

		/* clear interrupt flag and flag */
		usart_interrupt_flag_clear(RS485_3_COM_NR, USART_INT_FLAG_RBNE);
		usart_flag_clear(RS485_3_COM_NR, USART_FLAG_RBNE);
	}

	if (RESET != usart_flag_get(RS485_3_COM_NR, USART_FLAG_ORERR)) {
		usart_flag_clear(RS485_3_COM_NR, USART_FLAG_ORERR);
	}
}