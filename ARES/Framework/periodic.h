/**
 * @file periodic.h
 * @author proton
 * @brief provide a way to execute periodic tasks
 * @version 0.1
 * @date 2024-03-16
 *
 * @todo use cmsis_os2 apis instead of raw freertos apis 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef PERIODIC_H
#define PERIODIC_H

#include <stdint.h>
#include "macro.h"
#include "ares_section.h"
#include "slist.h"
#include "initcall.h"

#ifdef PERIODIC_FREQ_XMACRO
#undef PERIODIC_FREQ_XMACRO
#endif
#define PERIODIC_FREQ_XMACRO(freq)

/* Add needed freq here */
#define ARES_PERIODIC_FREQ_XMACROS                                                                 \
  PERIODIC_FREQ_XMACRO(1000)                                                                       \
  PERIODIC_FREQ_XMACRO(500)                                                                        \
  PERIODIC_FREQ_XMACRO(250)                                                                        \
  PERIODIC_FREQ_XMACRO(1)

#undef PERIODIC_FREQ_XMACRO
#define PERIODIC_FREQ_XMACRO(freq) FREQ_##freq##HZ,
typedef enum PeriodicFreq { ARES_PERIODIC_FREQ_XMACROS NUM_FREQ } PeriodicFreq;

#undef PERIODIC_FREQ_XMACRO

typedef struct PeriodicFunc {
  struct ArgedFunc callback;
  sList            entrys;
} PeriodicFunc;

extern void Periodic_showDiagnosis(void);
extern int  Periodic_register(PeriodicFreq freq, PeriodicFunc *func);
#define Periodic_registerStatic(freq, func, args)                                                  \
  static PeriodicFunc __##func##_periodic_func = {                                                 \
      .callback = ARGED_FUNC(func, args),                                                          \
  };                                                                                               \
  static int __periodic_register_##func(void) {                                                    \
    return Periodic_register(freq, &__##func##_periodic_func);                                     \
  }                                                                                                \
  Initcall_registerPostOs(__periodic_register_##func)

#endif