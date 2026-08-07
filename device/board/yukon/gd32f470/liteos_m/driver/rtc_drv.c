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
#include "gd32f4xx.h"
#include "los_interrupt.h"
#include "rtc_time_hook.h"
#include "common_time.h"
#include "log_printf.h"
#include "shell_port.h"

/* 使用外部晶振作为RTC时钟源 */
#define CONFIG_RTC_CLOCK_SOURCE_LXTAL		1

#if IS_ENABLED(CONFIG_RTC_CLOCK_SOURCE_LXTAL)
#define prescaler_s 0xFF
#define prescaler_a 0x7F
#else
#define prescaler_s 0x13F
#define prescaler_a 0x63
#endif

/* 北京时间, TZ时域 CST-8 */
#define TZ_VAL 	(-8 * 60 * 60) 

// 定义备份寄存器用途
#define RTC_BKP_MAGIC 	RTC_BKP0 // 存储魔数
#define RTC_BKP_CENTURY RTC_BKP1 // 存储世纪
#define BKP_MAGIC_VALUE 0xA5A5A5A5

#define BCD2DEC(val) 	(((val) >> 4) * 10 + ((val)&0x0F))
#define DEC2BCD(val) 	(((val) / 10) << 4 | ((val) % 10))


static int rtc_check_magic(void)
{
	return (RTC_BKP_MAGIC == BKP_MAGIC_VALUE);
}

static uint32_t rtc_get_century(void)
{
	uint32_t century = 0;

	unsigned int intSave = LOS_IntLock();
	if (rtc_check_magic())
		century = (RTC_BKP_CENTURY >= 19) ? RTC_BKP_CENTURY : 0;
	LOS_IntRestore(intSave);

	return century;
}

static void rtc_save_century(uint32_t century)
{
	if (century < 19) {
		log_err("** rtc_save_century: invalid century value(%d)", (int)century);
		return;
	}

	unsigned int intSave = LOS_IntLock();
	RTC_BKP_CENTURY = century;
	RTC_BKP_MAGIC = BKP_MAGIC_VALUE;
	LOS_IntRestore(intSave);
}

/* 计算星期几，返回 1-7 对应 周一至周日 */
static uint8_t get_weekday(uint32_t year, uint8_t month, uint8_t day)
{
	uint32_t y = year;
	uint8_t m = month;

	if (m == 1 || m == 2) {
		y--;
		m += 12;
	}

	/* 基姆拉尔森公式
	 * 公式结果: 0=周日, 1=周一, ..., 6=周六
	 * 需要映射到 GD32 RTC: 1=周一, ..., 7=周日
	 */
	uint32_t weekday = (day + 2 * m + 3 * (m + 1) / 5 + y + y / 4 - y / 100 + y / 400 + 1) % 7;
	if (weekday == 0) {
		return 7;
	} else {
		return (uint8_t)weekday;
	}
}

static int _rtc_time_set(uint32_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t minute, uint8_t second)
{
	rtc_parameter_struct rtc_time = {};

	rtc_time.year = DEC2BCD(year % 100); // 年份只取后两位
	rtc_time.month = DEC2BCD(month);
	rtc_time.date = DEC2BCD(date);
	rtc_time.day_of_week = get_weekday(year, month, date);
	rtc_time.hour = DEC2BCD(hour);
	rtc_time.minute = DEC2BCD(minute);
	rtc_time.second = DEC2BCD(second);
	rtc_time.factor_asyn = prescaler_a;
	rtc_time.factor_syn = prescaler_s;
	rtc_time.am_pm = RTC_AM;
	rtc_time.display_format = RTC_24HOUR;

	if (rtc_init(&rtc_time) == ERROR) {
		log_err("** RTC time set failed!");
		return -1;
	}
	rtc_save_century(year/100);

	return 0;
}

static INT32 RtcGetTimeHook(UINT64 *usec)
{
	if (!usec)
		return -1;

	rtc_parameter_struct rtc_time = {};
	rtc_current_time_get(&rtc_time);

	uint32_t century = rtc_get_century();
	if (century == 0)
		return -1;

	uint32_t year = BCD2DEC(rtc_time.year);
	if (century == 19 && year < 70)
		return -1;
	year += (century * 100);

	uint8_t month = BCD2DEC(rtc_time.month);
	uint8_t day = BCD2DEC(rtc_time.date);
	uint8_t hour = BCD2DEC(rtc_time.hour);
	uint8_t minute = BCD2DEC(rtc_time.minute);
	uint8_t second = BCD2DEC(rtc_time.second);

	uint32_t ts_sec = calendar_to_timestamp(year, month, day, hour, minute, second);
	*usec = (UINT64)ts_sec * 1000000ULL;

	return 0;
}

static INT32 RtcSetTimeHook(UINT64 msec, UINT64 *usec)
{
	if (!usec)
		return -1;

	uint32_t year;
	uint8_t month, date, hour, minute, second;
	uint32_t timestamp_sec;

	timestamp_sec = (uint32_t)(msec / 1000);
	timestamp_to_calendar(timestamp_sec, &year, &month, &date, &hour, &minute, &second);

	if (_rtc_time_set(year, month, date, hour, minute, second) < 0)
		return -1;
	return 0;
}

static INT32 RtcGetTimezoneHook(INT32 *tz)
{
	*tz = TZ_VAL;
	return 0;
}

static struct RtcTimeHook rtchook = {
	.RtcGetTickHook = NULL,
	.RtcGetTimeHook = RtcGetTimeHook,
	.RtcSetTimeHook = RtcSetTimeHook,
	.RtcGetTimezoneHook = RtcGetTimezoneHook,
	.RtcSetTimezoneHook = NULL,
};

static void initRtcHardware(void)
{
	rcu_periph_clock_enable(RCU_PMU);
	pmu_backup_write_enable();
#if IS_ENABLED(CONFIG_RTC_CLOCK_SOURCE_LXTAL)
	rcu_osci_on(RCU_LXTAL);
	rcu_osci_stab_wait(RCU_LXTAL);
	rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);
#else
	rcu_osci_on(RCU_IRC32K);
	rcu_osci_stab_wait(RCU_IRC32K);
	rcu_rtc_clock_config(RCU_RTCSRC_IRC32K);
#endif
	rcu_periph_clock_enable(RCU_RTC);
	rtc_register_sync_wait();

	uint32_t RTCSRC_FLAG = GET_BITS(RCU_BDCTL, 8, 9);
	/* check if RTC has aready been configured */
	if (!rtc_check_magic() || (0x00 == RTCSRC_FLAG)) {
		printf("* RTC not configured yet or lost, initialize time to default(0)\n");
		UINT64 usec = 0;
		RtcSetTimeHook(0, &usec);
	}

	rcu_all_reset_flag_clear();
}

void initRtc(void)
{
	initRtcHardware();
	LOS_RtcHookRegister(&rtchook);
}
