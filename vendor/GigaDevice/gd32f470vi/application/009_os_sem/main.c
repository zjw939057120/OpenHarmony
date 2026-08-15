#include <stdio.h>
#include <ohos_init.h>
#include "los_task.h"
#include "los_sem.h"


#define TASK_STACK_SIZE     4096
#define TASK_PRIORITY       6       /* 值越大，优先级越高 */

UINT32 g_sem;

VOID os_sem_production(VOID)
{
    while (1)
    {
        LOS_TaskDelay(2000);
        printf("Producing an item ...\n");
        LOS_SemPost(g_sem);
    }
}
VOID os_sem_consumption(VOID)
{
    while (1)
    {
        if (LOS_SemPend(g_sem, LOS_WAIT_FOREVER) == LOS_OK) {
            printf("Consuming an item ...\n\n");
        }
    }
}

static void os_sem_handler(void)
{
    UINT32 ret;

    /* 创建信号量 */
    ret = LOS_SemCreate(0, &g_sem); /* 初始值为 0，表示没有可用项目 */
    if (ret != LOS_OK) {
        printf("Failed to create semaphore! Error: 0x%x\n", ret);
    }

    TSK_INIT_PARAM_S g_task0 = {0};
    UINT32 g_task_id0;
    g_task0.pfnTaskEntry = (TSK_ENTRY_FUNC)os_sem_production;
    g_task0.uwStackSize = TASK_STACK_SIZE;
    g_task0.pcName = "os_sem_production";
    g_task0.usTaskPrio = TASK_PRIORITY;
    if (LOS_TaskCreate(&g_task_id0, &g_task0) != LOS_OK) {
        printf("Create os_sem_production failed!\n");
    }

    TSK_INIT_PARAM_S g_task1 = {0};
    UINT32 g_task_id1;
    g_task1.pfnTaskEntry = (TSK_ENTRY_FUNC)os_sem_consumption;
    g_task1.uwStackSize = TASK_STACK_SIZE;
    g_task1.pcName = "os_sem_consumption";
    g_task1.usTaskPrio = TASK_PRIORITY;
    if (LOS_TaskCreate(&g_task_id1, &g_task1) != LOS_OK) {
        printf("Create os_sem_consumption failed!\n");
    }
}

APP_FEATURE_INIT(os_sem_handler);
