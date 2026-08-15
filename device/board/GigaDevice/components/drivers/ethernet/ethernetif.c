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

#include "lwip/opt.h"
#include "netif/etharp.h"  
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/stats.h"
#include "lwip/snmp.h"
#include "lwip/ethip6.h"
#include "lwip/etharp.h"
#include "netif/etharp.h"
#include "netif/ppp/pppoe.h"
#include "lwip/sys.h"

#include "ethernetif.h" 
#include "ethernet_init.h" 
#include "string.h"

#include "los_task.h"
#include "los_sem.h"
#include "los_interrupt.h"

#include "gd32f4xx_enet.h"
#include "gd32f4xx_timer.h"

/* 定义一个信号量 */
UINT32 g_semId;
extern UINTPTR LOS_IntLock(VOID); 

#define NETIF_IN_TASK_STACK_SIZE    ( 2048 )
#define NETIF_IN_TASK_PRIORITY      ( 6 )

/* Define those to better describe your network interface. */
#define IFNAME0 'G'
#define IFNAME1 'D'

/* ENET RxDMA/TxDMA descriptor */
extern enet_descriptors_struct  rxdesc_tab[ENET_RXBUF_NUM], txdesc_tab[ENET_TXBUF_NUM];

/* ENET receive buffer  */
extern uint8_t rx_buff[ENET_RXBUF_NUM][ENET_RXBUF_SIZE]; 

/* ENET transmit buffer */
extern uint8_t tx_buff[ENET_TXBUF_NUM][ENET_TXBUF_SIZE]; 

/*global transmit and receive descriptors pointers */
extern enet_descriptors_struct  *dma_current_txdesc;
extern enet_descriptors_struct  *dma_current_rxdesc;

/* preserve another ENET RxDMA/TxDMA ptp descriptor for normal mode */
enet_descriptors_struct  ptp_txstructure[ENET_TXBUF_NUM];
enet_descriptors_struct  ptp_rxstructure[ENET_RXBUF_NUM];


/* Forward declarations. */
void  ethernetif_input(void *pParams);

/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
  
static void low_level_init(struct netif *netif)
{
    netif->hwaddr_len = ETHARP_HWADDR_LEN; /* 设置MAC地址长度,为6个字节 */
    /* 初始化MAC地址,设置什么地址由用户自己设置,但是不能与网络中其他设备MAC地址重复 */
    netif->hwaddr[0]=lwipdev.mac[0]; 
    netif->hwaddr[1]=lwipdev.mac[1]; 
    netif->hwaddr[2]=lwipdev.mac[2];
    netif->hwaddr[3]=lwipdev.mac[3];   
    netif->hwaddr[4]=lwipdev.mac[4];
    netif->hwaddr[5]=lwipdev.mac[5];
    
    netif->mtu=1500; /* 最大允许传输单元,允许该网卡广播和ARP功能 */
	
    /* 网卡状态信息标志位，是很重要的控制字段，它包括网卡功能使能、广播 */
    /* 使能、 ARP 使能等等重要控制位 */
    netif->flags = NETIF_FLAG_BROADCAST|NETIF_FLAG_ETHARP|NETIF_FLAG_LINK_UP;   /* 广播 ARP协议 链接检测 */
    
    enet_mac_address_set(ENET_MAC_ADDRESS0, netif->hwaddr);
	
    /* 创建一个信号量 */
    LOS_BinarySemCreate(0, &g_semId);

    /* 创建处理ETH_MAC的任务 */
    sys_thread_new("eth_thread",
                   ethernetif_input,        /* 任务入口函数 */
                   netif,                   /* 任务入口函数参数 */
                   NETIF_IN_TASK_STACK_SIZE,/* 任务栈大小 */
                   NETIF_IN_TASK_PRIORITY); /* 任务的优先级 */

    /* initialize descriptors list: chain/ring mode */
#ifdef SELECT_DESCRIPTORS_ENHANCED_MODE
    enet_ptp_enhanced_descriptors_chain_init(ENET_DMA_TX);
    enet_ptp_enhanced_descriptors_chain_init(ENET_DMA_RX);
#else
    enet_descriptors_chain_init(ENET_DMA_TX);
    enet_descriptors_chain_init(ENET_DMA_RX);
#endif

    for (int i = 0; i < ENET_RXBUF_NUM; i++) {
        enet_rx_desc_immediate_receive_complete_interrupt(&rxdesc_tab[i]);
    }

#ifdef CHECKSUM_BY_HARDWARE
    /* enable the TCP, UDP and ICMP checksum insertion for the Tx frames */
    for (int i = 0; i < ENET_TXBUF_NUM; i++) {
        enet_transmit_checksum_config(&txdesc_tab[i], ENET_CHECKSUM_TCPUDPICMP_FULL);
    }
#endif

    enet_enable();  /* 开启MAC和DMA */
}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become available since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */

