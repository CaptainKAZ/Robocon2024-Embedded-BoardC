#include "main.h"
#include "work_queue.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "initcall.h" // post_os initcall is executed in through workqueue
#include "log.h"
#include "cmsis_os2.h"

static uint8_t       xQueueStorage[WORK_QUEUE_LEN * sizeof(Work)];
static StaticQueue_t xQueueBuffer;
static QueueHandle_t work_queue;

DEFINE_WORK_STATIC(initcall, Initcall_doPostOs, NULL);

uint8_t Workqueue_schedule(const Work *work) {
  if (work_queue == NULL) {
    work_queue = xQueueCreateStatic(WORK_QUEUE_LEN, sizeof(Work), xQueueStorage, &xQueueBuffer);
  }
  // LOG_D("Scheduling work: 0x%x, function: 0x%x argument: 0x%x", work, work->function, work->argument);
  return xQueueSend(work_queue, work, portMAX_DELAY) == pdTRUE;
}

void worker_task(void *arg) {
  Work work;
  LOG_D_STAMPED("Entering worker task...");
  Workqueue_schedule(&initcall);
  for (;;) {
    if (xQueueReceive(work_queue, &work, portMAX_DELAY) == pdTRUE) {
      // LOG_D("[worker_task] Got work: 0x%x, function: 0x%x argument: 0x%x", &work, work.function, work.argument);
      work.function(work.argument);
    }
    taskYIELD();
  }
}