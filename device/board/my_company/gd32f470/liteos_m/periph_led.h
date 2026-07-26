#ifndef _PERIPH_LED_H_
#define _PERIPH_LED_H_

#include <stdio.h>
#include <stdbool.h>
#include "systick.h"
#include "los_task.h"
#include "gd32f4xx.h"
#include "uart_drv.h"
#include "shell_port.h"
#include "rtc_drv.h"
#include "wdg_drv.h"

// RS485_1_LED PA10
#define RS485_1_LED_INDEX           0U
#define RS485_1_LED_GPIO_RCU_CLOCK	RCU_GPIOA
#define RS485_1_LED_GPIO_PORT		GPIOA
#define RS485_1_LED_GPIO_PIN		GPIO_PIN_10
// RS485_2_LED PC8
#define RS485_2_LED_INDEX           1U
#define RS485_2_LED_GPIO_RCU_CLOCK	RCU_GPIOC
#define RS485_2_LED_GPIO_PORT		GPIOC
#define RS485_2_LED_GPIO_PIN		GPIO_PIN_8
// RS485_3_LED PA11
#define RS485_3_LED_INDEX           2U
#define RS485_3_LED_GPIO_RCU_CLOCK	RCU_GPIOA
#define RS485_3_LED_GPIO_PORT		GPIOA
#define RS485_3_LED_GPIO_PIN		GPIO_PIN_11
// RUNSTA_LED PA9
#define RUNSTA_LED_INDEX            3U
#define RUNSTA_LED_GPIO_RCU_CLOCK	RCU_GPIOA
#define RUNSTA_LED_GPIO_PORT		GPIOA
#define RUNSTA_LED_GPIO_PIN			GPIO_PIN_9


typedef struct {
    rcu_periph_enum periph;
    uint32_t gpio;
    uint32_t pin;
} gpio_t;

void init_led(uint8_t index);

void led_on(uint8_t index, bool on);

void init_periph_led();


#endif
