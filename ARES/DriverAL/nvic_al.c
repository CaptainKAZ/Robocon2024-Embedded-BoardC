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

#include "nvic_al.h"
#include "FreeRTOS.h"
#include "chrono.h"
#include "cmsis_os2.h"
#include "initcall.h"
#include "macro.h"
#include "slist.h"
#include "stdint.h"
#include "cmsis_gcc.h"
#include "stm32f407xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_conf.h"
#include "tim_al.h"
#include "ram_al.h"
#include <stdint.h>
#include "stm32f4xx_it.h"
#include "log.h"

/* clear all related macro defines */
#ifdef X_MACRO
#undef X_MACRO
#endif
#ifdef X_MACRO_Naked
#undef X_MACRO_Naked
#endif
#ifdef X_MACRO_Cortex
#undef X_MACRO_Cortex
#endif
#ifdef X_MACRO_IRQ
#undef X_MACRO_IRQ
#endif
#ifdef DUMMY_IRQ
#undef DUMMY_IRQ
#endif

/* clang-format off */
/* define X-MACROs Table */
#define ARES_NVIC_XMACROS                                        \
  X_MACRO_Naked(Reset)                                           \
  X_MACRO_Naked(NMI)                                             \
  X_MACRO_Naked(HardFault)                                       \
  X_MACRO_Naked(MemManage)                                       \
  X_MACRO_Naked(BusFault)                                        \
  X_MACRO_Naked(UsageFault)                                      \
  DUMMY_IRQ                                                      \
  DUMMY_IRQ                                                      \
  DUMMY_IRQ                                                      \
  DUMMY_IRQ                                                      \
  X_MACRO_Naked(SVC)                                             \
  X_MACRO_Naked(DebugMon)                                        \
  DUMMY_IRQ                                                      \
  X_MACRO_Naked(PendSV)                                          \
  X_MACRO_Cortex(SysTick,SysTick_IRQn)                           \
  X_MACRO_IRQ(WWDG)                                              \
  X_MACRO_IRQ(PVD)                                               \
  X_MACRO_IRQ(TAMP_STAMP)                                        \
  X_MACRO_IRQ(RTC_WKUP)                                          \
  X_MACRO_IRQ(FLASH)                                             \
  X_MACRO_IRQ(RCC)                                               \
  X_MACRO_IRQ(EXTI0)                                             \
  X_MACRO_IRQ(EXTI1)                                             \
  X_MACRO_IRQ(EXTI2)                                             \
  X_MACRO_IRQ(EXTI3)                                             \
  X_MACRO_IRQ(EXTI4)                                             \
  X_MACRO_IRQ(DMA1_Stream0)                                      \
  X_MACRO_IRQ(DMA1_Stream1)                                      \
  X_MACRO_IRQ(DMA1_Stream2)                                      \
  X_MACRO_IRQ(DMA1_Stream3)                                      \
  X_MACRO_IRQ(DMA1_Stream4)                                      \
  X_MACRO_IRQ(DMA1_Stream5)                                      \
  X_MACRO_IRQ(DMA1_Stream6)                                      \
  X_MACRO_IRQ(ADC)                                               \
  X_MACRO_IRQ(CAN1_TX)                                           \
  X_MACRO_IRQ(CAN1_RX0)                                          \
  X_MACRO_IRQ(CAN1_RX1)                                          \
  X_MACRO_IRQ(CAN1_SCE)                                          \
  X_MACRO_IRQ(EXTI9_5)                                           \
  X_MACRO_IRQ(TIM1_BRK_TIM9)                                     \
  X_MACRO_IRQ(TIM1_UP_TIM10)                                     \
  X_MACRO_IRQ(TIM1_TRG_COM_TIM11)                                \
  X_MACRO_IRQ(TIM1_CC)                                           \
  X_MACRO_IRQ(TIM2)                                              \
  X_MACRO_IRQ(TIM3)                                              \
  X_MACRO_IRQ(TIM4)                                              \
  X_MACRO_IRQ(I2C1_EV)                                           \
  X_MACRO_IRQ(I2C1_ER)                                           \
  X_MACRO_IRQ(I2C2_EV)                                           \
  X_MACRO_IRQ(I2C2_ER)                                           \
  X_MACRO_IRQ(SPI1)                                              \
  X_MACRO_IRQ(SPI2)                                              \
  X_MACRO_IRQ(USART1)                                            \
  X_MACRO_IRQ(USART2)                                            \
  X_MACRO_IRQ(USART3)                                            \
  X_MACRO_IRQ(EXTI15_10)                                         \
  X_MACRO_IRQ(RTC_Alarm)                                         \
  X_MACRO_IRQ(OTG_FS_WKUP)                                       \
  X_MACRO_IRQ(TIM8_BRK_TIM12)                                    \
  X_MACRO_IRQ(TIM8_UP_TIM13)                                     \
  X_MACRO_IRQ(TIM8_TRG_COM_TIM14)                                \
  X_MACRO_IRQ(TIM8_CC)                                           \
  X_MACRO_IRQ(DMA1_Stream7)                                      \
  X_MACRO_IRQ(FSMC)                                              \
  X_MACRO_IRQ(SDIO)                                              \
  X_MACRO_IRQ(TIM5)                                              \
  X_MACRO_IRQ(SPI3)                                              \
  X_MACRO_IRQ(UART4)                                             \
  X_MACRO_IRQ(UART5)                                             \
  X_MACRO_IRQ(TIM6_DAC)                                          \
  X_MACRO_IRQ(TIM7)                                              \
  X_MACRO_IRQ(DMA2_Stream0)                                      \
  X_MACRO_IRQ(DMA2_Stream1)                                      \
  X_MACRO_IRQ(DMA2_Stream2)                                      \
  X_MACRO_IRQ(DMA2_Stream3)                                      \
  X_MACRO_IRQ(DMA2_Stream4)                                      \
  X_MACRO_IRQ(ETH)                                               \
  X_MACRO_IRQ(ETH_WKUP)                                          \
  X_MACRO_IRQ(CAN2_TX)                                           \
  X_MACRO_IRQ(CAN2_RX0)                                          \
  X_MACRO_IRQ(CAN2_RX1)                                          \
  X_MACRO_IRQ(CAN2_SCE)                                          \
  X_MACRO_IRQ(OTG_FS)                                            \
  X_MACRO_IRQ(DMA2_Stream5)                                      \
  X_MACRO_IRQ(DMA2_Stream6)                                      \
  X_MACRO_IRQ(DMA2_Stream7)                                      \
  X_MACRO_IRQ(USART6)                                            \
  X_MACRO_IRQ(I2C3_EV)                                           \
  X_MACRO_IRQ(I2C3_ER)                                           \
  X_MACRO_IRQ(OTG_HS_EP1_OUT)                                    \
  X_MACRO_IRQ(OTG_HS_EP1_IN)                                     \
  X_MACRO_IRQ(OTG_HS_WKUP)                                       \
  X_MACRO_IRQ(OTG_HS)                                            \
  X_MACRO_IRQ(DCMI)                                              \
  DUMMY_IRQ                                                      \
  X_MACRO_IRQ(HASH_RNG)                                          \
  X_MACRO_IRQ(FPU)