static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
    struct pbuf *q;
    int framelength = 0;
    uint8_t *buffer;

    while((uint32_t)RESET != (dma_current_txdesc->status & ENET_TDES0_DAV)){
    }  
    buffer = (uint8_t *)(enet_desc_information_get(dma_current_txdesc, TXDESC_BUFFER_1_ADDR));
    
    /* copy frame from pbufs to driver buffers */
    for(q = p; q != NULL; q = q->next){ 
        memcpy((uint8_t *)&buffer[framelength], q->payload, q->len);
        framelength = framelength + q->len;
    }   
    
    /* note: padding and CRC for transmitted frame 
       are automatically inserted by DMA */

    /* transmit descriptors to give to DMA */ 
#ifdef SELECT_DESCRIPTORS_ENHANCED_MODE
    ENET_NOCOPY_PTPFRAME_TRANSMIT_ENHANCED_MODE(framelength, NULL);
  
#else
    
    ENET_NOCOPY_FRAME_TRANSMIT(framelength);
#endif /* SELECT_DESCRIPTORS_ENHANCED_MODE */

    return ERR_OK;
}

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
static struct pbuf *low_level_input(struct netif *netif)
{  
    struct pbuf *p = NULL, *q;
    uint32_t l = 0;
    u16_t len;
    uint8_t *buffer;

    /* obtain the size of the packet and put it into the "len" variable. */
    len = enet_desc_information_get(dma_current_rxdesc, RXDESC_FRAME_LENGTH);
    buffer = (uint8_t *)(enet_desc_information_get(dma_current_rxdesc, RXDESC_BUFFER_1_ADDR));

    if (len > 0) {
        p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
    }
    if (p != NULL) {
        for (q = p; q != NULL; q = q->next) {
            memcpy_s((uint8_t *)q->payload, q->len, (u8_t *)&buffer[l], q->len);
            l = l + q->len;
        }
    }
#ifdef SELECT_DESCRIPTORS_ENHANCED_MODE
    ENET_NOCOPY_PTPFRAME_RECEIVE_ENHANCED_MODE(NULL);
#else
    ENET_NOCOPY_FRAME_RECEIVE();
#endif

    return p;    
}

/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
void ethernetif_input(void *pParams)
{
    struct netif *netif;
    struct pbuf *p = NULL;
    netif = (struct netif *) pParams;
    UINT32 intSave1;
	
    for (;;) {
        if (LOS_SemPend(g_semId, 100) == LOS_OK) {
            continue;
        }
        while (1) {
            intSave1 = LOS_IntLock();
            p = low_level_input(netif);
            LOS_IntRestore(intSave1);

            if (p == NULL) {
                break;
            }
            if (ERR_OK != netif->input(p, netif)) {
                pbuf_free(p);
                break;
            }
        }
    }    
}

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t ethernetif_init(struct netif *netif)
{
    LWIP_ASSERT("netif != NULL", (netif != NULL));
  
    netif->name[0] = IFNAME0;
    netif->name[1] = IFNAME1;

    netif->output = etharp_output;
    netif->linkoutput = low_level_output;

    /* initialize the hardware */
    low_level_init(netif);

    return ERR_OK;
}
