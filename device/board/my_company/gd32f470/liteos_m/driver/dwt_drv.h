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

#ifndef __DWT_DRV_H__
#define __DWT_DRV_H__

#include <stdint.h>

void DWT_Init(void);

void DWT_DelayUs(uint32_t us);

void DWT_DelayMs(uint32_t ms);

uint32_t DWT_GetTick(void);

#endif /* __DWT_DRV_H__ */
