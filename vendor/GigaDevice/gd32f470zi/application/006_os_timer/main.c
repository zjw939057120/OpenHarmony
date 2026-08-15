#include <stdio.h>
#include <ohos_init.h>
#include "los_swtmr.h"


UINT32 g_timer;

static void timer_callback(UINT32 arg)
{
    UINT32 ret = LOS_OK;
    static uint8_t count = 0;
    count++;
    printf("The software timer rings for the %d time ~\n", count);

    /* 定时器工作5次后删除 */
    if (5 == count)
    {
        ret = LOS_SwtmrDelete(g_timer);//删除软件定时器
        if (LOS_OK != ret)
        {
            printf("Failed to remove the software timer ~\n");
        } else
        {
            printf("Remove the software timer ~\n");
        }
    }
}

static void os_timer_handler(void)
{
    UINT32 ret = LOS_OK;

#if (LOSCFG_BASE_CORE_SWTMR_ALIGN == 1)
    /* 创建一个软件定时器定时器  定时时间 工作模式 回调函数 定时器ID 回调函数入参 触发定时器时的事件 定时器的灵敏度 */
    ret = LOS_SwtmrCreate(3000, LOS_SWTMR_MODE_PERIOD, (SWTMR_PROC_FUNC)timer_callback, &g_timer, 0, 0, 0);
    if (ret != LOS_OK)
    {
        printf("Software timer creation failed! Error: 0x%x\n", ret);
    }
#else
    /* 创建一个软件定时器定时器  定时时间 工作模式 回调函数 定时器ID 回调函数入参 */
    ret = LOS_SwtmrCreate(3000, LOS_SWTMR_MODE_PERIOD, (SWTMR_PROC_FUNC)timer_callback, &g_timer, 0);
    if (ret != LOS_OK)
    {
        printf("Software timer creation failed! Error: 0x%x\n", ret);
    }
#endif

    /* 开始运行定时器 */
    LOS_SwtmrStart(g_timer);
}

APP_FEATURE_INIT(os_timer_handler);
