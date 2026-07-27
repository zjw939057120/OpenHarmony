#include "periph_gpio.h"
#include "periph_uart.h"

gpio_t gpio_list[] = {
	//LED
	{RS485_1_LED_GPIO_RCU_CLOCK, RS485_1_LED_GPIO_PORT, RS485_1_LED_GPIO_PIN},
	{RS485_2_LED_GPIO_RCU_CLOCK, RS485_2_LED_GPIO_PORT, RS485_2_LED_GPIO_PIN},
	{RS485_3_LED_GPIO_RCU_CLOCK, RS485_3_LED_GPIO_PORT, RS485_3_LED_GPIO_PIN},
	{RUNSTA_LED_GPIO_RCU_CLOCK, RUNSTA_LED_GPIO_PORT, RUNSTA_LED_GPIO_PIN},
	//RS485_EN
	{RS485_1_EN_GPIO_RCU_CLOCK, RS485_1_EN_GPIO_PORT, RS485_1_EN_GPIO_PIN},
	{RS485_2_EN_GPIO_RCU_CLOCK, RS485_2_EN_GPIO_PORT, RS485_2_EN_GPIO_PIN},
	{RS485_3_EN_GPIO_RCU_CLOCK, RS485_3_EN_GPIO_PORT, RS485_3_EN_GPIO_PIN},
};

void led_init(uint8_t index)
{
	rcu_periph_clock_enable(gpio_list[index].periph);
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
	rcu_periph_clock_enable(gpio_list[index].periph);
	gpio_mode_set(gpio_list[index].gpio, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, gpio_list[index].pin);
	gpio_output_options_set(gpio_list[index].gpio, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, gpio_list[index].pin);
	//接收模式
	gpio_bit_reset(gpio_list[index].gpio, gpio_list[index].pin);
}

void rs485_en(uint8_t index, bool en)
{
	en == true ? gpio_bit_set(gpio_list[index].gpio, gpio_list[index].pin) : gpio_bit_reset(gpio_list[index].gpio, gpio_list[index].pin);
}