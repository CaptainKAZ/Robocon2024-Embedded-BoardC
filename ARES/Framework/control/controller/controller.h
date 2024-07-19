#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "macro.h"
#include "control/interface.h"
#include "log.h"
#include <limits.h>
#include <stdint.h>

#ifdef CONTROLLER_XMACRO
#undef CONTROLLER_XMACRO
#endif

#define ARES_CONTROLLER_XMACRO CONTROLLER_XMACRO(PID)

typedef enum ControllerType {
  CONTROLLER_TYPE_UNKNOWN = 0,
#define CONTROLLER_XMACRO(x) CONTROLLER_TYPE_##x,
  ARES_CONTROLLER_XMACRO
#undef CONTROLLER_XMACRO
      CONTROLLER_TYPE_COUNT,
} ControllerType;

typedef struct ControlInputLim {
  float low;
  float high;
} ControlInputLim;

typedef struct ControlOutputLim {
  float min;
  float max;
} ControlOutputLim;

typedef struct Controller {
  ControllerType    type;
  ControlIfaceMux   set;
  ControlIfaceMux   ref;
  ControlIfaceMux   out;
  ControlInputLim  *inputLim;
  ControlOutputLim *outputLim;
  void             *params;
} Controller;

#define CONTROLLER_XMACRO(x)                                                                       \
  typedef struct Controller##x Controller##x;                                                      \
  extern int                   Controller##x##_update(Controller *self, float *err, float **out);  \
  extern int                   Controller##x##_initInternal(Controller##x *self);                  \
  extern int                   Controller##x##_clear(Controller *self);                            \
  inline int Controller##x##_init(Controller *self, uint32_t refMask, uint32_t setMask,            \
                                  uint32_t outMask, ControlIface *ref, ControlIface *set,          \
                                  ControlIface *out, ControlInputLim *inputLim,                    \
                                  ControlOutputLim *outputLim, void *params) {                     \
    self->type      = CONTROLLER_TYPE_##x;                                                         \
    self->ref.mask  = refMask;                                                                     \
    self->set.mask  = setMask;                                                                     \
    self->out.mask  = outMask;                                                                     \
    self->ref.iface = ref;                                                                         \
    self->set.iface = set;                                                                         \
    self->out.iface = out;                                                                         \
    self->inputLim  = inputLim;                                                                    \
    self->outputLim = outputLim;                                                                   \
    self->params    = params;                                                                      \
    return Controller##x##_initInternal((Controller##x *)self);                                    \
  }
ARES_CONTROLLER_XMACRO
#undef CONTROLLER_XMACRO

#define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))
extern inline int Controller_update(Controller *self) {
  float err[__builtin_popcount(self->ref.mask)];
  int   ret = 0;
  CTRL_INTERFACE_MUX_RD(&self->ref, feedback);
  CTRL_INTERFACE_MUX_RD(&self->set, set);
  CTRL_INTERFACE_MUX_WR_START(&self->out, out);

  FOR_RANGE(i, 0, ARRAY_LEN(err)) {
    err[i] = set[i] - feedback[i];
    if (self->inputLim) {
      ARES_ASSERT(self->inputLim[i].low <= self->inputLim[i].high,
                  "low(%d) should be less than high(%d)", self->inputLim[i].low,
                  self->inputLim[i].high);
      float range = self->inputLim[i].high - self->inputLim[i].low;
      while (err[i] < self->inputLim[i].low)
        err[i] += range;
      while (err[i] > self->inputLim[i].high)
        err[i] -= range;
    }
  }
  switch (self->type) {
#define CONTROLLER_XMACRO(x)                                                                       \
  case CONTROLLER_TYPE_##x: ret = Controller##x##_update(self, err, out);
    ARES_CONTROLLER_XMACRO
#undef CONTROLLER_XMACRO
  default: ret = ARES_NOT_IMPLEMENTED;
  }
  if (self->outputLim) {
    FOR_RANGE(i, 0, ARRAY_LEN(out)) {
      ARES_ASSERT(self->outputLim[i].min <= self->outputLim[i].max,
                  "min(%d) should be less than max(%d)", self->outputLim[i].min,
                  self->outputLim[i].max);
      *out[i] = CLAMP(*out[i], self->outputLim[i].min, self->outputLim[i].max);
    }
  }
  CTRL_INTERFACE_MUX_WR_END(&self->out);
  return ret;
};

extern inline int Controller_clear(Controller *self) {
  switch (self->type) {
#define CONTROLLER_XMACRO(x)                                                                       \
  case CONTROLLER_TYPE_##x: return Controller##x##_clear(self);
    ARES_CONTROLLER_XMACRO
#undef CONTROLLER_XMACRO
  default: return ARES_NOT_IMPLEMENTED;
  }
}

#endif