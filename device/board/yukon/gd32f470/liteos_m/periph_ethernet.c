#include "los_task.h"
#include "gd32f4xx.h"
#include "gd32f4xx_enet.h"
#include "utils.h"
#include "periph_ethernet.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/dhcp.h"

static void enet_gpio_config(void);
static void enet_mac_dma_config(void);
UINT32 ethernet_poll_task_init(void);

void periph_ethernet_init(void)
{
    enet_gpio_config();
    enet_mac_dma_config();
    ethernet_poll_task_init();
}

/*!
    \brief      configures the different GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
*/

/* PA1: ETH_RMII_REF_CLK */
/* PA2: ETH_MDIO */
/* PA7: ETH_RMII_CRS_DV */
/* PG11: ETH_RMII_TX_EN */
/* PG13: ETH_RMII_TXD0 */
/* PG14: ETH_RMII_TXD1 */
/* PC1: ETH_MDC */
/* PC4: ETH_RMII_RXD0 */
/* PC5: ETH_RMII_RXD1 */
static void enet_gpio_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOG);
    rcu_periph_clock_enable(RCU_SYSCFG);

    syscfg_enet_phy_interface_config(SYSCFG_ENET_PHY_RMII);

    /* PA1: ETH_RMII_REF_CLK */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_1);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_1);

    /* PA2: ETH_MDIO */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_2);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_2);

    /* PA7: ETH_RMII_CRS_DV */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_7);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_7);

    gpio_af_set(GPIOA, GPIO_AF_11, GPIO_PIN_1);
    gpio_af_set(GPIOA, GPIO_AF_11, GPIO_PIN_2);
    gpio_af_set(GPIOA, GPIO_AF_11, GPIO_PIN_7);

    /* PC1: ETH_MDC */
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_1);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_1);

    /* PC4: ETH_RMII_RXD0 */
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_4);

    /* PC5: ETH_RMII_RXD1 */
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_5);

    gpio_af_set(GPIOC, GPIO_AF_11, GPIO_PIN_1);
    gpio_af_set(GPIOC, GPIO_AF_11, GPIO_PIN_4);
    gpio_af_set(GPIOC, GPIO_AF_11, GPIO_PIN_5);

    /* PG11: ETH_RMII_TX_EN */
    gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_11);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_11);

    /* PG13: ETH_RMII_TXD0 */
    gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_13);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_13);

    /* PG14: ETH_RMII_TXD1 */
    gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_14);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_14);

    gpio_af_set(GPIOG, GPIO_AF_11, GPIO_PIN_11);
    gpio_af_set(GPIOG, GPIO_AF_11, GPIO_PIN_13);
    gpio_af_set(GPIOG, GPIO_AF_11, GPIO_PIN_14);
}

/*!
    \brief      configures the ethernet MAC and DMA
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void enet_mac_dma_config(void)
{
    ErrStatus reval_state = ERROR;
    uint16_t phy_value = 0U;

    rcu_periph_clock_enable(RCU_ENET);
    rcu_periph_clock_enable(RCU_ENETTX);
    rcu_periph_clock_enable(RCU_ENETRX);

    enet_deinit();

    reval_state = enet_software_reset();
    if(ERROR == reval_state) {
        printf("[ETH] software reset failed!\n");
        return;
    }
    printf("[ETH] software reset OK\n");

    reval_state = enet_init(ENET_100M_FULLDUPLEX, ENET_AUTOCHECKSUM_DROP_FAILFRAMES, ENET_BROADCAST_FRAMES_PASS);
    if(ERROR == reval_state) {
        printf("[ETH] enet_init failed! Check PHY address and MDIO/MDC wiring.\n");
        return;
    }
    printf("[ETH] enet_init OK\n");

    enet_descriptors_ring_init(ENET_DMA_TX);
    enet_descriptors_ring_init(ENET_DMA_RX);

    uint8_t mac_addr[6] = {0x5e, 0x4d, 0x3c, 0x2b, 0x1a, 0x00};
    generate_mac_from_uid(mac_addr);
    enet_mac_address_set(ENET_MAC_ADDRESS0, mac_addr);

    enet_enable();
    printf("[ETH] ENET enabled, MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
           mac_addr[5], mac_addr[4], mac_addr[3], mac_addr[2], mac_addr[1], mac_addr[0]);

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