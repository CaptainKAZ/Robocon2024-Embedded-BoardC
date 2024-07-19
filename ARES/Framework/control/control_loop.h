/**
 * @file control_loop.h
 * @author proton
 * @brief provide a rather high accuracy and isolated framework for control loops with frequency monitoring and soft watchdog
 * @version 0.1
 * @date 2024-03-20
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef CONTROL_LOOP_H
#define CONTROL_LOOP_H
#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "periodic.h"
#include "portmacro.h"
#include "slist.h"
#include "macro.h"
#include "cmsis_os.h"
#include <stdint.h>
#include "chrono.h"

typedef struct ControlLoop {
  sList         *steps;
  osThreadAttr_t attr;
  uint8_t        expired;
  TickType_t     timeout;
  osThreadId_t   thread;
  bool_t         isTimeout;
} ControlLoop;

typedef struct ControlStep {
  ControlLoop     *loop;
  sList            entry;
  struct ArgedFunc stepFunc;
} ControlStep;

void ControlLoop_build(ControlLoop *loop, ControlStep *steps, size_t num_steps);
void ControlLoop_run(ControlLoop *loop);
void ControlLoop_waitFor(ControlLoop *loop, uint32_t events);
void ControlLoop_raiseEvent(ControlLoop *loop, uint32_t events);
void ControlLoop_trySyncWith(ControlLoop *loop, ControlLoop *sync);

#endif