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

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief 一个安全的内存拷贝函数，保证在任何地址下都不会触发对齐异常。
 * @param dest 目标地址
 * @param src  源地址
 * @param n    拷贝的字节数
 * @return 返回 dest
 * 
 * @note 对于 <= 32 字节的拷贝，使用逐字节循环，保证绝对安全。
 *       对于大块拷贝，先通过逐字节操作让源地址和目标地址都对齐到 4 字节边界，
 *       然后使用 32 位字（word）拷贝来提高效率。
 */
void *memcpy(void *dest, const void *src, size_t n)
{
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;
    size_t i;

	/* 1. 如果数据量很小，直接逐字节拷贝是最简单安全的 */
    if (n <= 32) {
        for (i = 0; i < n; i++) {
            d[i] = s[i];
        }
        return dest;
    }

    /* 2. 大块拷贝：先对齐到 4 字节边界 */
    /* 分别计算源和目标地址到下一个 4 字节边界的距离 */
    size_t align_dest = ((uintptr_t)d & 0x3);
    size_t align_src  = ((uintptr_t)s & 0x3);
    
    /* 只有当两者对齐状态相同时，字拷贝才是安全的，否则只能全程用字节拷贝 */
    if (align_dest == align_src) {
        /* 预先将未对齐的部分用字节拷贝处理，直到地址变为 4 字节对齐 */
        while (n > 0 && ((uintptr_t)d & 0x3)) {
            *d++ = *s++;
            n--;
        }
        
        /* 现在 d 已对齐到 4 字节，s 也与 d 有相同的对齐状态，可以进行字拷贝 */
        uint32_t *dw = (uint32_t *)d;
        const uint32_t *sw = (const uint32_t *)s;
        size_t words = n / 4;
        for (i = 0; i < words; i++) {
            dw[i] = sw[i];
        }
        
        /* 更新指针和剩余字节数 */
        d = (uint8_t *)(dw + words);
        s = (const uint8_t *)(sw + words);
        n -= words * 4;
    }
    
    /* 3. 处理剩余的不足 4 字节的部分 */
    for (i = 0; i < n; i++) {
        d[i] = s[i];
    }

    return dest;
}

