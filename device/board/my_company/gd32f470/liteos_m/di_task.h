#ifndef _DI_TASK_H_
#define _DI_TASK_H_

#include <stdio.h>
#include <stdbool.h>
#include "systick.h"
#include "los_task.h"
#include "gd32f4xx.h"
#include "uart_drv.h"
#include "shell_port.h"
#include "rtc_drv.h"
#include "wdg_drv.h"


UINT32 di_task_init();

#endif
