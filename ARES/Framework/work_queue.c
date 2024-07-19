/* @todo use cmsis_os2 instead of freertos APIs */
#include "macro.h"
#include "main.h"
#include "work_queue.h"
#include "FreeRTOS.h"
#include "portmacro.h"
#include "projdefs.h"
#include "queue.h"
#include "initcall.h" // post_os initcall is executed in through workqueue
#include "log.h"
#include "cmsis_os2.h"

static uint8_t       xQueueStorage[WORK_QUEUE_LEN * sizeof(Work)];
static StaticQueue_t xQueueBuffer;
static QueueHandle_t workQueue;

DECLARE_WORK_STATIC(initcall, Initcall_doPostOs, NULL);

int Workqueue_schedule(const Work *work) {
  // LOG_D("Scheduling work: 0x%x, function: 0x%x argument: 0x%x", work, work->function, work->argument);
  if (xPortIsInsideInterrupt()) {
    BaseType_t highPrioTaskWoken, ret;
    ret = xQueueSendFromISR(workQueue, work, &highPrioTaskWoken);
    portYIELD_FROM_ISR(highPrioTaskWoken);
    return ret;
  } else {
    return xQueueSend(workQueue, work, portMAX_DELAY) == pdTRUE;
  }
}

void worker_task(void *arg) {
  Work work;
  workQueue = xQueueCreateStatic(WORK_QUEUE_LEN, sizeof(Work), xQueueStorage, &xQueueBuffer);
  LOG_D_STAMPED("Entering worker task...");
  Workqueue_schedule(&initcall);
  for (;;) {
    if (xQueueReceive(workQueue, &work, portMAX_DELAY) == pdTRUE) {
      // LOG_D("[worker_task] Got work: 0x%x, function: 0x%x argument: 0x%x", &work, work.function, work.argument);
      RUN_ARGED_FUNC(work);
    }
    taskYIELD();
  }
}