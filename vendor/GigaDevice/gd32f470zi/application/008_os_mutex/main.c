#include <stdio.h>
#include <ohos_init.h>
#include "los_task.h"
#include "los_mux.h"


#define TASK_STACK_SIZE     4096
#define TASK_PRIORITY0      8       /* 值越大，优先级越高 */
#define TASK_PRIORITY1      7
#define TASK_PRIORITY2      6

UINT32 g_mutex;

VOID os_mutex_thread0(VOID)
{
    while (1)
    {
        // 获取互斥锁
        if (LOS_MuxPend(g_mutex, LOS_WAIT_FOREVER) == LOS_OK) {
            // 临界区：访问共享资源
            printf("Task 1 get mutex.\n");
            // 释放互斥锁
            LOS_Msleep(3000);
            printf("Task 1 post mutex.\n");
            LOS_MuxPost(g_mutex);
        }
        LOS_Msleep(3000);
    }
}
VOID os_mutex_thread1(VOID)
{
    while (1)
    {
        // 获取互斥锁
        if (LOS_MuxPend(g_mutex, LOS_WAIT_FOREVER) == LOS_OK) {
            // 临界区：访问共享资源
            printf("Task 2 get mutex.\n");
            // 释放互斥锁
            LOS_Msleep(3000);
            printf("Task 2 post mutex.\n");
            LOS_MuxPost(g_mutex);
        }
        LOS_Msleep(2000);
    }
}
VOID os_mutex_thread2(VOID)
{
    while (1)
    {
        // 获取互斥锁
        if (LOS_MuxPend(g_mutex, LOS_WAIT_FOREVER) == LOS_OK) {
            // 临界区：访问共享资源
            printf("Task 3 get mutex.\n");
            // 释放互斥锁
            LOS_Msleep(3000);
            printf("Task 3 post mutex.\n");
            LOS_MuxPost(g_mutex);
        }
        LOS_Msleep(1000);
    }
}

static void os_mutex_handler(void)
{
    UINT32 ret;

    /* 互斥锁初始化 */
    ret = LOS_MuxCreate(&g_mutex);
    if (ret != LOS_OK) {
        printf("Failed to Create mutex! Error: 0x%x\n", ret);
    }

    TSK_INIT_PARAM_S g_task0 = {0};
    UINT32 g_task_id0;
    g_task0.pfnTaskEntry = (TSK_ENTRY_FUNC)os_mutex_thread0;
    g_task0.uwStackSize = TASK_STACK_SIZE;
    g_task0.pcName = "os_mutex_thread0";
    g_task0.usTaskPrio = TASK_PRIORITY0;
    if (LOS_TaskCreate(&g_task_id0, &g_task0) != LOS_OK) {
        printf("Create os_mutex_thread0 failed!\n");
    }

    TSK_INIT_PARAM_S g_task1 = {0};
    UINT32 g_task_id1;
    g_task1.pfnTaskEntry = (TSK_ENTRY_FUNC)os_mutex_thread1;
    g_task1.uwStackSize = TASK_STACK_SIZE;
    g_task1.pcName = "os_mutex_thread1";
    g_task1.usTaskPrio = TASK_PRIORITY1;
    if (LOS_TaskCreate(&g_task_id1, &g_task1) != LOS_OK) {
        printf("Create os_mutex_thread1 failed!\n");
    }

    TSK_INIT_PARAM_S g_task2 = {0};
    UINT32 g_task_id2;
    g_task2.pfnTaskEntry = (TSK_ENTRY_FUNC)os_mutex_thread2;
    g_task2.uwStackSize = TASK_STACK_SIZE;
    g_task2.pcName = "os_mutex_thread2";
    g_task2.usTaskPrio = TASK_PRIORITY2;
    if (LOS_TaskCreate(&g_task_id2, &g_task2) != LOS_OK) {
        printf("Create os_mutex_thread2 failed!\n");
    }
}

APP_FEATURE_INIT(os_mutex_handler);
