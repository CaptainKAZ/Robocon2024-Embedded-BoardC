#ifndef INIT_CALL_H
#define INIT_CALL_H

#include "ares_section.h"
#include "macro.h"

typedef struct NamedFunc InitCall;

#define INIT_CALL(stage) __attribute__((__used__)) AT_SECTION(".init." #stage)

#define PURE_INIT INIT_CALL(pure_init)
#define DEVICE_INIT INIT_CALL(device_init)
#define POST_OS_INIT INIT_CALL(post_os_init)

/* register sequences before HAL_init() */
#define Initcall_registerPure(fun)                                                                 \
  static InitCall PURE_INIT __initcall_##fun##_pure = NAMED_FUNC(fun)
/* register sequences after HAL_init() and before osKernelStart() */
#define Initcall_registerDevice(fun)                                                               \
  static InitCall DEVICE_INIT __initcall_##fun##_device = NAMED_FUNC(fun)
/* register sequences after osKernelStart() under task context */
#define Initcall_registerPostOs(fun)                                                               \
  static InitCall POST_OS_INIT __initcall_##fun##_post = NAMED_FUNC(fun)

extern int Initcall_doPure();
extern int Initcall_doDevice();
extern int Initcall_doPostOs();

#endif