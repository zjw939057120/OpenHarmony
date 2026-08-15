/*
 * Copyright (c) 2022 PandaBoard.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gd32f4xx.h"
#include "gd32f4xx_enet.h"
#include "ethernet_init.h"
#include <stdio.h>

#define ETH_PHY_PCR_REG_ADDR        (0x14)
#define ETH_PHY_CSSR_REG_ADDR       (0x1E)
#define ETH_PHY_ANLPAR_REG_ADDR     (0x05)
#define ETH_PHY_BMCR_REG_ADDR       (0x00)
#define ETH_PHY_BMSR_REG_ADDR       (0x01)
typedef union {
    struct {
        unsigned short int reserved : 7;          /*!< Reserved */
        unsigned short int collision_test : 1;    /*!< Collision test */
        unsigned short int duplex_mode : 1;       /*!< Duplex mode:Full Duplex(1) and Half Duplex(0) */
        unsigned short int restart_auto_nego : 1; /*!< Restart auto-negotiation */
        unsigned short int isolate : 1;           /*!< Isolate the PHY from MII except the SMI interface */
        unsigned short int power_down : 1;        /*!< Power off PHY except SMI interface */
        unsigned short int en_auto_nego : 1;      /*!< Enable auto negotiation */
        unsigned short int speed_select : 1;      /*!< Select speed: 100Mbps(1) and 10Mbps(0) */
        unsigned short int en_loopback : 1;       /*!< Enables transmit data to be routed to the receive path */
        unsigned short int reset : 1;             /*!< Reset PHY registers. This bit is self-clearing. */
    };
    unsigned short int val;
} bmcr_reg_t;
typedef union {
    struct {
        unsigned short int ext_capability : 1;       /*!< Extended register capability */
        unsigned short int jabber_detect : 1;        /*!< Jabber condition detected */
        unsigned short int link_status : 1;          /*!< Link status */
        unsigned short int auto_nego_ability : 1;    /*!< Auto negotiation ability */
        unsigned short int remote_fault : 1;         /*!< Remote fault detected */
        unsigned short int auto_nego_complete : 1;   /*!< Auto negotiation completed */
        unsigned short int mf_preamble_suppress : 1; /*!< Preamble suppression capability for management frame */
        unsigned short int reserved : 1;             /*!< Reserved */
        unsigned short int ext_status : 1;           /*!< Extended Status */
        unsigned short int base100_t2_hdx : 1;       /*!< 100Base-T2 Half Duplex capability */
        unsigned short int base100_t2_fdx : 1;       /*!< 100Base-T2 Full Duplex capability */
        unsigned short int base10_t_hdx : 1;         /*!< 10Base-T Half Duplex capability */
        unsigned short int base10_t_fdx : 1;         /*!< 10Base-T Full Duplex capability */
        unsigned short int base100_tx_hdx : 1;       /*!< 100Base-Tx Half Duplex capability */
        unsigned short int base100_tx_fdx : 1;       /*!< 100Base-Tx Full Duplex capability */
        unsigned short int based100_t4 : 1;          /*!< 100Base-T4 capability */
    };
    unsigned short int val;
} bmsr_reg_t;
typedef union {
    struct {
        unsigned short int op_mode : 3;    /* LED 模式 */
        unsigned short int force_mdix : 1; /* Force the MDIX channel to be selected */
        unsigned short int reserved1 : 4;  /* Reserved */
        unsigned short int link_up : 1;    /* Indicate the link status is OK or FAIL */
        unsigned short int reserved2 : 7;  /* Reserved */
    };
    unsigned short int val;
} cssr_reg_t;
typedef union {
    struct {
        unsigned short int protocol_select : 5;  /*!< Link Partner’s binary encoded node selector */
        unsigned short int base10_t : 1;         /*!< 10Base-T support */
        unsigned short int base10_t_fd : 1;      /*!< 10Base-T full duplex support */
        unsigned short int base100_tx : 1;       /*!< 100Base-TX support */
        unsigned short int base100_tx_fd : 1;    /*!< 100Base-TX full duplex support */
        unsigned short int base100_t4 : 1;       /*!< 100Base-T4 support */
        unsigned short int symmetric_pause : 1;  /*!< Symmetric pause supported by Link Partner */
        unsigned short int asymmetric_pause : 1; /*!< Asymmetric pause supported by Link Partner */
        unsigned short int reserved : 1;         /*!< Reserved */
        unsigned short int remote_fault : 1;     /*!< Link partner is indicating a remote fault */
        unsigned short int acknowledge : 1;      /*!< Acknowledges from link partner */
        unsigned short int next_page : 1;        /*!< Next page indication */
    };
    unsigned short int val;
} anlpar_reg_t;

/* 获取当前的PHY的连接状态以确认网络的硬件连接，决定是否重新初始化网络 */
eth_link_t get_ip101gr_status(void)
{
    unsigned short int value = 16U;
    cssr_reg_t cssr;
    anlpar_reg_t anlpar;

    enet_phy_write_read(ENET_PHY_WRITE, PHY_ADDRESS, ETH_PHY_PCR_REG_ADDR, &value);
    enet_phy_write_read(ENET_PHY_READ, PHY_ADDRESS, ETH_PHY_CSSR_REG_ADDR, &(cssr.val));
    enet_phy_write_read(ENET_PHY_READ, PHY_ADDRESS, ETH_PHY_ANLPAR_REG_ADDR, &(anlpar.val));

    // printf("\n******************************************\n");
    // printf("* OP mode: %d\n", cssr.op_mode);
    // printf("* MDIX channel: %d\n", cssr.force_mdix);
    // printf("* Link up: %d\n", cssr.link_up);
    // printf("* Link partner: %d\n", anlpar.protocol_select);
    // printf("* 10Base-T: %d\n", anlpar.base10_t);
    // printf("* 10Base-T full duplex: %d\n", anlpar.base10_t_fd);
    // printf("* 100Base-T: %d\n", anlpar.base100_tx);
    // printf("* 100Base-T full duplex: %d\n", anlpar.base100_tx_fd);
    // printf("* 100Base-T4: %d\n", anlpar.base100_t4);
    // printf("******************************************\n");

    return cssr.link_up ? ETH_LINK_UP : ETH_LINK_DOWN;;
}
