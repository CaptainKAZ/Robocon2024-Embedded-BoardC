/**
 * @file dwt_al.h
 * @author proton
 * @brief cortex m4 dwt hw warpper
 * @version 0.1
 * @date 2023-12-28
 * 
 * @ref: https://www.amobbs.com/thread-5665490-1-1.html
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef DWT_AL_H
#define DWT_AL_H

#include "main.h"
#include "nvic_al.h"

typedef uint32_t         DwtTick;
typedef uint64_t         DwtTick64;
extern volatile uint32_t Dwt_Period;

extern void Dwt_init();

inline DwtTick Dwt_get() { return DWT->CYCCNT; }

extern int Dwt_updatePeriod();

inline DwtTick64 Dwt_get64() {
  Dwt_updatePeriod();
  return (DwtTick64)Dwt_Period << 32 | Dwt_get();
}

#endif