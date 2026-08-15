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

#include "ethernet_init.h"
#include "lwip/dhcp.h"

#include "los_task.h"

#if LWIP_DHCP

extern struct netif lwip_netif;
UINT32 dhcp1_handle;
UINT32 dhcp2_handle;

struct dhcp gdhcp;
static unsigned char link_status = ETH_LINK_DOWN;

typedef enum {
    DHCP_START,
    DHCP_WAIT_GET_IP,
    DHCP_SUCCESS,
    DHCP_TIMEOUT,
    DHCP_FAIL,
} dhcp_status_t;

/**
 * DHCP处理任务
 */
void lwip_dhcp_process_handle(void)
{
    uint32_t ip = 0, netmask = 0, gw = 0;
    struct dhcp *dhcp_client;
    switch (lwipdev.dhcpstatus)
    {
        case DHCP_START:     /* 开启DHCP */
            dhcp_start(&lwip_netif);
            lwipdev.dhcpstatus = DHCP_WAIT_GET_IP;             /* 等待通过DHCP获取到的地址 */
            printf("START DHCP ... \n");
            break;
        case DHCP_WAIT_GET_IP:     /* 等待获取到IP地址 */
        {
            ip = lwip_netif.ip_addr.addr;       /* 读取新IP地址 */
            netmask = lwip_netif.netmask.addr;  /* 读取子网掩码 */
            gw = lwip_netif.gw.addr;            /* 读取默认网关 */

            if (ip != 0)    /*正确获取到IP地址的时候*/
            {
                lwipdev.dhcpstatus = DHCP_SUCCESS;         /* DHCP成功 */
                printf("MAC ................%x.%x.%x.%x.%x.%x\r\n", lwipdev.mac[0], lwipdev.mac[1], lwipdev.mac[2], lwipdev.mac[3], lwipdev.mac[4], lwipdev.mac[5]);
                /* 解析出通过DHCP获取到的IP地址 */
                lwipdev.ip[3] = (uint8_t)(ip >> 24);
                lwipdev.ip[2] = (uint8_t)(ip >> 16);
                lwipdev.ip[1] = (uint8_t)(ip >> 8);
                lwipdev.ip[0] = (uint8_t)(ip);
                printf("GET IP ..............%d.%d.%d.%d\r\n", lwipdev.ip[0], lwipdev.ip[1], lwipdev.ip[2], lwipdev.ip[3]);
                /* 解析通过DHCP获取到的子网掩码地址 */
                lwipdev.netmask[3] = (uint8_t)(netmask >> 24);
                lwipdev.netmask[2] = (uint8_t)(netmask >> 16);
                lwipdev.netmask[1] = (uint8_t)(netmask >> 8);
                lwipdev.netmask[0] = (uint8_t)(netmask);
                printf("GET MASK ............%d.%d.%d.%d\r\n", lwipdev.netmask[0], lwipdev.netmask[1], lwipdev.netmask[2], lwipdev.netmask[3]);
                /* 解析出通过DHCP获取到的默认网关 */
                lwipdev.gateway[3] = (uint8_t)(gw >> 24);
                lwipdev.gateway[2] = (uint8_t)(gw >> 16);
                lwipdev.gateway[1] = (uint8_t)(gw >> 8);
                lwipdev.gateway[0] = (uint8_t)(gw);
                printf("GET GATEWAY ..........%d.%d.%d.%d\r\n", lwipdev.gateway[0], lwipdev.gateway[1], lwipdev.gateway[2], lwipdev.gateway[3]);
            } else {/* 通过DHCP服务获取IP地址失败,且超过最大尝试次数 */
                dhcp_client = netif_dhcp_data(&lwip_netif);
                if (dhcp_client->tries > LWIP_MAX_DHCP_TRIES)
                {
                    dhcp_stop(&lwip_netif);
                    lwipdev.dhcpstatus = DHCP_TIMEOUT;  /* DHCP超时失败. */
                    /* 使用静态IP地址 */
                    IP4_ADDR(&(lwip_netif.ip_addr), lwipdev.ip[0], lwipdev.ip[1], lwipdev.ip[2], lwipdev.ip[3]);
                    IP4_ADDR(&(lwip_netif.netmask), lwipdev.netmask[0], lwipdev.netmask[1], lwipdev.netmask[2], lwipdev.netmask[3]);
                    IP4_ADDR(&(lwip_netif.gw), lwipdev.gateway[0], lwipdev.gateway[1], lwipdev.gateway[2], lwipdev.gateway[3]);
                    netif_set_addr(&lwip_netif, &lwip_netif.ip_addr, &lwip_netif.netmask, &lwip_netif.gw);
                    netif_set_default(&lwip_netif); 
                    netif_set_up(&lwip_netif);
                    printf("DHCP TIMEOUT! \n");
                    printf("MAC ................%x.%x.%x.%x.%x.%x\r\n", lwipdev.mac[0], lwipdev.mac[1], lwipdev.mac[2], lwipdev.mac[3], lwipdev.mac[4], lwipdev.mac[5]);
                    printf("STATIC IP ........................%d.%d.%d.%d\r\n", lwipdev.ip[0], lwipdev.ip[1], lwipdev.ip[2], lwipdev.ip[3]);
                    printf("MASK ..........................%d.%d.%d.%d\r\n", lwipdev.netmask[0], lwipdev.netmask[1], lwipdev.netmask[2], lwipdev.netmask[3]);
                    printf("GATEWAY ..........................%d.%d.%d.%d\r\n", lwipdev.gateway[0], lwipdev.gateway[1], lwipdev.gateway[2], lwipdev.gateway[3]);
                }
            }
        }
        break;

        default :
            lwipdev.dhcpstatus = DHCP_FAIL;
            printf("+++++++++++++ %s %d %d\n", __func__, __LINE__, lwipdev.dhcpstatus);
            break;
    }
}

