/**
 * @file periodic.h
 * @author proton
 * @brief provide a way to execute event-driven taskes
 * @version 0.1
 * @date 2024-03-16
 * 
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

#ifdef PERIODIC_XMACROS
#undef PERIODIC_XMACROS
#endif
#define PERIODIC_XMACRO(freq)

/* Add needed freq here */
#define ARES_PERIODIC_XMACROS                                                                      \
  PERIODIC_XMACRO(1000)                                                                            \
  PERIODIC_XMACRO(500)                                                                             \
  PERIODIC_XMACRO(250)                                                                             \
  PERIODIC_XMACRO(1)

#undef PERIODIC_XMACRO
#define PERIODIC_XMACRO(freq) FREQ_##freq##HZ,
typedef enum PeriodicFreq { ARES_PERIODIC_XMACROS NUM_FREQ } PeriodicFreq;

#undef PERIODIC_XMACRO

typedef struct PeriodicFunc {
  struct ArgedFunc callback;
  SList            entrys;
} PeriodicFunc;

extern void Periodic_showDiagnosis(void);
extern int  Periodic_register(PeriodicFreq freq, PeriodicFunc *func);
#define Periodic_registerStatic(freq, func, args)                                                  \
  static PeriodicFunc __##func##_periodic_func = {                                                 \
      .callback = ARGED_FUNC(func, args),                                                          \
  };                                                                                               \
  static int __periodic_register_##func(void) {                                                    \
    Periodic_register(freq, &__##func##_periodic_func);                                            \
    return ARES_SUCCESS;                                                                           \
  }                                                                                                \
  Initcall_registerPostOs(__periodic_register_##func)

#endif