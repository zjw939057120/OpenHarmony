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
#include "modbus_rtu_task.h"
#include "message_queue.h"


// rs485_1 receive buffer
UINT8 rs485_1_recv_buf[QUEUE_MAX_NODE_SIZE] = {0};
UINT8 rs485_1_recv_buf_index = 0;
// rs485_2 receive buffer
UINT8 rs485_2_recv_buf[QUEUE_MAX_NODE_SIZE] = {0};
UINT8 rs485_2_recv_buf_index = 0;

void init_periph_uart(){
	rs485_1_init();
	rs485_2_init();
}

void rs485_1_init()
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

	rs485_1_RxIrqRegister();
}

void rs485_2_init()
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

	rs485_2_RxIrqRegister();
}

void rs485_1_ReceiveHandler(void)
{
	if ((RESET != usart_interrupt_flag_get(RS485_1_COM_NR, USART_INT_FLAG_RBNE)) &&
		(RESET != usart_flag_get(RS485_1_COM_NR, USART_FLAG_RBNE))) {
		/* receive data */
		rs485_1_recv_buf[rs485_1_recv_buf_index] = (char)usart_data_receive(RS485_1_COM_NR);
		// check index overflow
		if(rs485_1_recv_buf_index >= QUEUE_MAX_NODE_SIZE) {
			rs485_1_recv_buf_index = 0;
		}else {
			rs485_1_recv_buf_index++;	
		}

		/* clear interrupt flag and flag */
		usart_interrupt_flag_clear(RS485_1_COM_NR, USART_INT_FLAG_RBNE);
		usart_flag_clear(RS485_1_COM_NR, USART_FLAG_RBNE);
	} else if (RESET != usart_flag_get(RS485_1_COM_NR, USART_FLAG_IDLE)) {
		// 处理接收数据
		// 清除IDLE标志位
		usart_flag_get(RS485_1_COM_NR, USART_FLAG_IDLE);
		usart_data_receive(RS485_1_COM_NR);
		//截断字符串
		rs485_1_recv_buf[rs485_1_recv_buf_index] = '\0';

		// SEGGER_RTT_printf(0, "rs485_1_recv_buf = %s, queue_id = 0x%x\n", rs485_1_recv_buf,queueID_1);
		// put data to queue
		UINT32 ret = messageQueuePut(queueID_1, rs485_1_recv_buf, rs485_1_recv_buf_index);
		if(ret != LOS_OK) {
			printf("** messageQueuePut failed! queue_id = 0x%x, ret = 0x%x\n",queueID_1, ret);
		}
		// 清空接收缓冲区
		memset(rs485_1_recv_buf, 0, QUEUE_MAX_NODE_SIZE);
		rs485_1_recv_buf_index = 0;
	}

	if (RESET != usart_flag_get(RS485_1_COM_NR, USART_FLAG_ORERR)) {
		usart_flag_clear(RS485_1_COM_NR, USART_FLAG_ORERR);
	}
}

void rs485_1_RxIrqRegister(void)
{
	UINT32 ret = LOS_HwiCreate(RS485_1_COM_IRQ_NR, OS_HWI_PRIO_LOWEST, 0, (HWI_PROC_FUNC)rs485_1_ReceiveHandler, 0);
	if (ret != LOS_OK) {
        printf("%s failed! ret = 0x%x\r\n", __func__, ret);
        return;
    }

	nvic_irq_enable(RS485_1_COM_IRQ_NR, 0, 0);
	usart_interrupt_enable(RS485_1_COM_NR, USART_INT_RBNE);
	usart_interrupt_enable(RS485_1_COM_NR, USART_INT_IDLE);
}

void rs485_2_ReceiveHandler(void)
{
	if ((RESET != usart_interrupt_flag_get(RS485_2_COM_NR, USART_INT_FLAG_RBNE)) &&
		(RESET != usart_flag_get(RS485_2_COM_NR, USART_FLAG_RBNE))) {
		/* receive data */
		rs485_2_recv_buf[rs485_2_recv_buf_index] = (char)usart_data_receive(RS485_2_COM_NR);
		// check index overflow
		if(rs485_2_recv_buf_index >= QUEUE_MAX_NODE_SIZE) {
			rs485_2_recv_buf_index = 0;
		}else {
			rs485_2_recv_buf_index++;	
		}

		/* clear interrupt flag and flag */
		usart_interrupt_flag_clear(RS485_2_COM_NR, USART_INT_FLAG_RBNE);
		usart_flag_clear(RS485_2_COM_NR, USART_FLAG_RBNE);
	} else if (RESET != usart_flag_get(RS485_2_COM_NR, USART_FLAG_IDLE)) {
		// 处理接收数据
		// 清除IDLE标志位
		usart_flag_get(RS485_2_COM_NR, USART_FLAG_IDLE);
		usart_data_receive(RS485_2_COM_NR);
		//截断字符串
		rs485_2_recv_buf[rs485_2_recv_buf_index] = '\0';

		// SEGGER_RTT_printf(0, "rs485_2_recv_buf = %s, queue_id = 0x%x\n", rs485_2_recv_buf,queueID_2);
		// put data to queue
		UINT32 ret = messageQueuePut(queueID_2, rs485_2_recv_buf, rs485_2_recv_buf_index);
		if(ret != LOS_OK) {
			printf("** messageQueuePut failed! queue_id = 0x%x, ret = 0x%x\n",queueID_2, ret);
		}
		// 清空接收缓冲区
		memset(rs485_2_recv_buf, 0, QUEUE_MAX_NODE_SIZE);
		rs485_2_recv_buf_index = 0;
	}

	if (RESET != usart_flag_get(RS485_2_COM_NR, USART_FLAG_ORERR)) {
		usart_flag_clear(RS485_2_COM_NR, USART_FLAG_ORERR);
	}
}


void rs485_2_RxIrqRegister(void)
{
	UINT32 ret = LOS_HwiCreate(RS485_2_COM_IRQ_NR, OS_HWI_PRIO_LOWEST, 0, (HWI_PROC_FUNC)rs485_2_ReceiveHandler, 0);
	if (ret != LOS_OK) {
        printf("%s failed! ret = 0x%x\r\n", __func__, ret);
        return;
    }

	nvic_irq_enable(RS485_2_COM_IRQ_NR, 0, 0);
	usart_interrupt_enable(RS485_2_COM_NR, USART_INT_RBNE);
	usart_interrupt_enable(RS485_2_COM_NR, USART_INT_IDLE);
}

void rs485_1_send_bytes(uint8_t *bytes, uint32_t len)
{
	// 发送模式
	rs485_1_en(true);
    while(len--) {
		while (RESET == usart_flag_get(RS485_1_COM_NR, USART_FLAG_TBE)) ;
        usart_data_transmit(RS485_1_COM_NR, *bytes++);
    }
	// 接收模式
	rs485_1_en(false);
}

void rs485_2_send_bytes(uint8_t *bytes, uint32_t len)
{
	// 发送模式
	rs485_2_en(true);
    while(len--) {
		while (RESET == usart_flag_get(RS485_2_COM_NR, USART_FLAG_TBE)) ;
        usart_data_transmit(RS485_2_COM_NR, *bytes++);
    }
	// 接收模式
	rs485_2_en(false);
}
