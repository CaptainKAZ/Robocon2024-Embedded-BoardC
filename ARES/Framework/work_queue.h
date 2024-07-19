/**
 * @file work_queue.h
 * @author proton
 * @brief a workqueue for misc tasks
 * @version 0.1
 * @date 2024-03-31
 * 
 * 
 * @todo use cmsis os2 instead of freertos APIs
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WORK_QUEUE_H
#define WORK_QUEUE_H

#include "stdint.h"
#include "macro.h"

typedef struct ArgedFunc Work;

extern int Workqueue_schedule(const Work *work);

#define DECLARE_WORK_STATIC(name, func, arg) Work name = ARGED_FUNC(func, arg)

#ifndef WORK_QUEUE_LEN
#define WORK_QUEUE_LEN 32
#endif

#endif