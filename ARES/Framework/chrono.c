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
#include "initcall.h"
#include "stm32f4xx_hal_rcc.h"
#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "log.h"
#include "tim_al.h"
#include "macro.h"
static uint32_t sysClkFreq;

static void Chrono_delayCallback(void *arg) {
  vTaskNotifyGiveFromISR((TaskHandle_t)arg, NULL);
  // must invoke PendSV to call scheduler
  taskYIELD();
}

inline ChronoTick Chrono_get(void) { return Dwt_get(); }

inline float Chrono_diff(ChronoTick start, ChronoTick end) { return (float)((uint32_t)(end - start)) / sysClkFreq; }

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
  TimDelayCall call = {.callback = Chrono_delayCallback, .arg = (void *)xTaskGetCurrentTaskHandle()};

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
  STOPWATCH_TIC(test_chrono);
  Chrono_usdelayOs(200);
  Chrono_usdelayOs(200);
  Chrono_usdelayOs(200);
  float delay = STOPWATCH_TOC(test_chrono);
  LOG_I("delay %f s, expected 600 us", delay);
  return 0;
}
Initcall_registerPostOs(Chrono_test);

int Chrono_init(void) {
  sysClkFreq = HAL_RCC_GetSysClockFreq();
  Dwt_init();
  TimHw_init();
  return 0;
}
Initcall_registerDevice(Chrono_init);
