#include "log.h"
#include "initcall.h"
#include "macro.h"
#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdio.h>
#include "cmsis_os.h"

static uint8_t logInited = 0;

int Log_init(void) {
  SEGGER_RTT_Init();
  SEGGER_RTT_ConfigUpBuffer(0, "SEGGER RTT", NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_TRIM);
  SEGGER_RTT_ConfigDownBuffer(0, "SEGGER RTT", NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_TRIM);
  logInited = 1;
  LOG_D("!!!!!!! Log subsystem initialized !!!!!!!!!!");
  LOG_E("           ___    ____  ___________");
  LOG_E("          /   |  / __ \\/ ____/ ___/");
  LOG_E("         / /| | / /_/ / __/  \\__ \\ ");
  LOG_E("        / ___ |/ _, _/ /___ ___/ / ");
  LOG_E("       /_/  |_/_/ |_/_____//____/  ");
  LOG_W("Association of Robotics Engineers in SUSTech");
  LOG_I();
  LOG_I("Rebuild time:\t%s %s", __DATE__, __TIME__);
#ifdef __GIT_COMMIT_HASH
  LOG_I("Git commit:\t%s", __GIT_COMMIT_HASH);
#endif
  LOG_I("STM32 UID:\t%08X %08X %08X", HAL_GetUIDw0(), HAL_GetUIDw1, HAL_GetUIDw2());
  LOG_I("HAL_VERSION:\t%08X", HAL_GetHalVersion());
  LOG_I("KERNEL_VERSION:\t%s", RTOS_ID_s);
  return ARES_SUCCESS;
}

void Log_printf(const char *fmt, ...) {
  if (!logInited)
    return;

  char    buf[128];
  uint8_t len;
  va_list args;
  va_start(args, fmt);
  len = vsnprintf(buf, sizeof(buf), fmt, args);
  SEGGER_RTT_Write(0, buf, len);
  va_end(args);
}

Initcall_registerPure(Log_init);