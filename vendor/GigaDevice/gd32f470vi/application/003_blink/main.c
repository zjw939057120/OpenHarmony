#include <stdio.h>
#include <ohos_init.h>
#include "los_task.h"
#include "sys_led.h"


#define TASK_STACK_SIZE     4096
#define TASK_PRIORITY       6       /* 值越大，优先级越高 */

#define BLINK_TRUE          1
#define BLINK_FALSE         0

VOID blink_task(VOID)
{
    int i = 0;
    uint8_t flg = BLINK_FALSE;
    while (1)
    {    
        if (i > 25000)
        {
            flg = BLINK_TRUE;
        }
        if (i < 1)
        {
            flg = BLINK_FALSE;
        }

        if (flg == BLINK_FALSE)
        {
            i = i + 50;
        }
        if (flg == BLINK_TRUE)
        {
            i = i - 50;
        }
        system_led_pwm_duty(i);
        LOS_TaskDelay(2);
    }
}

VOID blink_task_handler(VOID)
{
    UINT32 ret;
    UINT32 g_task_id;
    TSK_INIT_PARAM_S g_task = {0};

    system_led_pwm_init();

    g_task.pfnTaskEntry = (TSK_ENTRY_FUNC)blink_task;
    g_task.uwStackSize = TASK_STACK_SIZE;
    g_task.pcName = "blink_task";
    g_task.usTaskPrio = TASK_PRIORITY;
    ret = LOS_TaskCreate(&g_task_id, &g_task);
    if (ret != LOS_OK) {
        printf("blink_task create failed.\n");
    }	
}

APP_FEATURE_INIT(blink_task_handler);
