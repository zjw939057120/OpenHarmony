#ifndef _MODBUS_RTU_TASK_H_
#define _MODBUS_RTU_TASK_H_

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "gd32f4xx.h"

extern UINT32 queueID_1;
extern UINT32 queueID_2;

UINT32 rs485_recv_queue_init();

UINT32 rs485_1_recv_task_init();
UINT32 rs485_2_recv_task_init();

#endif
