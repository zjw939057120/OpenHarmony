#include "periph_led.h"
#include "periph_uart.h"

gpio_t led_list[4] = {
	{RS485_1_LED_GPIO_RCU_CLOCK, RS485_1_LED_GPIO_PORT, RS485_1_LED_GPIO_PIN},
	{RS485_2_LED_GPIO_RCU_CLOCK, RS485_2_LED_GPIO_PORT, RS485_2_LED_GPIO_PIN},
	{RS485_3_LED_GPIO_RCU_CLOCK, RS485_3_LED_GPIO_PORT, RS485_3_LED_GPIO_PIN},
	{RUNSTA_LED_GPIO_RCU_CLOCK, RUNSTA_LED_GPIO_PORT, RUNSTA_LED_GPIO_PIN},
};

void init_led(uint8_t index)
{
	rcu_periph_clock_enable(led_list[index].periph);
	gpio_mode_set(led_list[index].gpio, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, led_list[index].pin);
	gpio_output_options_set(led_list[index].gpio, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, led_list[index].pin);
	//熄灭LED
	gpio_bit_set(led_list[index].gpio, led_list[index].pin);
}

void led_on(uint8_t index, bool on)
{
	on == true ? gpio_bit_reset(led_list[index].gpio, led_list[index].pin) : gpio_bit_set(led_list[index].gpio, led_list[index].pin);
}

void init_periph_led(){
	init_led(RS485_1_LED_INDEX);
	init_led(RS485_2_LED_INDEX);
	init_led(RS485_3_LED_INDEX);
	init_led(RUNSTA_LED_INDEX);
}
