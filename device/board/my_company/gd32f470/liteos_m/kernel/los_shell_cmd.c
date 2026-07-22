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
#include "los_interrupt.h"
#include "shell_port.h"
#include "los_task.h"
#include "sys/time.h"
#include "shcmd.h"
#include "log_printf.h"

static void los_task_display(void)
{
	OsGetAllTskInfo();
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN, task, los_task_display, (void));

static void los_date_display(void)
{
	struct timeval nowTime;

	if (gettimeofday(&nowTime, NULL)) {
		printf("** gettimeofday failed\n");
		return;
	}
	printf("%s\n", ctime(&nowTime.tv_sec));
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN, date, los_date_display, (void));

static void los_date_set(const char *time_str)
{
	set_curtime(time_str);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN, date_set, los_date_set, (yyyy-mm-dd hh:mm:ss));

static void los_free_display(void)
{
	OsShellCmdFree(0, NULL);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) | SHELL_CMD_DISABLE_RETURN, free, los_free_display, (void));

static void los_ifconfig(int argc, const char *argv[])
{
#if 0
	extern UINT32 lwip_ifconfig(INT32 argc, const CHAR **argv);
	lwip_ifconfig(argc - 1, &argv[1]);
#endif
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) | SHELL_CMD_DISABLE_RETURN, ifconfig, los_ifconfig, (void));

static void los_ping(int argc, const char *argv[])
{
#if 0
	extern UINT32 OsShellPing(INT32 argc, const CHAR **argv);
	OsShellPing(argc - 1, &argv[1]);
#endif
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) | SHELL_CMD_DISABLE_RETURN, ping, los_ping, (ip));

static void los_reboot(void)
{
	LOS_TaskLock();
	LOS_IntLock();
	NVIC_SystemReset();
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) | SHELL_CMD_DISABLE_RETURN, reboot, los_reboot, (void));
