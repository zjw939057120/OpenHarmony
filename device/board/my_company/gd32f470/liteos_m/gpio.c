#include "gpio.h"

led_t led_list[4] = {
	{RS485_1_LED_GPIO_RCU_CLOCK, RS485_1_LED_GPIO_PORT, RS485_1_LED_GPIO_PIN},
	{RS485_2_LED_GPIO_RCU_CLOCK, RS485_2_LED_GPIO_PORT, RS485_2_LED_GPIO_PIN},
	{RS485_3_LED_GPIO_RCU_CLOCK, RS485_3_LED_GPIO_PORT, RS485_3_LED_GPIO_PIN},
	{RUNSTA_LED_GPIO_RCU_CLOCK, RUNSTA_LED_GPIO_PORT, RUNSTA_LED_GPIO_PIN},
};

void init_led(uint8_t led)
{
	rcu_periph_clock_enable(led_list[led].periph);
	gpio_mode_set(led_list[led].gpio, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, led_list[led].pin);
	gpio_output_options_set(led_list[led].gpio, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, led_list[led].pin);
	gpio_bit_set(led_list[led].gpio, led_list[led].pin);
}

void led_on(uint8_t led)
{
	gpio_bit_reset(led_list[led].gpio, led_list[led].pin);
}

void led_off(uint8_t led)
{
	gpio_bit_set(led_list[led].gpio, led_list[led].pin);
}
