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

#include <fcntl.h>    // open
#include <unistd.h>   // close, read, write
#include <stdio.h>    // printf
#include <stdlib.h>   // exit
#include <string.h>   // strlen

#if (LOSCFG_FILE_SYSTEM_TEST == 1)

#define FILE_NAME   "/data/test_file.txt"

// 创建并写入文件
void crate_write_file() {
    printf("\n[FileSystem Test]: open and creat file:(%s)\n", FILE_NAME);
    int fd = open(FILE_NAME, O_WRONLY | O_CREAT | O_TRUNC);
    if (fd == -1) {
        printf("[FileSystem Test]: Failed to open file for writing\n");
        return;
    }

    const char *content = "Hello, OpenHarmony!";
    printf("[FileSystem Test]: write data:(%s) to file: (%s)\n", content, FILE_NAME);
    ssize_t bytes_written = write(fd, content, strlen(content));
    if (bytes_written == -1) {
        printf("[FileSystem Test]: Failed to write to file\n");
    } else {
        printf("[FileSystem Test]: Wrote %zd bytes to file.\n", bytes_written);
    }

    close(fd);
}

// 读取文件
void read_file() {
    char buffer[100];
    int fd = open(FILE_NAME, O_RDONLY);
    if (fd == -1) {
        printf("[FileSystem Test]: Failed to open file for reading\n");
        return;
    }

    ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes_read == -1) {
        printf("[FileSystem Test]: Failed to read from file\n");
    } else {
        buffer[bytes_read] = '\0'; // Null-terminate the string
        printf("[FileSystem Test]: Read file:(%s) data:(%s)\n", FILE_NAME, buffer);
        
    }

    close(fd);
}

// 删除文件
void delete_file() {
    if (remove(FILE_NAME) != 0) {
        printf("[FileSystem Test]: Failed to delete the file\n");
    }
    printf("[FileSystem Test]: remove file:(%s)\n\n", FILE_NAME);
}


void file_system_test(void)
{
    crate_write_file();
    LOS_TaskDelay(100);
    read_file();
    LOS_TaskDelay(100);
    delete_file();
}

#endif /* LOSCFG_FILE_SYSTEM_TEST */