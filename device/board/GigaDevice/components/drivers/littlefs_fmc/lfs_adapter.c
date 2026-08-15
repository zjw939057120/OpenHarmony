/*
 * Copyright (c) 2022 PandaBoard.
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
#include <los_interrupt.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <dirent.h>
#include <los_fs.h>
#include "fmc_operation.h"

struct littlefs_ctx {
    uint32_t startOffset; /* The partion address in chip; unit in byte */
    uint32_t len; /* The partion length, unit in byte */
    char *mountPoint;
};
struct littlefs_cfg {
    struct PartitionCfg cfg;
    struct littlefs_ctx ctx;
};

int lfs_read(int partition, UINT32 *offset, void *buf, UINT32 size);
int lfs_write(int partition, UINT32 *offset, const void *buf, UINT32 size);
int lfs_erase(int partition, UINT32 offset, UINT32 size);

#if (defined GD32F470VI)
struct littlefs_cfg g_lfs_cfg[] = {
    [0] = {
        .cfg = {
            /* partition low-level read func */
            .readFunc = lfs_read,
            /* partition low-level write func */
            .writeFunc = lfs_write,
            /* partition low-level erase func */
            .eraseFunc = lfs_erase,

            .readSize = 16,
            .writeSize = 16,
            .blockSize = 4096,
            .blockCount = 256,                          /* blockCount = ctx.len/blockSize */
            .cacheSize = 1024,

            .partNo = 0,
            .lookaheadSize = 16,
            .blockCycles = 1000,
        },
        .ctx = {
            .startOffset = ADDR_FMC_SECTOR_12,          /* littlefs开始位置 */
            .len = 0x100000,                            /* 1M */
            .mountPoint = "/data",
        }
    },
};
#else
struct littlefs_cfg g_lfs_cfg[] = {
    [0] = {
        .cfg = {
            /* partition low-level read func */
            .readFunc = lfs_read,
            /* partition low-level write func */
            .writeFunc = lfs_write,
            /* partition low-level erase func */
            .eraseFunc = lfs_erase,

            .readSize = 16,
            .writeSize = 16,
            .blockSize = 4096,
            .blockCount = 32,
            .cacheSize = 1024,

            .partNo = 0,
            .lookaheadSize = 16,
            .blockCycles = 1000,
        },
        .ctx = {
            .startOffset = ADDR_FMC_SECTOR_7,         /* littlefs开始位置, 以常见的512K大小为例 */
            .len = 128*1024,
            .mountPoint = "/data",
        }
    },
};
#endif

int lfs_read(int partition, UINT32 *offset, void *buf, UINT32 size)
{
    fmc_read((uint32_t)(*offset), size, (uint8_t *)buf);
    return 0;
}
int lfs_write(int partition, UINT32 *offset, const void *buf, UINT32 size)
{
    fmc_write((uint32_t)(*offset), size, (uint8_t *)buf);
    return 0;
}
int lfs_erase(int partition, UINT32 offset, UINT32 size)
{
    fmc_erase_page((uint32_t)offset, size);
    return 0;
}

void lfs_init(void)
{
    uint32_t num = sizeof(g_lfs_cfg) / sizeof(g_lfs_cfg[0]);
    int ret;

    INT32 lengthArray[num];
    INT32 addrArray[num];
    for (size_t i = 0; i < num; i++)
    {
        lengthArray[i] = g_lfs_cfg[i].ctx.len;
        addrArray[i] = g_lfs_cfg[i].ctx.startOffset;
    }

    ret = LOS_DiskPartition("spiflash", "littlefs", lengthArray, addrArray, num);
    if (ret != 0)
    {
        printf("LOS_DiskPartition fail, num:%ld\n", num);
    }
    
    for (int i = 0; i < num; i++) {
        ret = mount(NULL, g_lfs_cfg[i].ctx.mountPoint, "littlefs", 0, &g_lfs_cfg[i].cfg);
        if (ret < 0) {
            printf("[ERR] littlefs [%s] mount failed!!!\n", g_lfs_cfg[i].ctx.mountPoint);
            continue;
        }

        DIR *dir = NULL;
        if ((dir = opendir(g_lfs_cfg[i].ctx.mountPoint)) == NULL) {
            ret = mkdir(g_lfs_cfg[i].ctx.mountPoint, S_IRUSR | S_IWUSR);
            if (ret) {
                printf("[ERR] littlefs mkdir [%s] mount failed!!!\n", g_lfs_cfg[i].ctx.mountPoint);
                continue;
            }
        } else {
            closedir(dir);
        }
    }
}