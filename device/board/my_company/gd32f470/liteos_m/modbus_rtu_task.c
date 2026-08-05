/*
 * Copyright (c) 2026 zhangyao
 *
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
#include "los_task.h"
#include "gd32f4xx.h"
#include "utils.h"
#include "modbus_rtu_task.h"
#include "message_queue.h"


UINT32 rs485_1_queue_id = 0;
UINT32 rs485_2_queue_id = 0;

UINT32 modbus_rtu_queue_init()
{
	UINT32 ret = 0;
    ret = messageQueueNew("rs485_1_queue", MESSAGE_QUEUE_MAX_COUNT, &rs485_1_queue_id, 0, MESSAGE_QUEUE_MAX_SIZE);
    if(ret != LOS_OK) {
        printf("** messageQueueNew rs485_1_queue failed!\n");
        return ret;
    }

    ret = messageQueueNew("rs485_2_queue", MESSAGE_QUEUE_MAX_COUNT, &rs485_2_queue_id, 0, MESSAGE_QUEUE_MAX_SIZE);
    if(ret != LOS_OK) {
        printf("** messageQueueNew rs485_2_queue failed!\n");
        return ret;
    }
    return ret;
}

static void *thread_modbus_rtu_task_1(unsigned int arg)
{
	UINT8 rs485_com_1_queue_buf[MESSAGE_QUEUE_MAX_SIZE] = {0};
	UINT32 rs485_com_1_queue_size = 0;
	while(1) {
		rs485_com_1_queue_size = MESSAGE_QUEUE_MAX_SIZE;
		// UINT32 ret = messageQueueGet(rs485_1_queue_id, rs485_com_1_queue_buf, rs485_com_1_queue_size);
		// if(ret != LOS_OK) {
		// 	printf("** messageQueueGet rs485_1_queue_id = %d, ret = 0x%x\n",rs485_1_queue_id, ret);
		// }
		// process data
		// printf("rs485_com_1_queue_buf: %s\n", rs485_com_1_queue_buf);
		LOS_TaskDelay(1000);
	}
}

static void *thread_modbus_rtu_task_2(unsigned int arg)
{
	UINT8 rs485_com_2_queue_buf[MESSAGE_QUEUE_MAX_SIZE] = {0};
	UINT32 rs485_com_2_queue_size = 0;
	while(1) {
		// rs485_com_2_queue_size = MESSAGE_QUEUE_MAX_SIZE;
		// UINT32 ret = messageQueueGet(rs485_2_queue_id, rs485_com_2_queue_buf, rs485_com_2_queue_size);
		// if(ret != LOS_OK) {
		// 	printf("** messageQueueGet rs485_2_queue_id = %d, ret = 0x%x\n",rs485_2_queue_id, ret);
		// }
		// process data
		// printf("rs485_com_2_queue_buf: %s\n", rs485_com_2_queue_buf);
		LOS_TaskDelay(1000);
	}
}

UINT32 modbus_rtu_task_1_init(void)
{
	UINT32 taskID;
	TSK_INIT_PARAM_S stTask = {
		.pfnTaskEntry = thread_modbus_rtu_task_1,
		.uwStackSize = 0x1000,
		.pcName = "modbusRtuTask_1",
		.usTaskPrio = 6,
	};
	if (LOS_TaskCreate(&taskID, &stTask) != LOS_OK) {
        printf("** LOS_TaskCreate modbusRtuTask_1 failed!\n");
		return -1;
	}
	return taskID;
}

UINT32 modbus_rtu_task_2_init(void)
{
	UINT32 taskID;
	TSK_INIT_PARAM_S stTask = {
		.pfnTaskEntry = thread_modbus_rtu_task_2,
		.uwStackSize = 0x1000,
		.pcName = "modbusRtuTask_2",
		.usTaskPrio = 6,
	};
	if (LOS_TaskCreate(&taskID, &stTask) != LOS_OK) {
        printf("** LOS_TaskCreate modbusRtuTask_2 failed!\n");
		return -1;
	}
	return taskID;
}