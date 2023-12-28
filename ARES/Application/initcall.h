#ifndef INIT_CALL_H
#define INIT_CALL_H

#include "ares_section.h"

typedef struct InitCall {
  char *name;
  int (*func)(void);
} InitCall;

/* register sequences before HAL_init() */
#define register_initcall_pure(fun) static InitCall PURE_INIT __initcall_##fun##_pure = {.name = #fun, .func = fun}
/* register sequences after HAL_init() and before osKernelStart() */
#define register_initcall_device(fun) static InitCall DEVICE_INIT __initcall_##fun##_device = {.name = #fun, .func = fun}
/* register sequences after osKernelStart() under task context */
#define register_initcall_post_os(fun) static InitCall POST_OS_INIT __initcall_##fun##_post = {.name = #fun, .func = fun}

extern int do_pure_initcall();
extern int do_device_initcall();
extern int do_post_os_initcall();

#endif