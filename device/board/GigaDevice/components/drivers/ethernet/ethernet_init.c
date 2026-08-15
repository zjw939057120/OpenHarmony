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

#include <stdio.h>
#include "netif/etharp.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/init.h"
#include "ethernetif.h"
#include "lwip/timeouts.h"
#include "lwip/tcpip.h"
#include "lwip/opt.h"
#include "lwip/err.h"
#include "arch/sys_arch.h"

#include "gd32f4xx.h"
#include "gd32f4xx_enet.h"
#include "gd32f4xx_misc.h"
#include "gd32f4xx_rcu.h"
#include "gd32f4xx_gpio.h"
#include "gd32f4xx_syscfg.h"

#include "ethernet_init.h"

#include "los_compiler.h"
#include "los_event.h"
#include "los_interrupt.h"

#define ETH_SUCCESS     0
#define ETH_ERROR       1

__lwip_dev lwipdev;                 /* lwip控制结构体 */
struct netif lwip_netif;            /* 定义一个全局的网络接口 */

extern UINT32 g_semId;

/* 以太网中断处理 */
void ENET_IRQHandler(void)
{
    /* clear the enet DMA Rx interrupt pending bits */
    enet_interrupt_flag_clear(ENET_DMA_INT_FLAG_RS_CLR);
    enet_interrupt_flag_clear(ENET_DMA_INT_FLAG_NI_CLR);   
    
    /* handles all the received frames */    
    if(enet_rxframe_size_get())
    {
        LOS_SemPost(g_semId);
    }  
}
/* 初始化ETH MAC层及DMA配置 网卡相关的配置 */
unsigned char ethernet_mac_mda_config(void)
{
    rcu_periph_clock_enable(RCU_ENET);
    rcu_periph_clock_enable(RCU_ENETTX);
    rcu_periph_clock_enable(RCU_ENETRX);

    enet_deinit();  /* AHB总线重启以太网 */  
    enet_software_reset(); /* 软件重启网络 */
	
    /* 改为自动、强制模式切换 */
    /* 通过硬件计算和验证 IP、UDP、TCP 和 ICMP 校验和 */
    ErrStatus enet_init_status = enet_init(ENET_10M_FULLDUPLEX, ENET_AUTOCHECKSUM_ACCEPT_FAILFRAMES, ENET_BROADCAST_FRAMES_DROP);
   
    if(SUCCESS == enet_init_status)   
    {   
        enet_interrupt_enable(ENET_DMA_INT_NIE);
        enet_interrupt_enable(ENET_DMA_INT_RIE);

        int ret = LOS_HwiCreate(ENET_IRQn, 0, 0, (HWI_PROC_FUNC)ENET_IRQHandler, NULL);  /* 中断号 优先级 中断模式 处理函数 传入参数 */
        if (ret != LOS_OK) {
            printf("%s:%d Interrupt Create fail! ret=%x  ENET_IRQn=%d\n", __FILE__, __LINE__, ret, ENET_IRQn);
        }

        /* configure systick clock source as HCLK */
        systick_clksource_set(SYSTICK_CLKSOURCE_HCLK);

        return ETH_SUCCESS;   
    } else {
       return  ETH_ERROR;
    }     
}
/* 初始化 gpio */
unsigned char ethernet_gpio_init(void)
{
    unsigned char rval = ETH_ERROR;   

	/* 配置嵌套矢量中断控制器 */
    nvic_irq_enable(ENET_IRQn, 1, 0);

    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    
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

    /* PB11: ETH_RMII_TX_EN */
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_11);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_11);

    /* PB12: ETH_RMII_TXD0 */
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_12);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_12);

    /* PB13: ETH_RMII_TXD1 */
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_13);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_13);

    gpio_af_set(GPIOB, GPIO_AF_11, GPIO_PIN_11);
    gpio_af_set(GPIOB, GPIO_AF_11, GPIO_PIN_12);
    gpio_af_set(GPIOB, GPIO_AF_11, GPIO_PIN_13);

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
     
    rval = ethernet_mac_mda_config(); /* 配置MAC及DMA */
    
    return rval;
}

