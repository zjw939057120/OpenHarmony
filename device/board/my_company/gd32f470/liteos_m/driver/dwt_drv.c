/*
 * Copyright (c) 2026 zhangyao
 *
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

#include "gd32f4xx.h"
#include "dwt_drv.h"

void DWT_Init(void) 
{
    /* 使能 TRC (Trace Enable)，这是使用 DWT 的前提 */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

    /* 关闭 CYCCNT 计数器 */
    DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk;
    
    /* 将计数器清零 */
    DWT->CYCCNT = 0;
    
    /* 重新使能 CYCCNT 计数器 */
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

void DWT_DelayUs(uint32_t us)
{
    uint32_t start = DWT->CYCCNT;
    // 将微秒转换为时钟周期数：us * (SystemCoreClock / 1000000)
    uint32_t ticks = us * (SystemCoreClock / 1000000); 
    while ((DWT->CYCCNT - start) < ticks);
}

void DWT_DelayMs(uint32_t ms)
{
    // 毫秒延时建议通过循环调用微秒延时来实现，避免大数乘法溢出
    for (uint32_t i = 0; i < ms; i++)
    {
        DWT_DelayUs(1000);
    }
}

uint32_t DWT_GetTick(void)
{
    return DWT->CYCCNT;
}