/**
 * @file chrono.c
 * @author proton
 * @brief misc functions for chrono stuff
 * @version 0.1
 * @date 2023-12-28
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "chrono.h"
#include "dwt_al.h"
#include "initcall.h"
#include "stm32f4xx_hal_rcc.h"
#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "task.h"
#include "log.h"
#include "tim_al.h"
#include "macro.h"
#include "ram_al.h"

ChronoTick64 InitalTick64;
uint32_t     SysClkFreq;

/**
 * @brief return the elapsed time in second
 * @note the elapsed time should be under UINT32_MAX/SYSCLK_FREQ otherwise it overflows
 * @param start the start tick
 * @param end   the end tick
 * @return float the elapsed time in second
 */
inline float Chrono_diff32(ChronoTick32 start, ChronoTick32 end) {
  return (float)((uint32_t)(end - start)) / SysClkFreq;
}

/**
 * @brief return the elapsed time in second
 * @note the elapsed time will not overflow but it takes more calc than Chrono_diffSubSecond
 * @param start the start tick
 * @param end   the end tick
 * @return float the elapsed time in second
 */
float Chrono_diff64(ChronoTick64 *start, ChronoTick64 *end) {
  ChronoTick64 endval;
  if (end == NULL) {
    endval = Chrono_get64();
  } else {
    endval = *end;
  }
  return (float)((uint64_t)(endval - *start)) / SysClkFreq;
}

RAM_FUCNTION int Chrono_delayCallback(void *arg) {
  BaseType_t xHigherPriorityTaskWoken;
  vTaskNotifyGiveFromISR((TaskHandle_t)arg, &xHigherPriorityTaskWoken);
  // invoke PendSV to call scheduler
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  return ARES_SUCCESS;
}

/**
 * @brief delay specific us in task context. during delay, other task may run.
 * @note  due to the hw limitation only 4 task can be pending simultaneously.
 *        can be solved by virtualize hw tim, TBD.
 * 
 * @param us 
 */
void Chrono_usdelayOs(uint32_t us) {
  // the function is only available in task context
  if (unlikely(xPortIsInsideInterrupt() == pdTRUE)) {
    LOG_E("Chrono_usdelayOs is not designed to use under isr context");
    for (;;)
      ;
  }
  // delay itself takes about CHRONO_DELAY_OFFSET us, compensate it
  if (us > CHRONO_DELAY_OFFSET)
    us -= CHRONO_DELAY_OFFSET;
  else {
    taskYIELD();
    return;
  }

  // if delay is longer than 16bit
  if (us > 0xFFFF) {
    osDelay(us / 1000);
    us %= 1000;
  }
  TimDelayCall call = ARGED_FUNC(Chrono_delayCallback, ((void *)xTaskGetCurrentTaskHandle()));

  int ret = Timer_setupDelay(&call, us);
  // no available Hw
  if (unlikely(ret != ARES_SUCCESS)) {
    LOG_E("no available delay timesrc, degenerate to osDelay");
    osDelay(us / 1000);
    return;
  }
  ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
}

static int Chrono_test() {
  STOPWATCH32_TIC(test_chrono);
  Chrono_usdelayOs(200);
  Chrono_usdelayOs(200);
  Chrono_usdelayOs(200);
  float delay = STOPWATCH32_TOC(test_chrono);
  LOG_I("delay %f us, expected 600 us", delay * 1e6);
  return 0;
}
Initcall_registerPostOs(Chrono_test);

int Chrono_init(void) {
  SysClkFreq = HAL_RCC_GetSysClockFreq();
  Dwt_init();
  TimHw_init();
  InitalTick64 = Chrono_get64();
  LOG_I("SysClkFreq\t%d", SysClkFreq);
  return 0;
}
Initcall_registerDevice(Chrono_init);
