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
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "los_interrupt.h"
#include "uart_drv.h"

static char g_buf[1024];

static void dputs(const char *s)
{
    uartPuts(s);
}

#ifdef LOSCFG_LIBC_NEWLIB
int __wrap_printf(char const *fmt, ...)
#else
int printf(const char *fmt, ...)
#endif
{
	int len = 0;
	int truncated = 0;
	va_list ap;

	va_start(ap, fmt);

	/* 检查是否超出缓冲区大小 */
	va_list copy;
	va_copy(copy, ap);
	len = vsnprintf(NULL, 0, fmt, copy);
	va_end(copy);
	if (len >= sizeof(g_buf))
		truncated = 1;

	/* 输出字符串 */
	unsigned int intSave = LOS_IntLock();
	len = vsnprintf(g_buf, sizeof(g_buf), fmt, ap);
	if (len > 0) {
		dputs(g_buf);
		if (truncated)
			dputs("* [printf: output truncated, data too long]\n");
	} else {
		dputs("[printf: vsnprintf error]\n");
	}
	LOS_IntRestore(intSave);

	va_end(ap);

	if (len > 0) {
		if (truncated)
			return (sizeof(g_buf) - 1);
		return len;
	} else {
		return -1;
	}
}
