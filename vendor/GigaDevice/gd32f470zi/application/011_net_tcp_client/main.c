#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <ohos_init.h>
#include "los_task.h"
#include "ethernet_init.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 

#define TASK_STACK_SIZE     4096
#define TASK_PRIORITY       6                   /* 值越大，优先级越高 */

#define TCP_SERVER_IP       "192.168.0.128"     /* 服务器远端IP */
#define TCP_SERVER_PORT     (1234)              /* 服务器远端PORT */
#define REST_CONNECT_DELAY  (3000)              /* 连接失败后等待的时间 */
#define TCP_RECV_LENGHT     (256)               /* TCP通信接收的数据长度 */

static int g_sockfd = -1;
static bool g_status = false;

/* TCP 连接服务器处理 */
int tcp_connect_handler(void)
{
    /* 配置服务器 */
    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr(TCP_SERVER_IP);
    dest_addr.sin_port = htons(TCP_SERVER_PORT);

    /* 连接服务器 */
    if (connect(g_sockfd, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) != 0)
    {
        printf("Socket unable to connect.\n");
        // 连接失败后，关闭之前新建的socket，等待下次新建
        close(g_sockfd);
        g_sockfd = -1;
        return false;
    }
    
    printf("Socket connect success.\n");
    return true;
}

/* TCP 接收接收数据处理 */
void tcp_recv_handler(void)
{
    int ret = 0;
    uint8_t rx_buffer[TCP_RECV_LENGHT] = {0};

    memset(rx_buffer, 0, TCP_RECV_LENGHT);
    ret = recv(g_sockfd, rx_buffer, TCP_RECV_LENGHT - 1, 0);
    if (ret < 0)
    {
        printf("Error occured during receiving.\n");
        g_status = false;
        g_sockfd = -1;
        return;
    }

    if (ret == 0) 
    { 
        printf("Closed connection during receiving.\n\n");
        return;
    }

    rx_buffer[ret] = '\0';
    printf("Received data %d bytes.\n%s\n\n", ret, rx_buffer);

    send(g_sockfd, rx_buffer, ret, 0);

    LOS_TaskDelay(100);
}

/* TCP 客户端 */
VOID tcp_client(VOID)
{
    while (1)
    {
        /* 新建socket */
        if (g_sockfd < 0)
        {
            g_sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP); // addr_family  SOCK_STREAM  ip_protocol
            if (g_sockfd < 0)
            {
                printf("Unable to create socket.\n");
                // 新建失败后，关闭新建的socket，等待下次新建
                shutdown(g_sockfd, 0);
                close(g_sockfd);
                g_sockfd = -1;
            }
        }
        
        if (!g_status)
        {
            g_status = tcp_connect_handler();
            LOS_TaskDelay(REST_CONNECT_DELAY);
        } else {
            tcp_recv_handler();
        }
    }
}

VOID tcp_client_handler(VOID)
{
    UINT32 ret;
    UINT32 g_task_id;
    TSK_INIT_PARAM_S g_task = {0};

    /* 等待网络正常后进行 TCP 通信 */
    while (get_network_link() != ETH_LINK_UP)
    {
        LOS_TaskDelay(1000);
    }

    g_task.pfnTaskEntry = (TSK_ENTRY_FUNC)tcp_client;
    g_task.uwStackSize = TASK_STACK_SIZE;
    g_task.pcName = "tcp_client";
    g_task.usTaskPrio = TASK_PRIORITY;
    ret = LOS_TaskCreate(&g_task_id, &g_task);
    if (ret != LOS_OK) {
        printf("Create tcp_client failed!\n");
    }	
}

APP_FEATURE_INIT(tcp_client_handler);
