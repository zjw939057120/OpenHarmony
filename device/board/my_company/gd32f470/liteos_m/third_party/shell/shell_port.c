/**
 * @file shell_port.c
 * @author Letter (NevermindZZT@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2019-02-22
 * 
 * @copyright (c) 2019 Letter
 * 
 */

#include <stdio.h>
#include "shell_port.h"
#include "los_sem.h"
#include "los_task.h"
#include "uart_drv.h"

static Shell shell;
static char shellBuffer[512];
static UINT32 g_semId;

/**
 * @brief 用户shell写
 * 
 * @param data 数据
 */
static void userShellWrite(char data)
{
	char msg[2] = {data, '\0'};
	uartPuts(msg);
}

/**
 * @brief 用户shell初始化
 * 
 */
static void userShellInit(void)
{
    shell.write = userShellWrite;
    //读函数使用中断实现
    shell.read = NULL;

    shellInit(&shell, shellBuffer, sizeof(shellBuffer));
}

void wakeupShellRecvTask(void)
{
	LOS_SemPost(g_semId);
}

static void *thread_shell_recv(unsigned int arg)
{
	while (1) {
		if (LOS_SemPend(g_semId, LOS_WAIT_FOREVER) == LOS_OK) {
            char c;
			if (uartGetc(&c) < 0) 
                continue;
			shellHandler(&shell, c);
		}
	}
	return NULL;
}

void initLetterShell(void)
{
	LOS_SemCreate(0, &g_semId);

	userShellInit();

	UINT32 shellTaskID;
	TSK_INIT_PARAM_S stTask = {
		.pfnTaskEntry = thread_shell_recv,
		.uwStackSize = 0x1000,
		.pcName = "shellTask",
		.usTaskPrio = 6,
	};
	if (LOS_TaskCreate(&shellTaskID, &stTask) != LOS_OK) {
		printf("** LOS_TaskCreate shellTask failed!\n");
	}
}
