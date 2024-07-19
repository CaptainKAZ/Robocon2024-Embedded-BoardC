#include "SEGGER_RTT.h"
#include "chrono.h"
#include "cmsis_os2.h"
#include "initcall.h"
#include "macro.h"
#include "periodic.h"
#include <stdint.h>

#include "protocol/vofa_justfloat.h"

#define CONFIG_MAX_PLOTS   (10)
#define CONFIG_UP_BUFFER   (1024)
#define CONFIG_RTT_BUF_IDX (1)

typedef struct AresGdbPlotRequest {
  void *address;
  enum {
    TYPE_UNKNOWN,
    TYPE_U8,
    TYPE_U16,
    TYPE_U32,
    TYPE_I8,
    TYPE_I16,
    TYPE_I32,
    TYPE_F32,
    TYPE_F64
  } type;
  char name[32];
} AresGdbPlotRequest;

static uint8_t     plot_buffer[CONFIG_UP_BUFFER];
AresGdbPlotRequest aresGdbPlotReq[CONFIG_MAX_PLOTS];

int DynamicPlot_init(void) {
  SEGGER_RTT_ConfigUpBuffer(CONFIG_RTT_BUF_IDX, "plot", plot_buffer, ARRAY_LEN(plot_buffer),
                            SEGGER_RTT_MODE_NO_BLOCK_SKIP);
  return ARES_SUCCESS;
}
Initcall_registerDevice(DynamicPlot_init);

int DynamicPlot_run(void *arg) {
  JustFloatFrame frame;
  uint8_t buffer[(ARRAY_LEN(aresGdbPlotReq) + 1) * sizeof(float) + ARRAY_LEN(justFloat_tail)];
  JustFloat_init(&frame, buffer, ARRAY_LEN(buffer));
  for (size_t i = 0; i < ARRAY_LEN(aresGdbPlotReq); i++) {
    AresGdbPlotRequest *gdbReq = &aresGdbPlotReq[i];
    if (gdbReq->address != NULL) {
      switch (gdbReq->type) {
      case TYPE_UNKNOWN: break;
      case TYPE_U8: JustFloat_add(&frame, (float)(*((uint8_t *)gdbReq->address))); break;
      case TYPE_U16: JustFloat_add(&frame, (float)(*((uint16_t *)gdbReq->address))); break;
      case TYPE_U32: JustFloat_add(&frame, (float)(*((uint32_t *)gdbReq->address))); break;
      case TYPE_I8: JustFloat_add(&frame, (float)(*((int8_t *)gdbReq->address))); break;
      case TYPE_I16: JustFloat_add(&frame, (float)(*((int16_t *)gdbReq->address))); break;
      case TYPE_I32: JustFloat_add(&frame, (float)(*((int32_t *)gdbReq->address))); break;
      case TYPE_F32: JustFloat_add(&frame, (float)(*((float *)gdbReq->address))); break;
      case TYPE_F64: JustFloat_add(&frame, (float)(*((double *)gdbReq->address))); break;
      default: break;
      }
    }
  }
  size_t send_size = JustFloat_pack(&frame);
  if (send_size) {
    SEGGER_RTT_Write(CONFIG_RTT_BUF_IDX, buffer, send_size);
  }
  return ARES_SUCCESS;
}

Periodic_registerStatic(FREQ_1000HZ, DynamicPlot_run, NULL);