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


#ifndef _GLOBAL_COMMON_H_
#define _GLOBAL_COMMON_H_

#define __ARG_PLACEHOLDER_1		0,
#define __take_second_arg(__ignored, val, ...)	val

#define __is_defined(x)		___is_defined(x)
#define ___is_defined(x)	____is_defined(__ARG_PLACEHOLDER_##x)
#define ____is_defined(x)	__take_second_arg(x 1, 0)	

/* =1时返回真, 未定义或=其他值时返回假 */
#define IS_ENABLED(x)	__is_defined(x)

#endif
