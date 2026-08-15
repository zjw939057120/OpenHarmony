#include "los_config.h"
#include "los_debug.h"
#include "los_interrupt.h"
#include "los_task.h"
#include "los_tick.h"

#include "uart.h"
#include "sys_led.h"

#if defined(LOSCFG_SUPPORT_LITTLEFS)
#include "lfs_adapter.h"
#endif

#if (LOSCFG_FILE_SYSTEM_TEST == 1) && defined(LOSCFG_SUPPORT_LITTLEFS)
#include "file_test.h"
#endif

#if (LOSCFG_ETHERNET_ENABLE == 1)
#include "ethernet_init.h"
#endif

int main(void)
{
	UINT32 ret;   
    systick_config(); 
	uart_init();       
    system_led_init();
    
	//内核初始化
    ret = LOS_KernelInit();
    if (ret != LOS_OK) {
        printf("LiteOS kernel init failed! ERROR: 0x%x\n", ret);
        while(1){}
    }
    uart_irq_register();

#if defined(LOSCFG_SUPPORT_LITTLEFS)
    // lfs_init();
#endif
#if (LOSCFG_FILE_SYSTEM_TEST == 1) && defined(LOSCFG_SUPPORT_LITTLEFS)
    // file_system_test();
#endif
    printf("Open Harmony 4.1.1 start ...\r\n\r\n");

    extern void OHOS_SystemInit(void);
    OHOS_SystemInit();

#if (LOSCFG_ETHERNET_ENABLE == 1)
    // ethernet_init();
    // start_dhcp_handler();
#endif

#if (LOSCFG_USE_SHELL == 1)
    ret = LosShellInit();
    if (ret != LOS_OK) {
        printf("LosShellInit failed! ERROR: 0x%x\n", ret);
    }
#endif

    LOS_Start();	
	while (1){}
}
