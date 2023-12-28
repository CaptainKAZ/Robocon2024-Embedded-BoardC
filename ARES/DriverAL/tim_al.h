/**
 * @file tim_al.h
 * @author proton
 * @brief stm32 tim driver warpper for delay in interrupt way
 * @version 0.1
 * @date 2023-12-28
 * 
 * @attention: the timer should be prepared for 1US PER TICK
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

typedef enum DelayTimSrcHw {
  DELAYTIMSRC_TIM9_CH1,
  DELAYTIMSRC_TIM9_CH2,
  DELAYTIMSRC_TIM12_CH1,
  DELAYTIMSRC_TIM12_CH2,
  DELAYTIMSRC_HW_NUM
} DelayTimSrcHw;

typedef enum DelayTimSrcStat {
  TIMER_IDLE = 0,
  TIMER_PENDING,
} DelayTimSrcStat;

typedef volatile struct DelayTimSrc DelayTimSrc;

typedef void (*DelayTimSrcCb)(DelayTimSrc *);

typedef volatile struct DelayTimSrc {
  TIM_HandleTypeDef *tim;
  uint8_t            channel;
  uint8_t            cc_flag;
  DelayTimSrcStat    state;
  DelayTimSrcCb      callback;
} DelayTimSrc;

extern void DelayTimSrc_init(DelayTimSrc *timSrc, DelayTimSrcHw hw, DelayTimSrcCb cb);
extern void DelayTimSrc_setup(DelayTimSrc *timSrc, uint16_t delay);
extern void DelayTimSrc_isr(TIM_HandleTypeDef *htim);

#endif