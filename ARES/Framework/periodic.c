#include <stdint.h>
#include "log.h"
#include "periodic.h"
#include "macro.h"
#include "main.h"
#include "slist.h"
#include "FreeRTOS.h"
#include "timers.h"

typedef struct PeriodicHandle {
  uint32_t      divider;
  sList        *head;
  TimerHandle_t timer;
  const char   *name;
} PeriodicHandle;

#define PERIODIC_FREQ_XMACRO(freq)                                                                 \
  [FREQ_##freq##                                                                                   \
      HZ] = {.divider = configTICK_RATE_HZ / freq, .head = NULL, .name = "timer_" #freq "hz"},
static PeriodicHandle periodicHandles[] = {ARES_PERIODIC_FREQ_XMACROS};
#undef PERIODIC_FREQ_XMACRO

static void Periodic_process(TimerHandle_t xTimer) {
  PeriodicHandle *group = pvTimerGetTimerID(xTimer);
  Slist_foreach(item, group->head) {
    PeriodicFunc *fun = container_of(item, PeriodicFunc, entrys);
    RUN_ARGED_FUNC(fun->callback);
  }
}

static int Periodic_init(void) {
  for (size_t i = 0; i < ARRAY_LEN(periodicHandles); i++) {
    periodicHandles[i].timer = xTimerCreate(periodicHandles[i].name, periodicHandles[i].divider,
                                            pdTRUE, (void *)&periodicHandles[i], Periodic_process);
    if (!periodicHandles[i].timer) {
      LOG_E("not enough heap space for periodic soft timers!");
      return ARES_NO_RESOURCE;
    }
    xTimerStart(periodicHandles[i].timer, 0);
  }
  return ARES_SUCCESS;
}
Initcall_registerPostOs(Periodic_init);

int Periodic_register(PeriodicFreq freq, PeriodicFunc *func) {
  ARES_ASSERT((freq >= 0 && freq < NUM_FREQ), "wrong freq, please register your freq first");
  periodicHandles[freq].head = SList_pushFront(periodicHandles[freq].head, &func->entrys);
  return ARES_SUCCESS;
}

void Periodic_showDiagnosis(void) {
  // Unuesd, TBD
  return;
}