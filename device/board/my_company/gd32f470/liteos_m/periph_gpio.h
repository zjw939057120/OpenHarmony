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

// RS485_1_LED~RS485_3_LED LED灯定义
// RS485_1_LED PA10
#define RS485_1_LED_INDEX           0U
#define RS485_1_LED_GPIO_CLK        RCU_GPIOA
#define RS485_1_LED_GPIO_PORT		GPIOA
#define RS485_1_LED_PIN				GPIO_PIN_10
// RS485_2_LED PC8
#define RS485_2_LED_INDEX           1U
#define RS485_2_LED_GPIO_CLK        RCU_GPIOC
#define RS485_2_LED_GPIO_PORT		GPIOC
#define RS485_2_LED_PIN				GPIO_PIN_8
// RS485_3_LED PA11
#define RS485_3_LED_INDEX           2U
#define RS485_3_LED_GPIO_CLK        RCU_GPIOA
#define RS485_3_LED_GPIO_PORT		GPIOA
#define RS485_3_LED_PIN				GPIO_PIN_11

// RUNSTA_LED LED灯定义
// RUNSTA_LED PA9
#define RUNSTA_LED_INDEX            3U
#define RUNSTA_LED_GPIO_CLK         RCU_GPIOA
#define RUNSTA_LED_GPIO_PORT		GPIOA
#define RUNSTA_LED_PIN				GPIO_PIN_9

// RS485_1_EN~RS485_3_EN使能引脚定义
// RS485_1_EN PB9
#define RS485_1_EN_INDEX            4U
#define RS485_1_EN_GPIO_CLK         RCU_GPIOB
#define RS485_1_EN_GPIO_PORT        GPIOB
#define RS485_1_EN_GPIO_PIN			GPIO_PIN_9
// RS485_2_EN PB8
#define RS485_2_EN_INDEX            5U
#define RS485_2_EN_GPIO_CLK         RCU_GPIOB
#define RS485_2_EN_GPIO_PORT        GPIOB
#define RS485_2_EN_GPIO_PIN			GPIO_PIN_8
// RS485_3_EN PD7
#define RS485_3_EN_INDEX            6U
#define RS485_3_EN_GPIO_CLK         RCU_GPIOD
#define RS485_3_EN_GPIO_PORT        GPIOD
#define RS485_3_EN_GPIO_PIN			GPIO_PIN_7

// USER_KEY按键定义
// USER_KEY PA12
#define USER_KEY_INDEX              7U
#define USER_KEY_PIN                GPIO_PIN_12
#define USER_KEY_GPIO_PORT          GPIOA
#define USER_KEY_GPIO_CLK           RCU_GPIOA
#define USER_KEY_EXTI_LINE          EXTI_12
#define USER_KEY_EXTI_PORT_SOURCE   EXTI_SOURCE_GPIOA
#define USER_KEY_EXTI_PIN_SOURCE    EXTI_SOURCE_PIN12
#define USER_KEY_EXTI_IRQn          EXTI10_15_IRQn

//DO1~DO3引脚定义
//DO1 PB1
#define DO1_INDEX                   8U
#define DO1_PIN		                GPIO_PIN_1
#define DO1_GPIO_CLK         	    RCU_GPIOB
#define DO1_GPIO_PORT		        GPIOB
//DO2 PA6
#define DO2_INDEX                   9U
#define DO2_PIN		                GPIO_PIN_6
#define DO2_GPIO_CLK         	    RCU_GPIOA
#define DO2_GPIO_PORT		        GPIOA
//DO3 PA5
#define DO3_INDEX                   10U
#define DO3_PIN		                GPIO_PIN_5
#define DO3_GPIO_CLK         	    RCU_GPIOA
#define DO3_GPIO_PORT		        GPIOA

//DI1~DI6引脚定义
//DI1 PF7
#define DI1_INDEX                   11U
#define DI1_PIN		                GPIO_PIN_7
#define DI1_GPIO_CLK         	    RCU_GPIOF
#define DI1_GPIO_PORT		        GPIOF
//DI2 PE6
#define DI2_INDEX                   12U
#define DI2_PIN		                GPIO_PIN_6
#define DI2_GPIO_CLK         	    RCU_GPIOE
#define DI2_GPIO_PORT		        GPIOE
//DI3 PE5
#define DI3_INDEX                   13U
#define DI3_PIN		                GPIO_PIN_5
#define DI3_GPIO_CLK         	    RCU_GPIOE
#define DI3_GPIO_PORT		        GPIOE
//DI4 PE4
#define DI4_INDEX                   14U
#define DI4_PIN		                GPIO_PIN_4
#define DI4_GPIO_CLK         	    RCU_GPIOE
#define DI4_GPIO_PORT		        GPIOE
//DI5 PE3
#define DI5_INDEX                   15U
#define DI5_PIN		                GPIO_PIN_3
#define DI5_GPIO_CLK         	    RCU_GPIOE
#define DI5_GPIO_PORT		        GPIOE
//DI6 PE2
#define DI6_INDEX                   16U
#define DI6_PIN		                GPIO_PIN_2
#define DI6_GPIO_CLK         	    RCU_GPIOE
#define DI6_GPIO_PORT		        GPIOE



typedef struct {
    rcu_periph_enum rcu;
    uint32_t gpio;
    uint32_t pin;
} gpio_t;


void init_periph_gpio();

void led_init(uint8_t index);

void led_on(uint8_t index);

void led_off(uint8_t index);

void gpio_toggle(uint8_t index);

void do_init(uint8_t index);

void do_on(uint8_t index);

void do_off(uint8_t index);\

void di_init(uint8_t index);

uint8_t di_read(uint8_t index);

void rs485_en_init(uint8_t index);

void rs485_1_en(bool high);

void rs485_2_en(bool high);

void rs485_3_en(bool high);

void init_periph_led();

void init_periph_do();

void init_periph_di();

void init_periph_rs485_en();

void init_periph_key();

void EXTI10_15_IRQHandler(void);
#endif
