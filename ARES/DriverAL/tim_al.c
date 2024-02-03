#include "tim_al.h"
#include "log.h"
#include "macro.h"
#include "ares_section.h"
#include "ram_al.h"
#include "nvic_al.h"
#include "stm32f407xx.h"
#include "tim.h"

typedef volatile struct TimHw {
  TIM_HandleTypeDef *tim;
  uint8_t            channel;
  uint8_t            cc_flag;
  DelayTimSrcStat    state;
  TimDelayCall      *call;
} TimHw;

#define TIM_HW(timx, chl)                                                                          \
  {                                                                                                \
    .tim = &h##timx, .cc_flag = TIM_IT_CC##chl, .channel = TIM_CHANNEL_##chl, .state = TIMER_IDLE, \
    .call = NULL                                                                                   \
  }

static TimHw timHw[] = {
    TIM_HW(tim9, 1),
    TIM_HW(tim9, 2),
    TIM_HW(tim12, 1),
    TIM_HW(tim12, 2),
};

#define NUM_HW (sizeof(timHw) / sizeof(TimHw))

/**
 * @brief Call call->callback(call->arg) when @ref "us" passed.
 * 
 * @note the function DONOT copies @ref "call". User MUST make sure the address is usable when time expires.
 * @param call info of callback function
 * @param us the time to delay in us
 * @return int ARES standard errno
 */
int Timer_setupDelay(TimDelayCall *call, uint16_t us) {
  // first find a hw
  TimHw *hw = NULL;
  for (uint8_t i = 0; i < NUM_HW; i++) {
    if (timHw[i].state == TIMER_IDLE) {
      hw = &timHw[i];
      break;
    }
  }
  if (hw == NULL) {
    LOG_E("No available Hw for call");
    return ARES_NO_RESORCE;
  }
  // if the tim is not enabled, enable it
  if (unlikely(!(hw->tim->Instance->CR1 & TIM_CR1_CEN))) {
    __HAL_TIM_ENABLE(hw->tim);
  }
  // TIM9 and TIM12 are 16-bit timers
  uint16_t cnt     = __HAL_TIM_GET_COUNTER(hw->tim);
  hw->state        = TIMER_PENDING;
  hw->call         = call;
  uint16_t cc_flag = hw->cc_flag;
  __HAL_TIM_SET_COMPARE(hw->tim, hw->channel, (uint16_t)(cnt + us));
  __HAL_TIM_CLEAR_IT(hw->tim, cc_flag);
  __HAL_TIM_ENABLE_IT(hw->tim, cc_flag);
  return ARES_SUCCESS;
}

void TimerHw_isr(void *param) {
  // currently we don't use htim
  UNUSED(param);
  // we need to check all hw in case there is concurrency
  for (int i = 0; i < NUM_HW; i++) {
    if (__HAL_TIM_GET_ITSTATUS(timHw[i].tim, timHw[i].cc_flag)) {
      if (timHw[i].call == NULL) {
        LOG_W("tim src %d expired but not registered", i);
      }
      __HAL_TIM_CLEAR_IT(timHw[i].tim, timHw[i].cc_flag);
      __HAL_TIM_DISABLE_IT(timHw[i].tim, timHw[i].cc_flag);
      if (IS_FUNCTION(timHw[i].call->callback))
        timHw[i].call->callback(timHw->call->arg);
      else
        LOG_E("callback %x is not a function, sdata %x, edata %x, etext %x",
              timHw[i].call->callback, (void *)&_sdata, (void *)&_edata, (void *)&_etext);
      timHw[i].state = TIMER_IDLE;
    }
  }
}

static Nvic_IrqHandler Tim9Handler = {
    .isr_callback = TimerHw_isr,
    .param        = &htim9,
};

static Nvic_IrqHandler Tim12Handler = {
    .isr_callback = TimerHw_isr,
    .param        = &htim12,
};

void TimHw_init() {
  HAL_TIM_Base_Start(&htim9);
  HAL_TIM_Base_Start(&htim12);
  Nvic_requestSOI(TIM1_BRK_TIM9_IRQn, &Tim9Handler);
  Nvic_requestSOI(TIM8_BRK_TIM12_IRQn, &Tim12Handler);
}