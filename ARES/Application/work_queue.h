#ifndef WORK_QUEUE_H
#define WORK_QUEUE_H

#include "stdint.h"

typedef struct Work {
  int (*function)(void *arg);
  void *argument;
} Work;

extern uint8_t Workqueue_schedule(const Work *work);

#define DEFINE_WORK_STATIC(name, func, arg) Work name = {.function = func, .argument = arg}

#ifndef WORK_QUEUE_LEN
#define WORK_QUEUE_LEN 32
#endif

#endif