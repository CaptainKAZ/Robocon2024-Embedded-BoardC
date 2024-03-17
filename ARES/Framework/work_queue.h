#ifndef WORK_QUEUE_H
#define WORK_QUEUE_H

#include "stdint.h"
#include "macro.h"

typedef struct ArgedFunc Work;

extern int Workqueue_schedule(const Work *work);

#define DECLEAR_WORK_STATIC(name, func, arg) Work name = ARGED_FUNC(func, arg)

#ifndef WORK_QUEUE_LEN
#define WORK_QUEUE_LEN 32
#endif

#endif