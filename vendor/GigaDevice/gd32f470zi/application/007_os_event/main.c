#include <stdio.h>
#include <ohos_init.h>
#include "los_task.h"
#include "los_event.h"


#define TASK_STACK_SIZE     4096
#define TASK_PRIORITY       6       /* 值越大，优先级越高 */
/* 等待的事件类型 */
#define EVENT_WAIT_TYPE     0x00000001

EVENT_CB_S g_event;

VOID os_event_write(VOID)
{
    UINT32 ret;
    while (1)
    {
        printf("Event write.\n");
        ret = LOS_EventWrite(&g_event, EVENT_WAIT_TYPE);
        if (ret != LOS_OK) {
            printf("Event write failed.\n");
        }
        LOS_Msleep(5000);
    }
}
VOID os_event_read(VOID)
{
    UINT32 ret = 0;
    while (1)
    {
        /* 超时等待方式读事件, 当读事件超时，任务直接唤醒, 
           等待的时间为4秒，时间发起周期是5秒，当发生超时后的1秒，发送事件后，立马能读取到事件 */
        ret = LOS_EventRead(&g_event, EVENT_WAIT_TYPE, LOS_WAITMODE_AND, 4000);

        if (ret == EVENT_WAIT_TYPE) {
            printf("Read event: 0x%x\n", ret);
            LOS_EventClear(&g_event, ~g_event.uwEventID);
        }else{
            printf("Event wait timeout ~\n");
        }
    }
}
VOID os_other_task(VOID)
{
    while (1)
    {
        printf("This is an additional task ~\n");
        LOS_Msleep(1000);
    }
}

static void os_event_handler(void)
{
    UINT32 ret;

    /* 事件初始化 */
    ret = LOS_EventInit(&g_event);
    if (ret != LOS_OK) {
        printf("Init event failed. Error: 0x%x\n", ret);
    }

    TSK_INIT_PARAM_S g_task0 = {0};
    UINT32 g_task_id0;
    g_task0.pfnTaskEntry = (TSK_ENTRY_FUNC)os_event_write;
    g_task0.uwStackSize = TASK_STACK_SIZE;
    g_task0.pcName = "os_event_write";
    g_task0.usTaskPrio = TASK_PRIORITY;
    if (LOS_TaskCreate(&g_task_id0, &g_task0) != LOS_OK) {
        printf("Create os_event_write failed!\n");
    }

    TSK_INIT_PARAM_S g_task1 = {0};
    UINT32 g_task_id1;
    g_task1.pfnTaskEntry = (TSK_ENTRY_FUNC)os_event_read;
    g_task1.uwStackSize = TASK_STACK_SIZE;
    g_task1.pcName = "os_event_read";
    g_task1.usTaskPrio = TASK_PRIORITY;
    if (LOS_TaskCreate(&g_task_id1, &g_task1) != LOS_OK) {
        printf("Create os_event_read failed!\n");
    }

    TSK_INIT_PARAM_S g_task2 = {0};
    UINT32 g_task_id2;
    g_task2.pfnTaskEntry = (TSK_ENTRY_FUNC)os_other_task;
    g_task2.uwStackSize = TASK_STACK_SIZE;
    g_task2.pcName = "os_other_task";
    g_task2.usTaskPrio = TASK_PRIORITY;
    if (LOS_TaskCreate(&g_task_id2, &g_task2) != LOS_OK) {
        printf("Create os_other_task failed!\n");
    }
}

APP_FEATURE_INIT(os_event_handler);
