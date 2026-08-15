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

#include "uart.h"

void uart0_send_byte(uint8_t ch)
{
	usart_data_transmit(UART_NUMBER, (uint8_t)ch);
    while(RESET == usart_flag_get(UART_NUMBER, USART_FLAG_TBE));
}

void uart0_send_data(uint8_t *data, int len)
{
	int i;
	for(i = 0; i < len; i++)
	{
		uart0_send_byte(data[i]);
	}
}

void uart0_send_string(uint8_t *str)
{
	uint8_t *p = str;
	while(*p)
	{
		uart0_send_byte(*p);
		p++;
	}
}

#if (LOSCFG_USE_SHELL == 1)
#define RX_BUF_SIZE                     128
static uint8_t rx_buf[RX_BUF_SIZE];
static uint16_t tx_index;
static uint16_t rx_index;

INT32 UartGetc(VOID)
{
    uint8_t c = 0;
    if (tx_index != rx_index) {
        c = rx_buf[tx_index++];
        tx_index %= RX_BUF_SIZE;     
    }
    return c;
}
#endif

void USART0_IRQHandler(void)
{
    if(usart_flag_get(UART_NUMBER, USART_FLAG_RBNE) != RESET)    //判断是否接收中断标志位置位
    {      
        usart_flag_clear(UART_NUMBER, USART_FLAG_RBNE);
        uint8_t c = usart_data_receive(UART_NUMBER);    //将接收到的数据存入buf    
#if (LOSCFG_USE_SHELL == 1)
        rx_buf[rx_index++] = c;
        rx_index %= RX_BUF_SIZE;
        if (rx_index == tx_index) {
            tx_index++;
            tx_index %= RX_BUF_SIZE;
        }
        (void)LOS_EventWrite(&g_shellInputEvent, 0x1);
#endif
    }
}

void uart_irq_register(void)
{
    /* USART 中断配置 */
    nvic_irq_enable(UART_IRQ, 1, 0);
    /* 使能串口接收中断 */
	usart_interrupt_enable(UART_NUMBER, USART_INT_RBNE);

#if (LOSCFG_USE_SHELL == 1)
    int ret = LOS_HwiCreate(UART_IRQ, 0, 0, (HWI_PROC_FUNC)USART0_IRQHandler, NULL);  /* 中断号 优先级 中断模式 处理函数 传入参数 */
    if (ret != LOS_OK) {
            printf("%s:%d Interrupt Create fail! ret=%x  USART0_IRQn=%d\n", __FILE__, __LINE__, ret, UART_IRQ);
    }
#endif
}
int __io_putchar(int ch)
{
	usart_data_transmit(UART_NUMBER, (uint8_t)ch);
    while(RESET == usart_flag_get(UART_NUMBER, USART_FLAG_TBE));

    return ch;
}
void uart_init(void)
{
	//使能 GPIO 时钟源
    rcu_periph_clock_enable(GPIO_RXD_RCU);
    rcu_periph_clock_enable(GPIO_TXD_RCU);

    //使能 USART0 时钟源
    rcu_periph_clock_enable(UART_RCU);

    /* 复用引脚为 USARTx_Tx */
    gpio_af_set(GPIO_RXD_PORT, GPIO_RXD_AF, GPIO_TXD_PIN);

    /* 复用引脚为 USARTx_Rx */
    gpio_af_set(GPIO_TXD_PORT, GPIO_TXD_AF, GPIO_RXD_PIN);

    /* 设置引脚为上拉复用 */
    gpio_mode_set(GPIO_TXD_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_TXD_PIN);
    gpio_output_options_set(GPIO_TXD_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_TXD_PIN);

    /* 设置引脚为上拉复用 */
    gpio_mode_set(GPIO_RXD_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_RXD_PIN);
    gpio_output_options_set(GPIO_RXD_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_RXD_PIN);

    /* USART 配置 */
    usart_deinit(UART_NUMBER);
    usart_baudrate_set(UART_NUMBER, UART_BAUD);
    usart_receive_config(UART_NUMBER, USART_RECEIVE_ENABLE);
    usart_transmit_config(UART_NUMBER, USART_TRANSMIT_ENABLE);
    usart_enable(UART_NUMBER);

    uart_en_init();
}

void uart_en_init(void)
{
    //使能 UART_EN
    rcu_periph_clock_enable(UART_EN_RCU);
    gpio_mode_set(UART_EN_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, UART_EN_PIN);
    gpio_output_options_set(UART_EN_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, UART_EN_PIN);
    //接收数据时，使能 UART_EN 输出
    gpio_bit_set(UART_EN_PORT, UART_EN_PIN);

    //使能 UART_LED
    rcu_periph_clock_enable(UART_LED_RCU);
    gpio_mode_set(UART_LED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, UART_LED_PIN);
    gpio_output_options_set(UART_LED_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, UART_LED_PIN);
    //默认关闭 LED
    gpio_bit_set(UART_LED_PORT, UART_LED_PIN);
}


