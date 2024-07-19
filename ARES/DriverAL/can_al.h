#ifndef CANAL_H
#define CANAL_H
#include "macro.h"
#include "slist.h"
#include "initcall.h"

#define CONFIG_CAN_TX_SOFT_FIFO_LEN                                                                \
  3 // the max number of messages that can be stored in the soft fifo (hard fifo is 3)

typedef enum CanDev {
  INTERNAL_CAN1 = 0,
  INTERNAL_CAN2,
  // EXTERNAL_CAN1,
  // EXTERNAL_CAN2,
  CAN_DEV_NUM
} CanDev;

typedef enum {
  CAN_FRAME_UNKNOWN,
  CAN_FRAME_STD,
  CAN_FRAME_EXT,
} CanFrameType;

typedef enum CanFrameDir {
  CAN_DIR_UNKNOWN,
  CAN_DIR_TX,
  CAN_DIR_RX,
} CanFrameDir;

#pragma pack(1)
typedef struct CanFrame {
  CanDev       bus : 4;
  CanFrameDir  dir : 2;
  CanFrameType type : 2;
  uint8_t      len;
  uint32_t     id;
  uint8_t      data[8];
} CanFrame;
#pragma pack()

typedef struct CanRxHook {
  int   (*hook)(CanFrame *frame);
  sList entry;
} CanRxHook;

extern int CanHw_tx(CanFrame *frame);
extern int Can_rx(CanFrame *frame);
extern int Can_registerRxHook(CanRxHook *hook);
#define Can_registerRxHookStatic(func)                                                             \
  static CanRxHook __##func##_hook = {.hook = func};                                               \
  static int __Can_registerRxHook_##func(void) { return Can_registerRxHook(&__##func##_hook); }    \
  Initcall_registerPostOs(__Can_registerRxHook_##func)

#endif