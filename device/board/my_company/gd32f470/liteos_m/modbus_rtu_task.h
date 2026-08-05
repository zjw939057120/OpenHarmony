#ifndef _MODBUS_RTU_TASK_H_
#define _MODBUS_RTU_TASK_H_

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "gd32f4xx.h"

extern UINT32 rs485_1_queue_id;
extern UINT32 rs485_2_queue_id;

UINT32 modbus_rtu_queue_init();

UINT32 modbus_rtu_task_1_init();
UINT32 modbus_rtu_task_2_init();

#endif
