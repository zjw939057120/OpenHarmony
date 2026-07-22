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

#ifndef _TARGET_CONFIG_H
#define _TARGET_CONFIG_H
#include "gd32f4xx.h"
#include "global_config.h"

/* heap */
extern unsigned int __los_heap_addr_start__;
extern unsigned int __los_heap_addr_end__;
#define LOSCFG_SYS_EXTERNAL_HEAP 1
#define LOSCFG_SYS_HEAP_ADDR ((void *)&__los_heap_addr_start__)
#define LOSCFG_SYS_HEAP_SIZE (((unsigned long)&__los_heap_addr_end__) - ((unsigned long)&__los_heap_addr_start__))

/* sysclock */
#define LOSCFG_BASE_CORE_TICK_PER_SECOND                    (1000UL)
#define LOSCFG_BASE_CORE_TICK_RESPONSE_MAX                  0xFFFFFFUL
#if defined(CONFIG_SYS_CLOCK)
#define OS_SYS_CLOCK                                        CONFIG_SYS_CLOCK
#else
#define OS_SYS_CLOCK                                        240000000
#endif

/* interrupt */
#define LOSCFG_PLATFORM_HWI 1
#define LOSCFG_USE_SYSTEM_DEFINED_INTERRUPT 1
/* 中断向量表地址对齐 */
#define LOSCFG_ARCH_HWI_VECTOR_ALIGN	0x200
/* 从GD32F470芯片手册得到最后一个中断号 IPA_IRQn = 90 */
#define LOSCFG_PLATFORM_HWI_LIMIT 		91

/* task */
#define LOSCFG_BASE_CORE_TSK_LIMIT 		32
/* sem */
#define LOSCFG_BASE_IPC_SEM_LIMIT 		32
/* mutex */
#define LOSCFG_BASE_IPC_MUX_LIMIT 		32

#endif
