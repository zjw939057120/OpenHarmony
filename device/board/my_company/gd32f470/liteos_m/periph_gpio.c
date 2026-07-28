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


const gpio_t gpio_list[] = {
	//LED灯
    [RS485_1_LED_INDEX] = {RS485_1_LED_GPIO_CLK, RS485_1_LED_GPIO_PORT, RS485_1_LED_PIN},
    [RS485_2_LED_INDEX] = {RS485_2_LED_GPIO_CLK, RS485_2_LED_GPIO_PORT, RS485_2_LED_PIN},
    [RS485_3_LED_INDEX] = {RS485_3_LED_GPIO_CLK, RS485_3_LED_GPIO_PORT, RS485_3_LED_PIN},
    [RUNSTA_LED_INDEX]  = {RUNSTA_LED_GPIO_CLK, RUNSTA_LED_GPIO_PORT, RUNSTA_LED_PIN},
	//RS485使能引脚
    [RS485_1_EN_INDEX]  = {RS485_1_EN_GPIO_CLK, RS485_1_EN_GPIO_PORT, RS485_1_EN_GPIO_PIN},
    [RS485_2_EN_INDEX]  = {RS485_2_EN_GPIO_CLK, RS485_2_EN_GPIO_PORT, RS485_2_EN_GPIO_PIN},
    [RS485_3_EN_INDEX]  = {RS485_3_EN_GPIO_CLK, RS485_3_EN_GPIO_PORT, RS485_3_EN_GPIO_PIN},
	//USER_KEY按键
    [USER_KEY_INDEX]    = {USER_KEY_GPIO_CLK, USER_KEY_GPIO_PORT, USER_KEY_PIN},
	//DO引脚
    [DO1_INDEX] = {DO1_GPIO_CLK, DO1_GPIO_PORT, DO1_PIN},
    [DO2_INDEX] = {DO2_GPIO_CLK, DO2_GPIO_PORT, DO2_PIN},
    [DO3_INDEX] = {DO3_GPIO_CLK, DO3_GPIO_PORT, DO3_PIN},
	//DI引脚
    [DI1_INDEX] = {DI1_GPIO_CLK, DI1_GPIO_PORT, DI1_PIN},
    [DI2_INDEX] = {DI2_GPIO_CLK, DI2_GPIO_PORT, DI2_PIN},
    [DI3_INDEX] = {DI3_GPIO_CLK, DI3_GPIO_PORT, DI3_PIN},
    [DI4_INDEX] = {DI4_GPIO_CLK, DI4_GPIO_PORT, DI4_PIN},
    [DI5_INDEX] = {DI5_GPIO_CLK, DI5_GPIO_PORT, DI5_PIN},
    [DI6_INDEX] = {DI6_GPIO_CLK, DI6_GPIO_PORT, DI6_PIN},
};

void init_periph_gpio()
{
	init_periph_led();
	init_periph_rs485_en();
	init_periph_do();
	init_periph_di();
}

void led_init(uint8_t index)
{
	rcu_periph_clock_enable(gpio_list[index].rcu);
	gpio_mode_set(gpio_list[index].gpio, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, gpio_list[index].pin);
	gpio_output_options_set(gpio_list[index].gpio, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, gpio_list[index].pin);
	//默认关闭
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

void gpio_toggle(uint8_t index)
{
	//切换GPIO引脚状态
	gpio_bit_toggle(gpio_list[index].gpio, gpio_list[index].pin);
}

void do_init(uint8_t index)
{
	rcu_periph_clock_enable(gpio_list[index].rcu);
	gpio_mode_set(gpio_list[index].gpio, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, gpio_list[index].pin);
	gpio_output_options_set(gpio_list[index].gpio, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, gpio_list[index].pin);
	//默认关闭
	gpio_bit_set(gpio_list[index].gpio, gpio_list[index].pin);
}

void do_on(uint8_t index)
{
	//打开DO继电器
	gpio_bit_reset(gpio_list[index].gpio, gpio_list[index].pin);
}

void do_off(uint8_t index)
{
	//关闭DO继电器
	gpio_bit_set(gpio_list[index].gpio, gpio_list[index].pin);
}

void di_init(uint8_t index)
{
	rcu_periph_clock_enable(gpio_list[index].rcu);
	gpio_mode_set(gpio_list[index].gpio, GPIO_MODE_INPUT, GPIO_PUPD_NONE, gpio_list[index].pin);
}

uint8_t di_read(uint8_t index)
{
	//读取DI引脚状态
	return gpio_input_bit_get(gpio_list[index].gpio, gpio_list[index].pin);
}

void init_periph_led(){
	led_init(RS485_1_LED_INDEX);
	led_init(RS485_2_LED_INDEX);
	led_init(RS485_3_LED_INDEX);
	led_init(RUNSTA_LED_INDEX);
}

void init_periph_do(){
	do_init(DO1_INDEX);
	do_init(DO2_INDEX);
	do_init(DO3_INDEX);
}

void init_periph_di(){
	di_init(DI1_INDEX);
	di_init(DI2_INDEX);
	di_init(DI3_INDEX);
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
	//默认接收模式
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

//DO继电器切换
uint8_t do_toggle = 1;

void EXTI10_15_IRQHandler(void)
{
    if(RESET != exti_interrupt_flag_get(USER_KEY_EXTI_LINE)) {
	// printf("%s:%d\r\n" ,__func__ ,__LINE__);
	if (do_toggle == 1) {
		//切换DO1继电器
		gpio_toggle(DO1_INDEX);
	} else if (do_toggle == 2) {
		//切换DO2继电器
		gpio_toggle(DO2_INDEX);
	} else if (do_toggle == 3) {
		//切换DO3继电器
		gpio_toggle(DO3_INDEX);
	}
	do_toggle > 3 ? do_toggle = 1 : do_toggle++;
	}
    exti_interrupt_flag_clear(USER_KEY_EXTI_LINE);
}
