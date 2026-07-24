#ifndef _GPIO_H_
#define _GPIO_H_

#include <stdio.h>
#include "systick.h"
#include "los_task.h"
#include "gd32f4xx.h"
#include "uart_drv.h"
#include "shell_port.h"
#include "rtc_drv.h"
#include "wdg_drv.h"

// RS485 LED GPIO
#define RS485_1_LED_GPIO_RCU_CLOCK	RCU_GPIOA
#define RS485_1_LED_GPIO_PORT		GPIOA
#define RS485_1_LED_GPIO_PIN		GPIO_PIN_10

// RS485 LED GPIO
#define RS485_2_LED_GPIO_RCU_CLOCK	RCU_GPIOC
#define RS485_2_LED_GPIO_PORT		GPIOC
#define RS485_2_LED_GPIO_PIN		GPIO_PIN_8

// RS485 LED GPIO
#define RS485_3_LED_GPIO_RCU_CLOCK	RCU_GPIOA
#define RS485_3_LED_GPIO_PORT		GPIOA
#define RS485_3_LED_GPIO_PIN		GPIO_PIN_11

// RUNSTA LED GPIO
#define RUNSTA_LED_GPIO_RCU_CLOCK	RCU_GPIOA
#define RUNSTA_LED_GPIO_PORT		GPIOA
#define RUNSTA_LED_GPIO_PIN		GPIO_PIN_9

typedef struct {
    rcu_periph_enum periph;
    uint32_t gpio;
    uint32_t pin;
} led_t;

void init_led(uint8_t led);

void led_on(uint8_t led);

void led_off(uint8_t led);


#endif