/* clang-format on */

/* define all original external functions */
#define X_MACRO_Naked(irq)         extern void irq##_Handler();
#define X_MACRO_Cortex(irq, dummy) extern void irq##_Handler();
#define X_MACRO_IRQ(irq)           extern void irq##_IRQHandler();
#define DUMMY_IRQ                  ;
extern int _estack;

ARES_NVIC_XMACROS

#undef X_MACRO_Naked
#undef X_MACRO_Cortex
#undef X_MACRO_IRQ
#undef DUMMY_IRQ

/* define ares irq enum type (re-index irqs) */
#define X_MACRO_Naked(irq)
#define DUMMY_IRQ
#define X_MACRO_Cortex(irq, dummy) ARES_Cortex_##irq##_IRQe,
#define X_MACRO_IRQ(irq)           ARES_##irq##_IRQe,

typedef enum Nvic_IRQe {
  ARES_NVIC_XMACROS NUM_NVIC_IRQs,
} Nvic_IRQe;

#undef X_MACRO_Naked
#undef X_MACRO_Cortex
#undef X_MACRO_IRQ
#undef DUMMY_IRQ

/* define ares irq enum name */
#define X_MACRO_Naked(irq)
#define DUMMY_IRQ
#define X_MACRO_Cortex(irq, dummy) [ARES_Cortex_##irq##_IRQe] = #irq,
#define X_MACRO_IRQ(irq)           [ARES_##irq##_IRQe] = #irq,

const char *Nvic_Names[] = {ARES_NVIC_XMACROS};

#undef X_MACRO_Naked
#undef X_MACRO_Cortex
#undef X_MACRO_IRQ
#undef DUMMY_IRQ

#define X_MACRO_Naked(irq)
#define DUMMY_IRQ
#define X_MACRO_Cortex(irq, irqn) [irqn + NVIC_IRQN_OFFSET] = ARES_Cortex_##irq##_IRQe,
#define X_MACRO_IRQ(irq)          [irq##_IRQn + NVIC_IRQN_OFFSET] = ARES_##irq##_IRQe,
/* create a map from real irq index to our new defined one upper */
#define NVIC_IRQN_OFFSET 15
static const uint8_t                  IRQn2AresMap[128]               = {ARES_NVIC_XMACROS};
static sList                         *Nvic_SOIHandlers[NUM_NVIC_IRQs] = {NULL};
static sList                         *Nvic_EOIHandlers[NUM_NVIC_IRQs] = {NULL};
CCM_DATA static volatile ChronoTick32 Nvic_IrqDuration[NUM_NVIC_IRQs] = {0};
CCM_DATA static volatile uint32_t     Nvic_IrqCount[NUM_NVIC_IRQs]    = {0};

#undef X_MACRO_Naked
#undef X_MACRO_Cortex
#undef X_MACRO_IRQ
#undef DUMMY_IRQ

