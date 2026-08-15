#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ohos_init.h>
#include "los_task.h"
#include "los_queue.h"


#define TASK_STACK_SIZE     4096
#define TASK_PRIORITY       6       /* 值越大，优先级越高 */

#define QUEUE_SIZE          10      /* 队列长度 */
#define MESSAGE_SIZE        32      /* 单个消息最大值 */

UINT32 g_queue;

VOID os_queue_send(VOID)
{
    char message[MESSAGE_SIZE] = {0};
    UINT32 count = 0;

    while (1)
    {
        snprintf(message, MESSAGE_SIZE, "this is the %d message", count++);

        /* 发送消息到队列 带拷贝 */
        if (LOS_QueueWriteCopy(g_queue, message, MESSAGE_SIZE, LOS_WAIT_FOREVER) == LOS_OK) {
            printf("Sent message: %s\n", message);
        } else {
            printf("Failed to send message.\n");
        }
        LOS_TaskDelay(1500);
    }
}
VOID os_queue_recv(VOID)
{
    char message[MESSAGE_SIZE] = {0};
    UINT32 len = MESSAGE_SIZE;
    while (1)
    {
        /* 从队列中读取消息 */
        if (LOS_QueueReadCopy(g_queue, message, &len, LOS_WAIT_FOREVER) == LOS_OK) {
            printf("Received message: %s\n\n", (char *)message);
        } else {
            printf("Failed to receive message.\n");
        }
        LOS_TaskDelay(2000);
    }
}

static void os_queue_handler(void)
{
    UINT32 ret;

    /* 创建一个消息队列 */
    ret = LOS_QueueCreate("queue", QUEUE_SIZE, &g_queue, 0, MESSAGE_SIZE); /* 队列名 队列长度 队列ID 队列标志位 单个消息最大值 */
    if (ret != LOS_OK) {
        /* 错误类型 0x2000311 */
        /* 原因是通过 APP_FEATURE_INIT 启动的函数，申请的空间来自 bootstrap 剩余空间 */
        /* base/startup/init/begetd.gni 增大 bootstrap 的内存 config_ohos_startup_init_lite_memory_size */
        printf("Failed to create message queue! Error: 0x%x\n", ret);
    }

    TSK_INIT_PARAM_S g_task0 = {0};
    UINT32 g_task_id0;
    g_task0.pfnTaskEntry = (TSK_ENTRY_FUNC)os_queue_send;
    g_task0.uwStackSize = TASK_STACK_SIZE;
    g_task0.pcName = "os_queue_send";
    g_task0.usTaskPrio = TASK_PRIORITY;
    if (LOS_TaskCreate(&g_task_id0, &g_task0) != LOS_OK) {
        printf("Create os_queue_send failed!\n");
    }

    TSK_INIT_PARAM_S g_task1 = {0};
    UINT32 g_task_id1;
    g_task1.pfnTaskEntry = (TSK_ENTRY_FUNC)os_queue_recv;
    g_task1.uwStackSize = TASK_STACK_SIZE;
    g_task1.pcName = "os_queue_recv";
    g_task1.usTaskPrio = TASK_PRIORITY;
    if (LOS_TaskCreate(&g_task_id1, &g_task1) != LOS_OK) {
        printf("Create os_queue_recv failed!\n");
    }
}

APP_FEATURE_INIT(os_queue_handler);
