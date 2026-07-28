#ifndef _MAIN_TASK_H_
#define _MAIN_TASK_H_

#include <stdio.h>
#include <stdbool.h>
#include "systick.h"
#include "los_task.h"
#include "gd32f4xx.h"
#include "uart_drv.h"
#include "shell_port.h"
#include "rtc_drv.h"
#include "wdg_drv.h"


UINT32 main_task_init();

#endif
