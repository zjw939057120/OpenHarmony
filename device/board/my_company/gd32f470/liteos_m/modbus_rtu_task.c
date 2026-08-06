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
#include "modbus_rtu.h"


UINT32 queueID_1 = 0;
UINT32 queueID_2 = 0;

UINT32 rs485_recv_queue_init()
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

static void *thread_rs485_1_recv_task(unsigned int arg)
{
	UINT8 rx_buffer[QUEUE_MAX_NODE_SIZE] = {0};
	UINT32 rx_len = 0;
	while(1) {
		rx_len = QUEUE_MAX_NODE_SIZE;
		UINT32 ret = messageQueueGet(queueID_1, rx_buffer, &rx_len);
		rx_buffer[rx_len] = '\0';
		if(ret != LOS_OK) {
			// 没有数据可读，等待1秒后重试
			printf("** messageQueueGet queueID_1 = %d, ret = 0x%x\n",queueID_1, ret);
			LOS_TaskDelay(1000);
			continue;
		}
		// process data
		// printf("rx_buffer: %s, rx_len = %d\n", rx_buffer,rx_len);
	// 1. 接收到的原始数据
    // 2. 解析请求
	ModbusRequest req;
    if (!modbus_rtu_parse_request(rx_buffer, rx_len, &req)) {
        printf("❌ 接收到的帧解析失败或 CRC 错误!\n");
        continue;
    }

	uint8_t tx_buffer[64]; // 准备一个发送缓冲区
	uint16_t tx_len = 0;
    printf("✅ 收到请求: 从站 %d, 功能码 0x%02X, 起始地址 %d, 数量 %d\n", 
           req.slave_id, req.function_code, req.start_addr, req.quantity);

    // 3. 业务逻辑：根据请求准备寄存器数据
    // 假设我们读取 req.quantity 个寄存器，这里用假数据填充 (0x1234, 0x1235, ...)
    uint16_t holding_registers[req.quantity];
    for (int i = 0; i < req.quantity; i++) {
        holding_registers[i] = 0x1234 + i; 
    }

    // 4. 构造响应帧
    tx_len = modbus_rtu_build_read_response(req.slave_id, holding_registers, req.quantity, 
                                            tx_buffer, sizeof(tx_buffer));

    if (tx_len > 0) {
        printf("✅ 成功构造响应帧，总长度: %d 字节\n", tx_len);
        printf("📤 准备发送的原始数据: ");
        for (uint16_t i = 0; i < tx_len; i++) {
            printf("%02X ", tx_buffer[i]);
        }
        printf("\n");
        
        // 在实际项目中，这里会调用串口发送函数，例如：
		rs485_1_send_bytes(tx_buffer, tx_len);
    } else {
        printf("❌ 构造响应帧失败!\n");
    }

	}
}

static void *thread_rs485_2_recv_task(unsigned int arg)
{
    // 1. 【使用变量】定义你要读取的参数
    uint8_t target_slave_id = 0x01;      // 目标从站地址
    uint16_t target_start_addr = 0x0000; // 起始寄存器地址
    uint16_t target_quantity = 0x000A;   // 读取 10 个寄存器
	
	UINT8 rx_buffer[QUEUE_MAX_NODE_SIZE] = {0};
	UINT32 rx_len = 0;
	while(1) {
		rx_len = QUEUE_MAX_NODE_SIZE;
		UINT32 ret = messageQueueGet(queueID_2, rx_buffer, &rx_len);
		rx_buffer[rx_len] = '\0';
		if(ret != LOS_OK) {
			// 没有数据可读，等待1秒后重试
			printf("** messageQueueGet queueID_2 = %d, ret = 0x%x\n",queueID_2, ret);
			LOS_TaskDelay(1000);
			continue;
		}
		// printf("rx_buffer: %s, rx_len = %d\n", rx_buffer,rx_len);
    printf("📥 [主站接收] 收到 %d 字节数据\n", rx_len);

    if (rx_len > 4 && rx_buffer[0] == target_slave_id && rx_buffer[1] == 0x03) {
        uint16_t calc_crc = modbus_crc16(rx_buffer, rx_len - 2);
        uint16_t recv_crc = (uint16_t)rx_buffer[rx_len - 1] << 8 | rx_buffer[rx_len - 2];
        
        if (calc_crc == recv_crc) {
            uint8_t byte_count = rx_buffer[2];
            printf("✅ 解析成功! 包含 %d 字节数据\n", byte_count);
            
            uint16_t reg_count = byte_count / 2;
            printf("📊 寄存器数据: ");
            for (uint16_t i = 0; i < reg_count; i++) {
                uint16_t val = (uint16_t)rx_buffer[3 + i*2] << 8 | rx_buffer[3 + i*2 + 1];
                printf("0x%04X ", val);
            }
            printf("\n");
        } else {
            printf("❌ 响应帧 CRC 校验失败!\n");
        }
    }
	}
}

static void *thread_rs485_2_send_task(unsigned int arg)
{
    // 1. 【使用变量】定义你要读取的参数
    uint8_t target_slave_id = 0x01;      // 目标从站地址
    uint16_t target_start_addr = 0x0000; // 起始寄存器地址
    uint16_t target_quantity = 0x000A;   // 读取 10 个寄存器

    // 2. 准备发送缓冲区
    uint8_t tx_buffer[32];
    uint16_t tx_len = 0;
	while(1) {
    // 3. 【主站动作】使用变量动态构造请求帧
    tx_len = modbus_rtu_build_read_request(target_slave_id, target_start_addr, target_quantity, 
                                           tx_buffer, sizeof(tx_buffer));

    if (tx_len > 0) {
        // 4. 通过串口发送出去
		rs485_2_send_bytes(tx_buffer, tx_len);
    } else {
        printf("❌ 构造请求帧失败!\n");
    }

		// 5. 等待3秒后发送下一次请求
		LOS_TaskDelay(3000);
	}
}

UINT32 rs485_1_recv_task_init(void)
{
	UINT32 taskID;
	TSK_INIT_PARAM_S stTask = {
		.pfnTaskEntry = thread_rs485_1_recv_task,
		.uwStackSize = 0x1000,
		.pcName = "rs485_1_recv_task",
		.usTaskPrio = 6,
	};
	if (LOS_TaskCreate(&taskID, &stTask) != LOS_OK) {
        printf("** LOS_TaskCreate rs485_1_recv_task failed!\n");
		return -1;
	}
	return taskID;
}

UINT32 rs485_2_recv_task_init(void)
{
	UINT32 taskID;
	TSK_INIT_PARAM_S stTask = {
		.pfnTaskEntry = thread_rs485_2_recv_task,
		.uwStackSize = 0x1000,
		.pcName = "rs485_2_recv_task",
		.usTaskPrio = 6,
	};
	if (LOS_TaskCreate(&taskID, &stTask) != LOS_OK) {
        printf("** LOS_TaskCreate rs485_2_recv_task failed!\n");
		return -1;
	}
	return taskID;
}

UINT32 rs485_2_send_task_init(void)
{
	UINT32 taskID;
	TSK_INIT_PARAM_S stTask = {
		.pfnTaskEntry = thread_rs485_2_send_task,
		.uwStackSize = 0x1000,
		.pcName = "rs485_2_send_task",
		.usTaskPrio = 6,
	};
	if (LOS_TaskCreate(&taskID, &stTask) != LOS_OK) {
        printf("** LOS_TaskCreate rs485_2_send_task failed!\n");
		return -1;
	}
	return taskID;
}