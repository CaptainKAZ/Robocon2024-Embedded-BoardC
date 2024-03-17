/**
 * @file tim_al.h
 * @author proton
 * @brief stm32 tim driver warpper for delay in interrupt way
 * @version 0.1
 * @date 2023-12-28
 * 
 * @attention: the tim should be prepared for 1US PER TICK
 * 
 * @ref: https://www.armbbs.cn/forum.php?mod=viewthread&tid=121371
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef TIM_AL_H
#define TIM_AL_H

#include "main.h"
#include "tim.h"
#include "macro.h"
typedef enum DelayTimSrcStat {
  TIMER_IDLE = 0,
  TIMER_PENDING,
} DelayTimSrcStat;

typedef volatile struct TimHw TimHw;

typedef void (*TimCallback)(void *);

typedef struct ArgedFunc TimDelayCall;

extern void TimHw_init();
extern int  Timer_setupDelay(TimDelayCall *call, uint16_t delay);

#endif