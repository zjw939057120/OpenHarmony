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

#ifndef _ETHERNET_INIT_H_
#define _ETHERNET_INIT_H_ 

#define ETHERNET_INIT_COUNT     5       /* 以太网初始化最大重试次数 */
#define LWIP_MAX_DHCP_TRIES     3       /* DHCP服务器最大重试次数 */


/*lwip控制结构体*/
typedef struct  
{
    unsigned char mac[6];               /* MAC地址 */
    unsigned char remoteip[4];          /* 远端主机IP地址 */ 
    unsigned char ip[4];                /* 本机IP地址 */
    unsigned char netmask[4];           /* 子网掩码 */
    unsigned char gateway[4];           /* 默认网关的IP地址 */
    unsigned char dhcpstatus;           /* dhcp状态 */
}__lwip_dev;

typedef enum {
    ETH_LINK_UP,  /*!< Ethernet link is up */
    ETH_LINK_DOWN /*!< Ethernet link is down */
} eth_link_t;

typedef enum {
    LWIP_INIT_SUCCESS,
    LWIP_INIT_MEM_ERR,
    LWIP_NICE_INIT_ERR,
    LWIP_NICE_ADD_ERR,
} eth_init_t;

extern __lwip_dev lwipdev;

eth_link_t get_ip101gr_status(void);

/* 初始化以太网 */
unsigned char ethernet_init(void);
/* DHCP 处理 */
void start_dhcp_handler();
/* 获取网络状态 */
eth_link_t get_network_link(void);

#endif /* _ETHERNET_INIT_H_ */