/* 设置默认 IP 和 MAC */
static void lwip_comm_default_ip_set(__lwip_dev *lwipx)
{
    uint32_t sn0;
    sn0 = *(__IO uint32_t *)(0x1FFF7A10); /* 获取 GD32 的唯一ID的前24位作为MAC地址后三字节 (stm32 0x1FFFF7F0) */
    
    /* 默认远端IP为:192.168.1.134 */
    lwipx->remoteip[0] = 192;
    lwipx->remoteip[1] = 168;
    lwipx->remoteip[2] = 1;
    lwipx->remoteip[3] = 27;
    
    /* MAC地址设置(高2字节固定为:30,d0,低4字节用 GD32 唯一ID) */
    lwipx->mac[0] = 0x30;
    lwipx->mac[1] = 0xd0;
    lwipx->mac[2] = (sn0 >> 24) & 0XFF; /* 低4字节用 GD32 的唯一ID */
    lwipx->mac[3] = (sn0 >> 16) & 0XFF;
    lwipx->mac[4] = (sn0 >> 8) & 0XFFF;;
    lwipx->mac[5] = sn0 & 0XFF;

    /* 默认本地IP为:192.168.1.30 */
    lwipx->ip[0] = 192;
    lwipx->ip[1] = 168;
    lwipx->ip[2] = 0;
    lwipx->ip[3] = 132;
    /* 默认子网掩码:255.255.255.0 */
    lwipx->netmask[0] = 255;
    lwipx->netmask[1] = 255;
    lwipx->netmask[2] = 255;
    lwipx->netmask[3] = 0;
    
    /* 默认网关:192.168.1.1 */
    lwipx->gateway[0] = 192;
    lwipx->gateway[1] = 168;
    lwipx->gateway[2] = 0;
    lwipx->gateway[3] = 1;
    lwipx->dhcpstatus = 0; /* 没有DHCP */
}

unsigned char ethernet_init(void)
{
    uint8_t retry = 0;
    struct netif *netif_init_flag;              /* 调用netif_add()函数时的返回值,用于判断网络初始化是否成功 */
    ip_addr_t ipaddr;                           /* ip地址 */
    ip_addr_t netmask;                          /* 子网掩码 */
    ip_addr_t gw;                               /* 默认网关 */

    lwip_comm_default_ip_set(&lwipdev);         /* 设置默认IP等信息 */

    while (ethernet_gpio_init())                /* 初始化以太网芯片,如果失败的话就重试5次 */
    {
        retry++;
        if (retry > ETHERNET_INIT_COUNT)
        {
            retry = 0;                          /* 以太网芯片初始化失败 */
            return LWIP_NICE_INIT_ERR;
        }
    }

#if LWIP_DHCP                                   /* 使用动态IP */
    ipaddr.addr = 0;
    netmask.addr = 0;
    gw.addr = 0;
#else   /* 使用静态IP */
    IP4_ADDR(&ipaddr, lwipdev.ip[0], lwipdev.ip[1], lwipdev.ip[2], lwipdev.ip[3]);
    IP4_ADDR(&netmask, lwipdev.netmask[0], lwipdev.netmask[1], lwipdev.netmask[2], lwipdev.netmask[3]);
    IP4_ADDR(&gw, lwipdev.gateway[0], lwipdev.gateway[1], lwipdev.gateway[2], lwipdev.gateway[3]);
    printf("MAC ................%x.%x.%x.%x.%x.%x\r\n", lwipdev.mac[0], lwipdev.mac[1], lwipdev.mac[2], lwipdev.mac[3], lwipdev.mac[4], lwipdev.mac[5]);
    printf("STATIC IP........................%d.%d.%d.%d\r\n", lwipdev.ip[0], lwipdev.ip[1], lwipdev.ip[2], lwipdev.ip[3]);
    printf("MASK ..........................%d.%d.%d.%d\r\n", lwipdev.netmask[0], lwipdev.netmask[1], lwipdev.netmask[2], lwipdev.netmask[3]);
    printf("GATEWAY ..........................%d.%d.%d.%d\r\n", lwipdev.gateway[0], lwipdev.gateway[1], lwipdev.gateway[2], lwipdev.gateway[3]);
#endif  /* 向网卡列表中添加一个网口 */
    netif_init_flag = netif_add(&lwip_netif, (const ip_addr_t *)&ipaddr, (const ip_addr_t *)&netmask, (const ip_addr_t *)&gw, NULL, ethernetif_init, tcpip_input);


    if (netif_init_flag == NULL)
    {
        return LWIP_NICE_ADD_ERR;           /* 网卡添加失败 */
    } else {                                /* 网口添加成功后,设置netif为默认值,并且打开netif网口 */
        netif_set_default(&lwip_netif);     /* 设置netif为默认网口 */
        netif_set_up(&lwip_netif);          /* 打开netif网口 */
    }

	tcpip_init(NULL, NULL);

    return ETH_SUCCESS;                     /* 操作OK. */
}

/* 解决 __h_errno_location 没有的报错，https://gitee.com/openharmony/kernel_liteos_m/issues/I60GI5 */
#include <netdb.h>
#undef h_errno
int h_errno;

int *__h_errno_location(void)
{
    return &h_errno;
}