#ifndef _PERIPH_GPIO_H_
#define _PERIPH_GPIO_H_

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

// USER_KEY PA12
#define USER_KEY_INDEX                   4U
#define USER_KEY_PIN                     GPIO_PIN_12
#define USER_KEY_GPIO_PORT               GPIOA
#define USER_KEY_GPIO_CLK                RCU_GPIOA
#define USER_KEY_EXTI_LINE               EXTI_12
#define USER_KEY_EXTI_PORT_SOURCE        EXTI_SOURCE_GPIOA
#define USER_KEY_EXTI_PIN_SOURCE         EXTI_SOURCE_PIN12
#define USER_KEY_EXTI_IRQn               EXTI10_15_IRQn

// RS485_1_EN PB9
#define RS485_1_EN_INDEX            5U
#define RS485_1_EN_GPIO_RCU_CLOCK   RCU_GPIOB
#define RS485_1_EN_GPIO_PORT        GPIOB
#define RS485_1_EN_GPIO_PIN			GPIO_PIN_9
// RS485_2_EN PB8
#define RS485_2_EN_INDEX            6U
#define RS485_2_EN_GPIO_RCU_CLOCK   RCU_GPIOB
#define RS485_2_EN_GPIO_PORT        GPIOB
#define RS485_2_EN_GPIO_PIN			GPIO_PIN_8
// RS485_3_EN PD7
#define RS485_3_EN_INDEX            7U
#define RS485_3_EN_GPIO_RCU_CLOCK   RCU_GPIOD
#define RS485_3_EN_GPIO_PORT        GPIOD
#define RS485_3_EN_GPIO_PIN			GPIO_PIN_7

typedef struct {
    rcu_periph_enum rcu;
    uint32_t gpio;
    uint32_t pin;
} gpio_t;


void init_periph_gpio();

void led_init(uint8_t index);

void led_on(uint8_t index);

void led_off(uint8_t index);

void rs485_en_init(uint8_t index);

void rs485_1_en(bool high);

void rs485_2_en(bool high);

void rs485_3_en(bool high);

void init_periph_led();

void init_periph_rs485_en();

void init_periph_key();

void EXTI10_15_IRQHandler(void);
#endif
