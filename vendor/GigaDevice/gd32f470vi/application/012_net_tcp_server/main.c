#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <ohos_init.h>
#include "los_task.h"
#include "ethernet_init.h"

#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h> 

#define TASK_STACK_SIZE     4096
#define TASK_PRIORITY       6                   /* 值越大，优先级越高 */

#define TCP_SERVER_IP       ("0.0.0.0") 
#define TCP_SERVER_PORT     (4567)              /* 服务器远端PORT */
#define DISCONNET_DELAY     (3000)              /* 监听失败后等待的时间 */
#define TCP_RECV_LENGHT     (256)               /* TCP通信接收的数据长度 */
#define MAX_CLIENT           2                  /* 连接的客户端最大数量 */

static int g_sockfd = -1;
static bool g_status = false;

VOID tcp_client_handle(int client_sockfd) 
{
    uint8_t rx_buffer[TCP_RECV_LENGHT];

    while (1) {
        ssize_t received_bytes = recv(client_sockfd, rx_buffer, TCP_RECV_LENGHT - 1, 0);
        if (received_bytes <= 0) {
            printf("Client disconnected or error occurred.\n");
            break;
        }

        rx_buffer[received_bytes] = '\0'; // null-terminate the received data
        printf("Received: %s\n", rx_buffer);

        // Echo the received message back to the client
        send(client_sockfd, rx_buffer, received_bytes, 0);
    }

    close(client_sockfd); // Close the client socket when done

    /* 删除自身任务 */
    UINT32 task_id = LOS_CurTaskIDGet();
    LOS_TaskDelete(task_id);
}

VOID tcp_server(VOID)
{
    int ret;
    while (1)
    {
        if (g_sockfd < 0)
        {
            /* 1. 创建套接字 */
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
            /* 2. 绑定本机地址和端口 */
            struct sockaddr_in dest_addr;
            dest_addr.sin_family = AF_INET;
            dest_addr.sin_addr.s_addr = inet_addr(TCP_SERVER_IP);
            dest_addr.sin_port = htons(TCP_SERVER_PORT);
            if (bind(g_sockfd, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) != 0){
                printf("Socket bind fail.\n");
                // 连接失败后，关闭之前新建的socket，等待下次新建
                close(g_sockfd);
                g_sockfd = -1;
            } else {
                g_status = true;
            }
        }

        /* 3. 设置监听套接字 */
        ret = listen(g_sockfd, 1);
        if(ret < 0){
            g_status = false;
            LOS_TaskDelay(DISCONNET_DELAY);
        }else{
            break;
        }
    }

    while (1)
    {
        /* 4. 接收客户端的连接，并生成通信套接字 */
        int client_sockfd = accept(g_sockfd, NULL, NULL);
        if (client_sockfd <= 0) {
            printf("accept ...\n");
            continue;
        }

        /* 为每个客户端创建一个新的任务 */
        UINT32 g_task_id;
        TSK_INIT_PARAM_S g_task = {0};
        g_task.pfnTaskEntry = (TSK_ENTRY_FUNC)tcp_client_handle;
        g_task.uwStackSize = TASK_STACK_SIZE;
        g_task.pcName = "client_handler";
        g_task.usTaskPrio = TASK_PRIORITY;
        g_task.uwArg = (UINT32)client_sockfd;

        ret = LOS_TaskCreate(&g_task_id, &g_task);
        if (ret != LOS_OK) {
            printf("Create client handler task failed!\n");
            close(client_sockfd); // 如果任务创建失败，关闭客户端套接字
        }
    }
}

VOID tcp_server_handler(VOID)
{
    UINT32 ret;
    UINT32 g_task_id;
    TSK_INIT_PARAM_S g_task = {0};

    /* 等待网络正常后进行 TCP 通信 */
    while (get_network_link() != ETH_LINK_UP)
    {
        LOS_TaskDelay(1000);
    }

    g_task.pfnTaskEntry = (TSK_ENTRY_FUNC)tcp_server;
    g_task.uwStackSize = TASK_STACK_SIZE;
    g_task.pcName = "tcp_server";
    g_task.usTaskPrio = TASK_PRIORITY;
    ret = LOS_TaskCreate(&g_task_id, &g_task);
    if (ret != LOS_OK) {
        printf("Create tcp_server failed!\n");
    }	
}

APP_FEATURE_INIT(tcp_server_handler);