VOID dhcp_task_handler(VOID)
{
    while((lwipdev.dhcpstatus != DHCP_SUCCESS)&&(lwipdev.dhcpstatus != DHCP_TIMEOUT))  /* 等待DHCP获取 成功/超时 */
    {
        lwip_dhcp_process_handle();
        LOS_TaskDelay(5);
    }  

    LOS_TaskDelete(dhcp2_handle);
}

void dhcp_task(void *pvParameters)
{
    UINT32 uwRet;
    TSK_INIT_PARAM_S stTask = {0};

    stTask.pfnTaskEntry = (TSK_ENTRY_FUNC)dhcp_task_handler;
    stTask.uwStackSize = 2048;
    stTask.pcName = "dhcp_task_handler";
    stTask.usTaskPrio = 6;
    
    while (1) {
        if(get_ip101gr_status() == ETH_LINK_UP && link_status == ETH_LINK_DOWN)
        {
            printf("ethernet link-up\n");
            link_status = ETH_LINK_UP;
            lwipdev.dhcpstatus = DHCP_START;
            lwip_netif.ip_addr.addr = 0;
            lwip_netif.netmask.addr = 0;
            lwip_netif.gw.addr = 0;
            uwRet = LOS_TaskCreate(&dhcp2_handle, &stTask);     /* 创建新的线程来处理 DHCP 过程 */
            if (uwRet != LOS_OK) {
                printf("sample_task create failed\r\n");
            }
        }

        if(get_ip101gr_status() == ETH_LINK_DOWN && link_status == ETH_LINK_UP)
        {
            printf("ethernet link-down\n");
            link_status = ETH_LINK_DOWN;    
            if (dhcp2_handle != NULL)
            {
                LOS_TaskDelete(dhcp2_handle);     /* 删除 DHCP 过程的线程 */
            }
        }

        LOS_TaskDelay(1000);
    }
    
    LOS_TaskDelete(dhcp1_handle);
}

eth_link_t get_network_link(void)
{
    if (link_status == ETH_LINK_UP && lwipdev.dhcpstatus == DHCP_SUCCESS)
    {
        return ETH_LINK_UP;
    }
    return ETH_LINK_DOWN;
}

void start_dhcp_handler()
{
    printf("start dhcp\n");
    UINT32 uwRet;
    TSK_INIT_PARAM_S stTask = {0};

    stTask.pfnTaskEntry = (TSK_ENTRY_FUNC)dhcp_task;
    stTask.uwStackSize = 2048;
    stTask.pcName = "dhcp_task";
    stTask.usTaskPrio = 6;
    uwRet = LOS_TaskCreate(&dhcp1_handle, &stTask);
    if (uwRet != LOS_OK) {
        printf("sample_task create failed\r\n");
    }
}
#endif