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

typedef uint32_t DwtTickType;

inline void Dwt_init() {
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CYCCNT = 0;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}
inline DwtTickType Dwt_get() { return DWT->CYCCNT; }

#endif