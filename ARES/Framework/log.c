#include "log.h"
#include "initcall.h"
#include <stdio.h>

int log_init(void) {
  SEGGER_RTT_Init();
  SEGGER_RTT_ConfigUpBuffer(0, "SEGGER RTT", NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_TRIM);
  SEGGER_RTT_ConfigDownBuffer(0, "SEGGER RTT", NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_TRIM);
  SEGGER_RTT_ConfigUpBuffer(1, "Graph", NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_TRIM);
  SEGGER_RTT_ConfigDownBuffer(1, "Graph", NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_TRIM);
  LOG_D("!!!!!!! Log subsystem initialized !!!!!!!!!!");
  LOG_I("           ___    ____  ___________");
  LOG_I("          /   |  / __ \\/ ____/ ___/");
  LOG_I("         / /| | / /_/ / __/  \\__ \\ ");
  LOG_I("        / ___ |/ _, _/ /___ ___/ / ");
  LOG_I("       /_/  |_/_/ |_/_____//____/  ");
  LOG_W("Association of Robotics Engineers in SUSTech");
  LOG_I();
  return 0;
}

void do_log(const char *fmt, ...) {
  char    buf[128];
  uint8_t len;
  va_list args;
  va_start(args, fmt);
  len = vsnprintf(buf, sizeof(buf), fmt, args);
  SEGGER_RTT_Write(0, buf, len);
  va_end(args);
}

Initcall_registerPure(log_init);