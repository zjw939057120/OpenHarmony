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


UINT32 queueID_1 = 0;
UINT32 queueID_2 = 0;

UINT32 modbus_rtu_queue_init()
{
	UINT32 ret = 0;
    ret = messageQueueNew("Q1", QUEUE_MAX_LEN, &queueID_1, 0, QUEUE_MAX_NODE_SIZE);
    if(ret != LOS_OK) {
        printf("** messageQueueNew Q1 failed!\n");
        return ret;
    }

    ret = messageQueueNew("Q2", QUEUE_MAX_LEN, &queueID_2, 0, QUEUE_MAX_NODE_SIZE);
    if(ret != LOS_OK) {
        printf("** messageQueueNew Q2 failed!\n");
        return ret;
    }
    return ret;
}

static void *thread_modbus_rtu_task_1(unsigned int arg)
{
	UINT8 buf[QUEUE_MAX_NODE_SIZE] = {0};
	UINT32 len = 0;
	while(1) {
		len = QUEUE_MAX_NODE_SIZE;
		UINT32 ret = messageQueueGet(queueID_1, buf, &len);
		buf[len] = '\0';
		if(ret != LOS_OK) {
			printf("** messageQueueGet queueID_1 = %d, ret = 0x%x\n",queueID_1, ret);
		}
		// process data
		// printf("buf: %s, len = %d\n", buf,len);
		rs485_1_send_bytes(buf, len);
	}
}

static void *thread_modbus_rtu_task_2(unsigned int arg)
{
	UINT8 buf[QUEUE_MAX_NODE_SIZE] = {"ABCD"};
	UINT32 len = 0;
	while(1) {
		len = QUEUE_MAX_NODE_SIZE;
		UINT32 ret = messageQueueGet(queueID_2, buf, &len);
		buf[len] = '\0';
		if(ret != LOS_OK) {
			printf("** messageQueueGet queueID_2 = %d, ret = 0x%x\n",queueID_2, ret);
		}
		// process data
		// printf("buf: %s, len = %d\n", buf,len);
		rs485_2_send_bytes(buf, len);
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