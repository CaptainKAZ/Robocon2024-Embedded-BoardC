#ifndef INTERFACE_H
#define INTERFACE_H
#include "chrono.h"
#include "macro.h"
#include <stdint.h>

typedef struct ControlIface {
  void         (*update)(struct ControlIface *self); // calling update should update stamp
  ChronoTick32 stamp;                                // timestamp of last update
  uint32_t     dim;
  float       *data[0];
} ControlIface;

typedef struct ControlIfaceMux {
  uint32_t      mask;
  ControlIface *iface;
} ControlIfaceMux;

#define CTRL_INTERFACE_DEF(iface_name, ...)                                                        \
  typedef enum { __VA_ARGS__, iface_name##_DIM } iface_name##Type;                                 \
  typedef struct iface_name {                                                                      \
    ControlIface iface;                                                                            \
    float       *data[iface_name##_DIM];                                                           \
  } iface_name;
#define CTRL_INTERFACE_DIM(iface_name) iface_name##_DIM
#define CTRL_INTERFACE_INITIALIZER(iface_name, updateFunc)                                         \
  {                                                                                                \
    .iface = {.dim = iface_name##_DIM, .update = (updateFunc) }                                    \
  }
#define CTRL_INTERFACE_MASK(x) (1 << (x))

#define CTRL_INTERFACE_RD(iface, idx)                                                              \
  ({                                                                                               \
    if (IS_FUNCTION((iface)->update))                                                              \
      (iface)->update(iface);                                                                      \
    *(iface)->data[idx];                                                                           \
  })

#define CTRL_INTERFACE_WR(iface, idx, value)                                                       \
  ({                                                                                               \
    *(iface)->data[idx] = value;                                                                   \
    (iface)->stamp      = Chrono_get32();                                                          \
  })

#define CTRL_INTERFACE_RD_MASK(iface, mask, array)                                                 \
  float array[__builtin_popcount((mask))];                                                         \
  if ((iface)->update)                                                                             \
    (iface)->update(iface);                                                                        \
  FOR_RANGE(i, 0, sizeof(uint32_t) * CHAR_BIT) {                                                   \
    if ((mask) & (1 << i))                                                                         \
      array[i] = *(iface)->data[i];                                                                \
  }

#define CTRL_INTERFACE_WR_START_MASK(iface, mask, array)                                           \
  float *array[__builtin_popcount((mask))];                                                        \
  FOR_RANGE(i, 0, sizeof(uint32_t) * CHAR_BIT) {                                                   \
    if ((mask) & (1 << i))                                                                         \
      array[i] = (iface)->data[i];                                                                 \
  }

#define CTRL_INTERFACE_WR_END_MASK(iface) iface->stamp = Chrono_get32()

#define CTRL_INTERFACE_MUX_RD(ifaceMux, array)                                                     \
  CTRL_INTERFACE_RD_MASK((ifaceMux)->iface, (ifaceMux)->mask, array)
#define CTRL_INTERFACE_MUX_WR_START(ifaceMux, array)                                               \
  CTRL_INTERFACE_WR_START_MASK((ifaceMux)->iface, (ifaceMux)->mask, array)

#define CTRL_INTERFACE_MUX_WR_END(ifaceMux) CTRL_INTERFACE_WR_END_MASK((ifaceMux)->iface)

#endif