#include "dwt_al.h"
#include "nvic_al.h"
#include "ram_al.h"
#include "stm32f407xx.h"

CCM_DATA volatile uint32_t Dwt_Period = 0;

Nvic_IrqHandler Dwt_updatePeriodHandler = {.isr_callback = Dwt_updatePeriod};

void Dwt_init() {
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CYCCNT = 0;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
  Nvic_requestEOI(SysTick_IRQn, &Dwt_updatePeriodHandler);
}

void Dwt_updatePeriod() {
  static DwtTick last = 0;
  DwtTick        now  = Dwt_get();
  if (last > now) {
    ++Dwt_Period;
  }
  last = now;
}