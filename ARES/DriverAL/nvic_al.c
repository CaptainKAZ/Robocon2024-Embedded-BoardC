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
#include "chrono.h"
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
  X_MACRO_Naked(NMI)                        \
  X_MACRO_Naked(HardFault)                                  \
  X_MACRO_Naked(MemManage)                \
  X_MACRO_Naked(BusFault)                         \
  X_MACRO_Naked(UsageFault)                     \
  DUMMY_IRQ                                                      \
  DUMMY_IRQ                                                      \
  DUMMY_IRQ                                                      \
  DUMMY_IRQ                                                      \
  X_MACRO_Naked(SVC)                                \
  X_MACRO_Naked(DebugMon)                     \
  DUMMY_IRQ                                                      \
  X_MACRO_Naked(PendSV)                             \
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
static const uint8_t         IRQn2AresMap[128]               = {ARES_NVIC_XMACROS};
static SList                *Nvic_SOIHandlers[NUM_NVIC_IRQs] = {NULL};
static SList                *Nvic_EOIHandlers[NUM_NVIC_IRQs] = {NULL};
static volatile ChronoTick32 Nvic_IrqDuration[NUM_NVIC_IRQs] = {0};
static volatile uint32_t     Nvic_IrqCount[NUM_NVIC_IRQs]    = {0};

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
  SList *cur = Nvic_SOIHandlers[ares_irqn];
  while (cur != NULL) {
    Nvic_IrqHandler *handler = container_of(cur, Nvic_IrqHandler, list);
    handler->isr_callback(handler->param);
    cur = cur->next;
  }
}
static void Nvic_doEOI(Nvic_IRQe ares_irqn) {
  SList *cur = Nvic_EOIHandlers[ares_irqn];
  while (cur != NULL) {
    Nvic_IrqHandler *handler = container_of(cur, Nvic_IrqHandler, list);
    handler->isr_callback(handler->param);
    cur = cur->next;
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

static void Nvic_Statistics() {
  static uint32_t last_tick = 0;
  if (last_tick == 0) {
    last_tick = HAL_GetTick();
  }
  if (HAL_GetTick() - last_tick > 1000) {
    for (size_t i = 0; i < NUM_NVIC_IRQs; i++) {
      if (Nvic_IrqCount[i] != 0) {
        LOG_I_STAMPED("%s: freq = %d, total run time %fs", Nvic_Names[i], Nvic_IrqCount[i],
              Chrono_diff32(0, Nvic_IrqDuration[i]));
        Nvic_IrqCount[i]    = 0;
        Nvic_IrqDuration[i] = 0;
      }
    }
    last_tick = HAL_GetTick();
  }
}

Nvic_IrqHandler Nvic_StatisticsHandler = {.isr_callback = Nvic_Statistics};

static int Nvic_init(void) {
  __disable_irq();
  SCB->VTOR = (uint32_t)AresVecTab;
  __DSB();
  __enable_irq();
  return ARES_SUCCESS;
}
Initcall_registerPure(Nvic_init);

// RAM_FUCNTION void TIM1_BRK_TIM9_IRQHandler() { TimerHw_isr(&htim9); }
// RAM_FUCNTION void TIM8_BRK_TIM12_IRQHandler() { TimerHw_isr(&htim12); }

// extern TIM_HandleTypeDef htim6;

// void TIM6_DAC_IRQHandler(void) {
//   if (TIM6->SR & TIM_IT_UPDATE) {
//     HAL_IncTick();
//     TIM6->SR = ~(TIM_IT_UPDATE);
//   }
// }

// static volatile void *AresVecTab[128] __attribute__((aligned(0x200))) = {
//     &_estack,
//     Reset_Handler,
//     NMI_Handler,
//     HardFault_Handler,
//     MemManage_Handler,
//     BusFault_Handler,
//     UsageFault_Handler,
//     0,
//     0,
//     0,
//     0,
//     SVC_Handler,
//     DebugMon_Handler,
//     0,
//     PendSV_Handler,
//     SysTick_Handler,
//     WWDG_IRQHandler,
//     PVD_IRQHandler,
//     TAMP_STAMP_IRQHandler,
//     RTC_WKUP_IRQHandler,
//     FLASH_IRQHandler,
//     RCC_IRQHandler,
//     EXTI0_IRQHandler,
//     EXTI1_IRQHandler,
//     EXTI2_IRQHandler,
//     EXTI3_IRQHandler,
//     EXTI4_IRQHandler,
//     DMA1_Stream0_IRQHandler,
//     DMA1_Stream1_IRQHandler,
//     DMA1_Stream2_IRQHandler,
//     DMA1_Stream3_IRQHandler,
//     DMA1_Stream4_IRQHandler,
//     DMA1_Stream5_IRQHandler,
//     DMA1_Stream6_IRQHandler,
//     ADC_IRQHandler,
//     CAN1_TX_IRQHandler,
//     CAN1_RX0_IRQHandler,
//     CAN1_RX1_IRQHandler,
//     CAN1_SCE_IRQHandler,
//     EXTI9_5_IRQHandler,
//     TIM1_BRK_TIM9_IRQHandler,
//     TIM1_UP_TIM10_IRQHandler,
//     TIM1_TRG_COM_TIM11_IRQHandler,
//     TIM1_CC_IRQHandler,
//     TIM2_IRQHandler,
//     TIM3_IRQHandler,
//     TIM4_IRQHandler,
//     I2C1_EV_IRQHandler,
//     I2C1_ER_IRQHandler,
//     I2C2_EV_IRQHandler,
//     I2C2_ER_IRQHandler,
//     SPI1_IRQHandler,
//     SPI2_IRQHandler,
//     USART1_IRQHandler,
//     USART2_IRQHandler,
//     USART3_IRQHandler,
//     EXTI15_10_IRQHandler,
//     RTC_Alarm_IRQHandler,
//     OTG_FS_WKUP_IRQHandler,
//     TIM8_BRK_TIM12_IRQHandler,
//     TIM8_UP_TIM13_IRQHandler,
//     TIM8_TRG_COM_TIM14_IRQHandler,
//     TIM8_CC_IRQHandler,
//     DMA1_Stream7_IRQHandler,
//     FSMC_IRQHandler,
//     SDIO_IRQHandler,
//     TIM5_IRQHandler,
//     SPI3_IRQHandler,
//     UART4_IRQHandler,
//     UART5_IRQHandler,
//     TIM6_DAC_IRQHandler,
//     TIM7_IRQHandler,
//     DMA2_Stream0_IRQHandler,
//     DMA2_Stream1_IRQHandler,
//     DMA2_Stream2_IRQHandler,
//     DMA2_Stream3_IRQHandler,
//     DMA2_Stream4_IRQHandler,
//     ETH_IRQHandler,
//     ETH_WKUP_IRQHandler,
//     CAN2_TX_IRQHandler,
//     CAN2_RX0_IRQHandler,
//     CAN2_RX1_IRQHandler,
//     CAN2_SCE_IRQHandler,
//     OTG_FS_IRQHandler,
//     DMA2_Stream5_IRQHandler,
//     DMA2_Stream6_IRQHandler,
//     DMA2_Stream7_IRQHandler,
//     USART6_IRQHandler,
//     I2C3_EV_IRQHandler,
//     I2C3_ER_IRQHandler,
//     OTG_HS_EP1_OUT_IRQHandler,
//     OTG_HS_EP1_IN_IRQHandler,
//     OTG_HS_WKUP_IRQHandler,
//     OTG_HS_IRQHandler,
//     DCMI_IRQHandler,
//     0,
//     HASH_RNG_IRQHandler,
//     FPU_IRQHandler,
// };