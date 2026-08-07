#include <stdio.h>
#include <string.h>
#include "los_arch_interrupt.h"
#include "los_interrupt.h"
#include "los_sem.h"
#include "los_task.h"
#include "los_event.h"
#include "gd32f4xx.h"
#include "utils.h"
#include <stdint.h>

// GD32F470 唯一设备ID的基地址
#define GD32_UNIQUE_ID_BASE 0x1FFF7A10

/**
 * @brief 基于GD32芯片唯一ID生成MAC地址
 * @param mac_addr 用于存储生成的48位MAC地址的数组指针
 */
void generate_mac_from_uid(uint8_t *mac_addr)
{
    // 1. 获取指向96位(12字节)唯一ID的指针
    uint8_t *uid_ptr = (uint8_t *)GD32_UNIQUE_ID_BASE;
    
    // 2. 异或折叠法：将12字节的ID分为前6字节和后6字节，进行按位异或运算
    for (uint8_t i = 0; i < 6; i++) {
        mac_addr[i] = uid_ptr[i] ^ uid_ptr[i + 6];
    }
    
    // 3. 设置本地管理地址标志位（关键步骤）
    // 对数组的第一个字节 (mac_addr[0]) 进行位运算，而不是对指针本身
    mac_addr[0] |= 0x02;  // 将第1个字节的次低位置1，表明这是本地生成的地址
    mac_addr[0] &= 0xFE;  // 确保第一个字节的最低位为0，表明这是一个单播地址
}