/* define all ares hacked irq functons */
static void Nvic_doSOI(Nvic_IRQe ares_irqn);
static void Nvic_doEOI(Nvic_IRQe ares_irqn);

#define X_MACRO_Naked(irq) ; // Not hack reset of course
#define X_MACRO_Cortex(irq, dummy)                                                                 \
  void ARES_##irq##_Handler() {                                                                    \
    ChronoTick32 now = Chrono_get32();                                                             \
    Nvic_doSOI(ARES_Cortex_##irq##_IRQe);                                                          \
    irq##_Handler();                                                                               \
    Nvic_doEOI(ARES_Cortex_##irq##_IRQe);                                                          \
    Nvic_IrqDuration[ARES_Cortex_##irq##_IRQe] += Chrono_get32() - now;                            \
    ++Nvic_IrqCount[ARES_Cortex_##irq##_IRQe];                                                     \
  }

#define X_MACRO_IRQ(irq)                                                                           \
  void ARES_##irq##_IRQHandler() {                                                                 \
    ChronoTick32 now = Chrono_get32();                                                             \
    Nvic_doSOI(ARES_##irq##_IRQe);                                                                 \
    irq##_IRQHandler();                                                                            \
    Nvic_doEOI(ARES_##irq##_IRQe);                                                                 \
    Nvic_IrqDuration[ARES_##irq##_IRQe] += Chrono_get32() - now;                                   \
    ++Nvic_IrqCount[ARES_##irq##_IRQe];                                                            \
  }
#define DUMMY_IRQ ;

ARES_NVIC_XMACROS

#undef X_MACRO_Naked
#undef X_MACRO_Cortex
#undef X_MACRO_IRQ
#undef DUMMY_IRQ

/* build new irq vector table */
#define X_MACRO_Naked(irq)         irq##_Handler,
#define X_MACRO_Cortex(irq, dummy) ARES_##irq##_Handler,
#define X_MACRO_IRQ(irq)           ARES_##irq##_IRQHandler,
#define DUMMY_IRQ                  NULL,

static const void *AresVecTab[128] __attribute__((aligned(0x200))) = {&_estack, ARES_NVIC_XMACROS};

#undef X_MACRO_Naked
#undef X_MACRO_Cortex
#undef X_MACRO_IRQ
#undef DUMMY_IRQ
/* end of using x-macro tricks */

static void Nvic_doSOI(Nvic_IRQe ares_irqn) {
  Slist_foreach(cur, Nvic_SOIHandlers[ares_irqn]) {
    Nvic_IrqHandler *handler = container_of(cur, Nvic_IrqHandler, list);
    RUN_ARGED_FUNC(handler->func);
  }
}
static void Nvic_doEOI(Nvic_IRQe ares_irqn) {
  Slist_foreach(cur, Nvic_SOIHandlers[ares_irqn]) {
    Nvic_IrqHandler *handler = container_of(cur, Nvic_IrqHandler, list);
    RUN_ARGED_FUNC(handler->func);
  }
}

int Nvic_requestSOI(IRQn_Type irqn, Nvic_IrqHandler *handler) {
  ARES_ASSERT((irqn <= FPU_IRQn), "irqn not valid");
  Nvic_IRQe ares_irqn         = IRQn2AresMap[irqn + NVIC_IRQN_OFFSET];
  Nvic_SOIHandlers[ares_irqn] = SList_pushFront(Nvic_SOIHandlers[ares_irqn], &handler->list);
  return ARES_SUCCESS;
}

int Nvic_requestEOI(IRQn_Type irqn, Nvic_IrqHandler *handler) {
  ARES_ASSERT((irqn <= FPU_IRQn), "irqn not valid");
  Nvic_IRQe ares_irqn         = IRQn2AresMap[irqn + NVIC_IRQN_OFFSET];
  Nvic_EOIHandlers[ares_irqn] = SList_pushFront(Nvic_EOIHandlers[ares_irqn], &handler->list);
  return ARES_SUCCESS;
}

static int Nvic_init(void) {
  __disable_irq();
  SCB->VTOR = (uint32_t)AresVecTab;
  __DSB();
  __enable_irq();
  return ARES_SUCCESS;
}
Initcall_registerPure(Nvic_init);

#include "periodic.h"

static int Nvic_Statistics(void *args) {
  static uint32_t lastTick = 0;
  uint32_t        nowTick  = osKernelGetTickCount();

  for (size_t i = 0; i < NUM_NVIC_IRQs; i++) {
    if (Nvic_IrqCount[i] != 0) {
      LOG_I_STAMPED("%s: freq = %d, total run time %fs", Nvic_Names[i],
                    Nvic_IrqCount[i] * (nowTick - lastTick) / configTICK_RATE_HZ,
                    Chrono_diff32(0, Nvic_IrqDuration[i]));
      Nvic_IrqCount[i]    = 0;
      Nvic_IrqDuration[i] = 0;
    }
  }
  lastTick = nowTick;
  return ARES_SUCCESS;
}

Periodic_registerStatic(FREQ_1HZ, Nvic_Statistics, NULL);