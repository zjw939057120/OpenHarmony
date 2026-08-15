/*
 * Copyright (c) 2013-2019 Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**@defgroup los_config System configuration items
 * @ingroup kernel
 */

#ifndef _TARGET_CONFIG_H
#define _TARGET_CONFIG_H

#include "gd32f4xx.h"
#include "gd32f4xx_it.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/*=============================================================================
                                        System clock module configuration
=============================================================================*/
#define OS_SYS_CLOCK                                        240000000                   /* 系统时钟的频率 */
#define LOSCFG_BASE_CORE_TICK_PER_SECOND                    (1000UL)                    /* 系统每秒钟的滴答数（Tick */
#define LOSCFG_BASE_CORE_TICK_HW_TIME                       0                           /* 硬件时间计数器的使用状态 */
#define LOSCFG_BASE_CORE_TICK_WTIMER                        0                           /* 是否使用看门狗定时器进行系统滴答计时 */
#define LOSCFG_BASE_CORE_TICK_RESPONSE_MAX                  0xFFFFFFUL                  /* 系统滴答计时器的最大响应时间 */

/*=============================================================================
                                        Hardware interrupt module configuration
=============================================================================*/
#define LOSCFG_PLATFORM_HWI                                 1       /* 硬件中断（HWI）管理功能 */
#define LOSCFG_USE_SYSTEM_DEFINED_INTERRUPT                 1       /* 系统使用预定义的中断处理机制 */
#define LOSCFG_PLATFORM_HWI_LIMIT                           128     /* 系统中可以注册的最大硬件中断数量 */
#define LOSCFG_ARCH_HWI_VECTOR_ALIGN                        0x200
/*=============================================================================
                                       Task module configuration
=============================================================================*/
#define LOSCFG_BASE_CORE_TSK_LIMIT                          24          /* 创建的最大任务数量 */
#define LOSCFG_BASE_CORE_TSK_IDLE_STACK_SIZE                (0x500U)    /* 空闲任务（Idle Task）的堆栈大小 */
#define LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE             (0x1000U)   /* 默认任务的堆栈大小 */
#define LOSCFG_BASE_CORE_TSK_MIN_STACK_SIZE                 (0x500U)    /* 任务堆栈的最小大小 */
#define LOSCFG_BASE_CORE_TIMESLICE                          1           /* 时间片轮转调度 */
#define LOSCFG_BASE_CORE_TIMESLICE_TIMEOUT                  20000       /* 时间片的超时时间，单位为微秒 */

#define LOSCFG_BASE_CORE_TSK_MONITOR                        1           /* 任务监控功能 */
#define LOSCFG_BASE_CORE_EXC_TSK_SWITCH                     0
/*=============================================================================
                                       Semaphore module configuration
=============================================================================*/
#define LOSCFG_BASE_IPC_SEM                                 1       /* 信号量的功能 */
#define LOSCFG_BASE_IPC_SEM_LIMIT                           48      /* 系统中可以创建的最大信号量数量 */
/*=============================================================================
                                       Mutex module configuration
=============================================================================*/
#define LOSCFG_BASE_IPC_MUX                                 1       /* 互斥量（Mutex）的功能 */
#define LOSCFG_BASE_IPC_MUX_LIMIT                           24      /* 可以创建的最大互斥量数量 */
/*=============================================================================
                                       Queue module configuration
=============================================================================*/
#define LOSCFG_BASE_IPC_QUEUE                               1       /* 进程间通信队列的功能 */
#define LOSCFG_BASE_IPC_QUEUE_LIMIT                         24      /* 系统中可以创建的最大 IPC 队列数量 */
/*=============================================================================
                                       Software timer module configuration
=============================================================================*/
#define LOSCFG_BASE_CORE_SWTMR                              1       /* 系统软件定时器 */
#define LOSCFG_BASE_CORE_SWTMR_ALIGN                        0       /* 设置软件定时器的对齐方式 */
#define LOSCFG_BASE_CORE_SWTMR_LIMIT                        48      /* 系统中可以使用的最大软件定时器数量 */
/*=============================================================================
                                       Memory module configuration
=============================================================================*/
extern unsigned int __heap_start;
extern unsigned int __heap_size;
#define LOSCFG_SYS_EXTERNAL_HEAP                            1
#define LOSCFG_SYS_HEAP_ADDR                                (VOID *)&__heap_start
#define LOSCFG_SYS_HEAP_SIZE                                (UINTPTR)&__heap_size
#define LOSCFG_MEM_MUL_POOL                                 1       /* 多内存池的功能 */
#define OS_SYS_MEM_NUM                                      20      /* 最多可以存在的内存池数量 */
#define LOSCFG_BASE_MEM_NODE_INTEGRITY_CHECK                0       /* 内存节点完整性检查 */
#define LOSCFG_BASE_MEM_NODE_SIZE_CHECK                     0       /* 内存节点大小检查 */
#define LOSCFG_KERNEL_MEM_SLAB                              0       /* 内核内存块分配器（Slab Allocator） */
#define LOSCFG_MEMORY_BESTFIT                               1       /* 最佳适配的内存分配策略 */

// #define LOSCFG_MEM_WATERLINE                                1
// #define LOSCFG_MEM_LEAKCHECK                                1
// #define LOSCFG_MEM_RECORD_LR_CNT                            3
// #define LOSCFG_MEM_OMIT_LR_CNT                              4
/*=============================================================================
                                       Exception module configuration
=============================================================================*/
// #define LOSCFG_PLATFORM_EXC                                 0       /* 平台相关的异常处理功能 */

// #define LOSCFG_PLATFORM_HWI_WITH_ARG                        1       /* 带参数的硬件中断处理功能 */
// #define LOSCFG_BACKTRACE_TYPE                               2       /* 回溯类型 */
// #define LOS_KERNEL_TEST_NOT_SMOKE                           0       /* 非烟雾测试(对系统进行基本的功能检查) */
// #define LOS_KERNEL_HWI_TEST                                 0       /* 硬件中断测试 */
/* =============================================================================
                                       printf module configuration
============================================================================= */
// #define LOSCFG_KERNEL_PRINTF                                1       /* 内核打印功能 */
// #define LOSCFG_BASE_CORE_SCHED_SLEEP                        1       /* 调度器的睡眠功能 */
// #define LOSCFG_KERNEL_PM                                    0       /* 电源管理功能 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _TARGET_CONFIG_H */
