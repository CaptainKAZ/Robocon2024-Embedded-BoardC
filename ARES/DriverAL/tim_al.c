#include "tim_al.h"
#include "initcall.h"
#include "log.h"

static DelayTimSrc *hwToSrc[DELAYTIMSRC_HW_NUM];

void DelayTimSrc_init(DelayTimSrc *timSrc, DelayTimSrcHw hw, DelayTimSrcCb cb) {
  timSrc->state = TIMER_IDLE;
  switch (hw) {
  case DELAYTIMSRC_TIM9_CH1:
    timSrc->tim     = &htim9;
    timSrc->channel = TIM_CHANNEL_1;
    break;
  case DELAYTIMSRC_TIM9_CH2:
    timSrc->tim     = &htim9;
    timSrc->channel = TIM_CHANNEL_2;
    break;
  case DELAYTIMSRC_TIM12_CH1:
    timSrc->tim     = &htim12;
    timSrc->channel = TIM_CHANNEL_1;
    break;
  case DELAYTIMSRC_TIM12_CH2:
    timSrc->tim     = &htim12;
    timSrc->channel = TIM_CHANNEL_2;
    break;
  default: LOG_E("unknown delay tim src hw %d", hw); break;
  }
  timSrc->cc_flag  = timSrc->channel == TIM_CHANNEL_1 ? TIM_FLAG_CC1 : TIM_FLAG_CC2;
  timSrc->callback = cb;
  if (hwToSrc[hw] == NULL) {
    hwToSrc[hw] = timSrc;
  } else {
    LOG_W("delay tim src %d already exists", hw);
  }
  if (htim9.State == HAL_TIM_STATE_READY)
    HAL_TIM_Base_Start(&htim9);
  if (htim12.State == HAL_TIM_STATE_READY)
    HAL_TIM_Base_Start(&htim12);
}

void DelayTimSrc_setup(DelayTimSrc *timSrc, uint16_t delay) {
  timSrc->state = TIMER_PENDING;
  // if the tim is not enabled, enable it
  if (!(timSrc->tim->Instance->CR1 & TIM_CR1_CEN)) {
    __HAL_TIM_ENABLE(timSrc->tim);
  }
  // TIM9 and TIM12 are 16-bit timers
  uint16_t cnt     = __HAL_TIM_GET_COUNTER(timSrc->tim);
  uint16_t cc_flag = timSrc->cc_flag;
  __HAL_TIM_SET_COMPARE(timSrc->tim, timSrc->channel, (uint16_t)(cnt + delay));
  __HAL_TIM_CLEAR_IT(timSrc->tim, cc_flag);
  __HAL_TIM_ENABLE_IT(timSrc->tim, cc_flag);
}

void DelayTimSrc_isr(TIM_HandleTypeDef *htim) {
  uint8_t expired[DELAYTIMSRC_HW_NUM] = {0};
  if (htim == &htim9) {
    expired[DELAYTIMSRC_TIM9_CH1] = __HAL_TIM_GET_ITSTATUS(htim, TIM_IT_CC1);
    expired[DELAYTIMSRC_TIM9_CH2] = __HAL_TIM_GET_ITSTATUS(htim, TIM_IT_CC2);
  } else if (htim == &htim12) {
    expired[DELAYTIMSRC_TIM12_CH1] = __HAL_TIM_GET_ITSTATUS(htim, TIM_IT_CC1);
    expired[DELAYTIMSRC_TIM12_CH2] = __HAL_TIM_GET_ITSTATUS(htim, TIM_IT_CC2);
  }
  for (int i = 0; i < DELAYTIMSRC_HW_NUM; i++) {
    if (expired[i]) {
      if (hwToSrc[i] == NULL) {
        LOG_W("delay tim src %d expired but not registered", i);
      }
      __HAL_TIM_CLEAR_IT(htim, hwToSrc[i]->cc_flag);
      __HAL_TIM_DISABLE_IT(htim, hwToSrc[i]->cc_flag);
      hwToSrc[i]->callback(hwToSrc[i]);
      hwToSrc[i]->state = TIMER_IDLE;
    }
  }
}
