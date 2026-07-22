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
#include <time.h>
#include <stdint.h>
#include <stdbool.h>
#include "sys/time.h"
#include "log_printf.h"

// 月份天数表（非闰年）
static const uint8_t days_in_month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

// 闰年判断
bool is_leap_year(uint32_t year)
{
	if (year % 400 == 0)
		return true;
	if (year % 100 == 0)
		return false;
	if (year % 4 == 0)
		return true;
	return false;
}

// 获取某年某月的天数
uint8_t get_days_in_month(uint32_t year, uint8_t month)
{
	if (month == 2 && is_leap_year(year)) {
		return 29;
	}
	return days_in_month[month - 1];
}

// 核心函数：日历时间 -> POSIX时间戳
uint32_t calendar_to_timestamp(uint32_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
{
	uint32_t total_seconds = 0;

	// 1. 计算从1970年到 year-1 年的总秒数（每年按秒计）
	uint32_t y;
	for (y = 1970; y < year; y++) {
		total_seconds += is_leap_year(y) ? 366 * 86400 : 365 * 86400;
	}

	// 2. 加上当年已过月份的天数（按天计）
	uint8_t m;
	for (m = 1; m < month; m++) {
		total_seconds += get_days_in_month(year, m) * 86400;
	}

	// 3. 加上当月已过天数（按天计）
	total_seconds += (day - 1) * 86400;

	// 4. 加上当天已过的小时、分钟、秒
	total_seconds += hour * 3600;
	total_seconds += minute * 60;
	total_seconds += second;

	return total_seconds;
}

// 核心函数：POSIX时间戳 -> 日历时间
void timestamp_to_calendar(uint32_t timestamp, uint32_t *year, uint8_t *month, uint8_t *day, uint8_t *hour, uint8_t *minute, uint8_t *second)
{
	// 1. 提取 时分秒
	*second = timestamp % 60;
	timestamp /= 60;
	*minute = timestamp % 60;
	timestamp /= 60;
	*hour = timestamp % 24;
	uint32_t days = timestamp / 24; // 总天数

	// 2. 推算年份
	*year = 1970;
	while (days >= (is_leap_year(*year) ? 366 : 365)) {
		days -= is_leap_year(*year) ? 366 : 365;
		(*year)++;
	}

	// 3. 推算月份
	*month = 1;
	while (days >= get_days_in_month(*year, *month)) {
		days -= get_days_in_month(*year, *month);
		(*month)++;
	}

	// 4. 推算日期（剩余天数+1）
	*day = days + 1;
}


/**
 * 获取当前格式化时间yyyy-MM-dd hh:mm:ss
 */
void get_curtime_r(char *timestr, int len)
{
    time_t tv_time = 0;
    struct tm tv_tm = {0};

    tv_time = time(NULL);
    localtime_r(&tv_time, &tv_tm);
       
    strftime(timestr, len-1, "%F %T", &tv_tm);
}

void set_curtime_by_ts(uint32_t ts)
{
	struct timeval nowTime = {
		.tv_sec = ts,
		.tv_usec = 0,
	};
	if (settimeofday(&nowTime, NULL)) {
		printf("** settimeofday failed\n");
		return;
	}
}

/* 设置当前时间: yyyy-mm-dd hh:mm:ss */
int set_curtime(const char *timestr)
{
	struct tm tm_time = {};
	time_t timestamp;

	if (strptime(timestr, "%Y-%m-%d %H:%M:%S", &tm_time) == NULL) {
		printf("** timestr format error\n");
		return -1;
	}
	timestamp = mktime(&tm_time);

	if (timestamp == -1) {
		printf("** mktime failed\n");
		return -1;
	}
	set_curtime_by_ts(timestamp);

    return 0;
}
