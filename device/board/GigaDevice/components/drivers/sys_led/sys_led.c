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

#include "sys_led.h"

#define DUTY_COUNT      99999

/* 开灯 */
void system_led_on(void)
{
    gpio_bit_reset(LED_PORT, LED_PIN);
}

/* 关灯 */
void system_led_off(void)
{
    gpio_bit_set(LED_PORT, LED_PIN);
}

/* 初始化gpio */
void system_led_init(void)
{	
    rcu_periph_clock_enable(LED_CLK);
    
    gpio_mode_set(LED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_PIN);
    gpio_output_options_set(LED_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LED_PIN);

    gpio_bit_set(LED_PORT, LED_PIN);
}

/* led pwm 初始化 */
void system_led_pwm_init(void)
{
    rcu_periph_clock_enable(LED_CLK);
    rcu_periph_clock_enable(RCU_TIMER1);   //使能 TIMER1 时钟

    gpio_mode_set(LED_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LED_PIN);
    gpio_output_options_set(LED_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,LED_PIN);
    gpio_af_set(LED_PORT, GPIO_AF_1, LED_PIN);


    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;
 
    rcu_periph_clock_enable(RCU_TIMER0);
 
    timer_deinit(TIMER0);
 
    /* 定时器配置 */
    timer_initpara.prescaler         = 119;                     /* 对时钟进行预分频，240Mhz, 分下来是2Mhz */
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;      /* 计数的对齐方式，通常有上升沿或下降沿对齐 */
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;        /* 向上计数还是向下计数 */
    timer_initpara.period            = DUTY_COUNT;              /* 这个参数设置定时器的周期值，通常是计数器达到该值后会产生一个中断或重置 */
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;        /* 参数用于设置定时器的时钟分频 */
    timer_initpara.repetitioncounter = 0;                       /* 设置定时器的重复计数，提前装入的值，为了特定频率 */
    timer_init(TIMER0, &timer_initpara);
 
    /* PWM模式下配置 CH0、CH1 和 CH2 */
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;            /* 输出状态 */
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;          /* 反向输出状态 */
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;      /* 输出极性 */
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;     /* 反向输出极性 */
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;     /* 输出空闲状态 */
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;    /* 反向输出空闲状态 */
 
    // 配置 TIMER0 通道输出功能
    timer_channel_output_config(TIMER0, TIMER_CH_0, &timer_ocintpara);
 
    /* CH0 以 PWM 模式工作，占空比为 0% */
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, 0);
    timer_channel_output_mode_config(TIMER0, TIMER_CH_0, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);
 
	/* 配置 TIMER 主输出功能 */
	timer_primary_output_config(TIMER0, ENABLE);
 
    /* 自动重装载预装载使能 */
    timer_auto_reload_shadow_enable(TIMER0);
    /* 定时器使能 */
    timer_enable(TIMER0);
}

/* 设置占空比 */
void system_led_pwm_duty(int duty)
{
    if (duty > DUTY_COUNT)
    {
        return ;
    }
    
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, duty);
}