#ifndef _MESSAGE_QUEUE_H_
#define _MESSAGE_QUEUE_H_

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "gd32f4xx.h"


// 串口接收队列节点大小
#define QUEUE_MAX_NODE_SIZE 128
// 串口接收队列最大长度
#define QUEUE_MAX_LEN 12

UINT32 messageQueueNew(CHAR *queueName,UINT16 len,UINT32 *queueID,UINT32 flags,UINT16 maxMsgSize);

UINT32 messageQueuePut(UINT32 queueID,VOID *bufferAddr,UINT32 bufferSize);

UINT32 messageQueueGet(UINT32 queueID,VOID *bufferAddr,UINT32 *bufferSize);

#endif
