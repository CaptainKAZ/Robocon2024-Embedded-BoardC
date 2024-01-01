#ifndef INIT_CALL_H
#define INIT_CALL_H

#include "ares_section.h"

typedef struct InitCall {
  char *name;
  int (*func)(void);
} InitCall;

/* register sequences before HAL_init() */
#define Initcall_registerPure(fun) static InitCall PURE_INIT __initcall_##fun##_pure = {.name = #fun, .func = fun}
/* register sequences after HAL_init() and before osKernelStart() */
#define Initcall_registerDevice(fun) static InitCall DEVICE_INIT __initcall_##fun##_device = {.name = #fun, .func = fun}
/* register sequences after osKernelStart() under task context */
#define Initcall_registerPostOs(fun) static InitCall POST_OS_INIT __initcall_##fun##_post = {.name = #fun, .func = fun}

extern int Initcall_doPure();
extern int Initcall_doDevice();
extern int Initcall_doPostOs();

#endif