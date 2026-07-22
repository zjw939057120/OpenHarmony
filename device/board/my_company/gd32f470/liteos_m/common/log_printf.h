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

#ifndef _LOG_PRINTF_H_
#define _LOG_PRINTF_H_
#include <string.h>
#include <time.h>
#include "common_time.h"

/* log level */
enum {
	DPRINTF_LOG_EMERG,
	DPRINTF_LOG_ALERT,
	DPRINTF_LOG_CRIT,
	DPRINTF_LOG_ERR,
	DPRINTF_LOG_WARNING,
	DPRINTF_LOG_NOTICE,
	DPRINTF_LOG_INFO,
	DPRINTF_LOG_DEBUG,
};
#define DPRINTF_LOG_LEVEL_DEFAULT DPRINTF_LOG_INFO

#define _COLOR_NONE "\033[0m"
#define _COLOR_RED "\033[31m"
#define _COLOR_GREEN "\033[32m"
#define _COLOR_YELLOW "\033[33m"
#define _COLOR_BLUE "\033[34m"
#define _COLOR_PURPLE "\033[35m"
#define _COLOR_LIGHT_RED "\033[1;31m"
#define _COLOR_LIGHT_GREEN "\033[1;32m"
#define _COLOR_LIGHT_YELLOW "\033[1;33m"
#define _COLOR_LIGHT_BLUE "\033[1;34m"
#define _COLOR_LIGHT_PURPLE "\033[1;35m"

#define __FILENAME__ (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1) : __FILE__)

#define log_printf(level, fmt, ...)                             \
	do {                                                        \
		if (level > DPRINTF_LOG_LEVEL_DEFAULT)                  \
			break;                                              \
		char timestr[30] = {};                                  \
		get_curtime_r(timestr, sizeof(timestr));                \
		printf(_COLOR_NONE "[%s][%s:%d]" _COLOR_NONE            \
						   " " fmt "\n",                        \
			   timestr, __FILENAME__, __LINE__, ##__VA_ARGS__); \
	} while (0)

#define log_err(fmt, ...)                                                             \
	do {                                                                              \
		log_printf(DPRINTF_LOG_ERR, _COLOR_LIGHT_RED fmt _COLOR_NONE, ##__VA_ARGS__); \
	} while (0)

#define log_warning(fmt, ...)                                                                \
	do {                                                                                     \
		log_printf(DPRINTF_LOG_WARNING, _COLOR_LIGHT_YELLOW fmt _COLOR_NONE, ##__VA_ARGS__); \
	} while (0)

#define log_notice(fmt, ...)                                                              \
	do {                                                                                  \
		log_printf(DPRINTF_LOG_NOTICE, _COLOR_LIGHT_BLUE fmt _COLOR_NONE, ##__VA_ARGS__); \
	} while (0)

#define log_info(fmt, ...)                                \
	do {                                                  \
		log_printf(DPRINTF_LOG_INFO, fmt, ##__VA_ARGS__); \
	} while (0)

#define log_debug(fmt, ...)                                \
	do {                                                   \
		log_printf(DPRINTF_LOG_DEBUG, fmt, ##__VA_ARGS__); \
	} while (0)

#define DEBUG(...) log_err("------ DEBUG> "__VA_ARGS__)

#define DEBUG_AND_RETURN(ret) \
	do {                      \
		DEBUG("return");      \
		return ret;           \
	} while (0)

#define DEBUG_AND_RETURN_NONE() \
	do {                        \
		DEBUG("return");        \
		return;                 \
	} while (0)

#endif
