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


gpio_t gpio_list[] = {
	//LED
	{RS485_1_LED_GPIO_RCU_CLOCK, RS485_1_LED_GPIO_PORT, RS485_1_LED_GPIO_PIN},
	{RS485_2_LED_GPIO_RCU_CLOCK, RS485_2_LED_GPIO_PORT, RS485_2_LED_GPIO_PIN},
	{RS485_3_LED_GPIO_RCU_CLOCK, RS485_3_LED_GPIO_PORT, RS485_3_LED_GPIO_PIN},
	{RUNSTA_LED_GPIO_RCU_CLOCK, RUNSTA_LED_GPIO_PORT, RUNSTA_LED_GPIO_PIN},
	//USER_KEY
	{USER_KEY_GPIO_CLK, USER_KEY_GPIO_PORT, USER_KEY_PIN},
	//RS485_EN
	{RS485_1_EN_GPIO_RCU_CLOCK, RS485_1_EN_GPIO_PORT, RS485_1_EN_GPIO_PIN},
	{RS485_2_EN_GPIO_RCU_CLOCK, RS485_2_EN_GPIO_PORT, RS485_2_EN_GPIO_PIN},
	{RS485_3_EN_GPIO_RCU_CLOCK, RS485_3_EN_GPIO_PORT, RS485_3_EN_GPIO_PIN},
};

void init_periph_gpio()
{
	init_periph_led();
	init_periph_rs485_en();
}

void led_init(uint8_t index)
{
	rcu_periph_clock_enable(gpio_list[index].rcu);
	gpio_mode_set(gpio_list[index].gpio, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, gpio_list[index].pin);
	gpio_output_options_set(gpio_list[index].gpio, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, gpio_list[index].pin);
	//熄灭LED
	gpio_bit_set(gpio_list[index].gpio, gpio_list[index].pin);
}

void led_on(uint8_t index)
{
	//点亮LED
	gpio_bit_reset(gpio_list[index].gpio, gpio_list[index].pin);
}

void led_off(uint8_t index)
{
	//熄灭LED
	gpio_bit_set(gpio_list[index].gpio, gpio_list[index].pin);
}

void init_periph_led(){
	led_init(RS485_1_LED_INDEX);
	led_init(RS485_2_LED_INDEX);
	led_init(RS485_3_LED_INDEX);
	led_init(RUNSTA_LED_INDEX);
}

void init_periph_rs485_en(){
	rs485_en_init(RS485_1_EN_INDEX);
	rs485_en_init(RS485_2_EN_INDEX);
	rs485_en_init(RS485_3_EN_INDEX);
}

void rs485_en_init(uint8_t index)
{
	rcu_periph_clock_enable(gpio_list[index].rcu);
	gpio_mode_set(gpio_list[index].gpio, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, gpio_list[index].pin);
	gpio_output_options_set(gpio_list[index].gpio, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, gpio_list[index].pin);
	//接收模式
	gpio_bit_reset(gpio_list[index].gpio, gpio_list[index].pin);
}

void rs485_1_en(bool high)
{
	if (high == true) {
		//发送模式
		gpio_bit_set(gpio_list[RS485_1_EN_INDEX].gpio, gpio_list[RS485_1_EN_INDEX].pin);
		led_on(RS485_1_LED_INDEX);
	} else {
		//接收模式
		led_off(RS485_1_LED_INDEX);
		gpio_bit_reset(gpio_list[RS485_1_EN_INDEX].gpio, gpio_list[RS485_1_EN_INDEX].pin);
	}
}

void rs485_2_en(bool high)
{
	if (high == true) {
		//发送模式
		gpio_bit_set(gpio_list[RS485_2_EN_INDEX].gpio, gpio_list[RS485_2_EN_INDEX].pin);
		led_on(RS485_2_LED_INDEX);
	} else {
		//接收模式
		led_off(RS485_2_LED_INDEX);
		gpio_bit_reset(gpio_list[RS485_2_EN_INDEX].gpio, gpio_list[RS485_2_EN_INDEX].pin);
	}
}

void rs485_3_en(bool high)
{
	if (high == true) {
		//发送模式
		gpio_bit_set(gpio_list[RS485_3_EN_INDEX].gpio, gpio_list[RS485_3_EN_INDEX].pin);
		led_on(RS485_3_LED_INDEX);
	} else {
		//接收模式
		led_off(RS485_3_LED_INDEX);
		gpio_bit_reset(gpio_list[RS485_3_EN_INDEX].gpio, gpio_list[RS485_3_EN_INDEX].pin);
	}
}

void init_periph_key()
{
    /* enable the Tamper key GPIO clock */
    rcu_periph_clock_enable(USER_KEY_GPIO_CLK);
    rcu_periph_clock_enable(RCU_SYSCFG);

    gpio_mode_set(USER_KEY_GPIO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, USER_KEY_PIN);
    /* enable and set key EXTI interrupt priority */
    uint32_t ret = LOS_HwiCreate(USER_KEY_EXTI_IRQn, 2U, 0, EXTI10_15_IRQHandler, NULL);
    if (ret != LOS_OK) {
        printf("%s failed! ret = 0x%x\r\n", __func__, ret);
    }
    nvic_irq_enable(USER_KEY_EXTI_IRQn, 2U, 0U);
    /* connect key EXTI line to key GPIO pin */
    syscfg_exti_line_config(USER_KEY_EXTI_PORT_SOURCE, USER_KEY_EXTI_PIN_SOURCE);
    /* configure key EXTI line */
    exti_init(USER_KEY_EXTI_LINE, EXTI_INTERRUPT, EXTI_TRIG_RISING);
    exti_interrupt_flag_clear(USER_KEY_EXTI_LINE);
}

void EXTI10_15_IRQHandler(void)
{
    if(RESET != exti_interrupt_flag_get(USER_KEY_EXTI_LINE)) {
	printf("%s:%d\r\n" ,__func__ ,__LINE__);
    }
    exti_interrupt_flag_clear(USER_KEY_EXTI_LINE);
}
