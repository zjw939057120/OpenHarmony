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
#include "los_task.h"
#include "gd32f4xx.h"
#include "utils.h"
#include "message_queue.h"


UINT32 messageQueueNew(CHAR *queueName,UINT16 len,UINT32 *queueID,UINT32 flags,UINT16 maxMsgSize){
	return LOS_QueueCreate(queueName,len,queueID,flags,maxMsgSize);
}

UINT32 messageQueuePut(UINT32 queueID,VOID *bufferAddr,UINT32 *bufferSize){
	return LOS_QueueWriteCopy(queueID,bufferAddr,bufferSize,LOS_NO_WAIT);
}

UINT32 messageQueueGet(UINT32 queueID,VOID *bufferAddr,UINT32 *bufferSize){
	return LOS_QueueReadCopy(queueID,bufferAddr,bufferSize,LOS_WAIT_FOREVER);
}
