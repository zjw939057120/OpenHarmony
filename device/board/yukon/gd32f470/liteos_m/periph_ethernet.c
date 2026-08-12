#include "los_task.h"
#include "gd32f4xx.h"
#include "gd32f4xx_enet.h"
#include "utils.h"
#include "periph_ethernet.h"
#include "gd32f4xx_enet_eval.h"

UINT32 ethernet_poll_task_init(void);

void periph_ethernet_init(void)
{
    enet_system_setup();
    ethernet_poll_task_init();
}


static void *thread_ethernet_poll_task(unsigned int arg) {
    ErrStatus reval_state = ERROR;
    uint16_t phy_value = 0U;
    while(1) {
    reval_state = enet_phy_write_read(ENET_PHY_READ, PHY_ADDRESS, PHY_REG_BSR, &phy_value);
    if(SUCCESS == reval_state) {
        if(phy_value & PHY_LINKED_STATUS) {
            printf("[ETH] PHY link up, BSR=0x%04x\n", phy_value);
        } else {
            printf("[ETH] PHY link down, BSR=0x%04x\n", phy_value);
        }
    } else {
        printf("[ETH] PHY read failed, PHY may not respond. Check PHY_ADDRESS=%u\n", PHY_ADDRESS);
    }

    LOS_TaskDelay(3000);
    }
    return NULL;
}

UINT32 ethernet_poll_task_init(void)
{
	UINT32 taskID;
	TSK_INIT_PARAM_S stTask = {
		.pfnTaskEntry = thread_ethernet_poll_task,
		.uwStackSize = 0x1000,
		.pcName = "ethPollTask",
		.usTaskPrio = 6,
	};
	if (LOS_TaskCreate(&taskID, &stTask) != LOS_OK) {
		return -1;
	}
	return taskID;
}