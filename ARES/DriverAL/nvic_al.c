/**
 * @file nvic_al.c
 * @author proton
 * @brief this is the abstract layer where we define the user callback function tiggered by the HAL driver or NVIC
 * @version 0.1
 * @date 2023-12-27
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "tim_al.h"
#include "ram_al.h"

RAM_FUCNTION void TIM1_BRK_TIM9_IRQHandler() { TimerHw_isr(&htim9); }
RAM_FUCNTION void TIM8_BRK_TIM12_IRQHandler() { TimerHw_isr(&htim12); }

extern TIM_HandleTypeDef htim6;

void TIM6_DAC_IRQHandler(void) {
  if (TIM6->SR & TIM_IT_UPDATE) {
    HAL_IncTick();
    TIM6->SR = ~(TIM_IT_UPDATE);
  }
}