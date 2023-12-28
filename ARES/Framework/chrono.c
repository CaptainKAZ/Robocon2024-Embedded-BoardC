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

typedef struct OsDelaySrc {
  SemaphoreHandle_t sem;
  StaticSemaphore_t semBuffer;
  DelayTimSrc       src;
} OsDelaySrc;
OsDelaySrc osDelaySrc[DELAYTIMSRC_HW_NUM];

static void Chrono_delayCallback(DelayTimSrc *timSrc) {
  OsDelaySrc *src = container_of(timSrc, OsDelaySrc, src);
  xSemaphoreGiveFromISR(src->sem, NULL);
  // must invoke PendSV to let other tasks run
  taskYIELD();
}

inline ChronoTickType Chrono_get(void) { return Dwt_get(); }

inline float Chrono_diff(ChronoTickType start, ChronoTickType end) { return (float)((uint32_t)(end - start)) / sysClkFreq; }

void Chrono_usdelayOs(uint32_t us) {
  // delay itself takes about CHRONO_DELAY_OFFSET us, compensate it
  if (us > CHRONO_DELAY_OFFSET)
    us -= CHRONO_DELAY_OFFSET;
  else
    taskYIELD();
  // if delay is longer than 16bit
  if (us > 0xFFFF) {
    osDelay(us / 1000);
    us %= 1000;
  }
  // find an idle timesrc
  for (int i = 0; i < DELAYTIMSRC_HW_NUM; i++) {
    if (osDelaySrc[i].src.state == TIMER_IDLE) {
      DelayTimSrc_setup(&osDelaySrc[i].src, us);
      xSemaphoreTake(osDelaySrc[i].sem, portMAX_DELAY);
      return;
    }
  }
  // not found
  LOG_E("no available delay timesrc, degenerate to osDelay");
  osDelay(us / 1000);
}

static int Chrono_test() {
  STOPWATCH_TIC(test_chrono);
  Chrono_usdelayOs(100);
  Chrono_usdelayOs(100);
  Chrono_usdelayOs(100);
  Chrono_usdelayOs(100);
  Chrono_usdelayOs(100);
  Chrono_usdelayOs(100);
  float delay = STOPWATCH_TOC(test_chrono);
  LOG_I("delay %f s, expected 600 us", delay);
  return 0;
}
register_initcall_post_os(Chrono_test);

int Chrono_init(void) {
  sysClkFreq = HAL_RCC_GetSysClockFreq();
  Dwt_init();
  for (int i = 0; i < DELAYTIMSRC_HW_NUM; i++) {
    osDelaySrc[i].sem = xSemaphoreCreateBinaryStatic(&osDelaySrc[i].semBuffer);
    DelayTimSrc_init(&osDelaySrc[i].src, (DelayTimSrcHw)i, Chrono_delayCallback);
  }
  return 0;
}
register_initcall_device(Chrono_init);
