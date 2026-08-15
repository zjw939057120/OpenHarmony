#include <stdio.h>
#include <ohos_init.h>
#include "los_task.h"


#define TASK_STACK_SIZE     4096
#define TASK_PRIORITY       6       /* 值越大，优先级越高 */

VOID os_thread(VOID)
{
    while (1) {
        printf(">> %s %d\n", __FILE__, __LINE__);
        LOS_TaskDelay(5000);
    }
}

VOID os_thread_handler(VOID)
{
    UINT32 ret;
    UINT32 g_task_id;
    TSK_INIT_PARAM_S g_task = {0};

    g_task.pfnTaskEntry = (TSK_ENTRY_FUNC)os_thread;
    g_task.uwStackSize = TASK_STACK_SIZE;
    g_task.pcName = "os_thread";
    g_task.usTaskPrio = TASK_PRIORITY;
    ret = LOS_TaskCreate(&g_task_id, &g_task);
    if (ret != LOS_OK) {
        printf("Create os_thread failed!\n");
    }	
}

APP_FEATURE_INIT(os_thread_handler);
