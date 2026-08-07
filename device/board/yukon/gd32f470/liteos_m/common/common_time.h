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

#ifndef _COMMON_TIME_H_
#define _COMMON_TIME_H_
#include <stdint.h>
#include <stdbool.h>

extern bool is_leap_year(uint32_t year);
extern uint8_t get_days_in_month(uint32_t year, uint8_t month);
extern uint32_t calendar_to_timestamp(uint32_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
extern void timestamp_to_calendar(uint32_t timestamp, uint32_t *year, uint8_t *month, uint8_t *day, uint8_t *hour, uint8_t *minute, uint8_t *second);

extern void get_curtime_r(char *timestr, int len);
extern void set_curtime_by_ts(uint32_t ts);
extern int set_curtime(const char *timestr);

#endif
