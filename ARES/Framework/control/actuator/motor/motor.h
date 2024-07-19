#ifndef MOTOR_H
#define MOTOR_H
#include "ares_section.h"
#include "control/interface.h"
#include "control/controller/controller.h"
#include "macro.h"
#include <math.h>

/* 
 * composite CMD_POS_VEL type is commanding both absolute position and velocity
 * NOT have a genuine float instance, just for distinguishing control type
 * CMD_TORQ is the default control type
*/
CTRL_INTERFACE_DEF(MotorStdCmdIface, CMD_POS_VEL = 8, CMD_TORQ = 0, CMD_VEL, CMD_POS, CMD_POS_ABS);
#define MOTOR_STD_CMD_IFACE_SUPPORT_ALL                                                            \
  CTRL_INTERFACE_MASK(CMD_POS_VEL) | CTRL_INTERFACE_MASK(CMD_POS) |                                \
      CTRL_INTERFACE_MASK(CMD_POS_ABS) | CTRL_INTERFACE_MASK(CMD_VEL) |                            \
      CTRL_INTERFACE_MASK(CMD_TORQ)

/*
 * units:
 * STAT_POS: deg
 * STAT_POS_ABS: deg
 * STAT_VEL: deg/s
 * STAT_TORQ: Nm
 * STAT_TEMP: C
*/
CTRL_INTERFACE_DEF(MotorStdStatIface, STAT_POS, STAT_POS_ABS, STAT_VEL, STAT_TORQ, STAT_TEMP);
#define MOTOR_STD_STAT_IFACE_SUPPORT_ALL                                                           \
  CTRL_INTERFACE_MASK(STAT_POS) | CTRL_INTERFACE_MASK(STAT_POS_ABS) |                              \
      CTRL_INTERFACE_MASK(STAT_VEL) | CTRL_INTERFACE_MASK(STAT_TORQ) |                             \
      CTRL_INTERFACE_MASK(STAT_TEMP)

#define ARES_MOTOR_SUPPORT ARES_MOTOR_XMACRO(RM)

typedef enum MotorType {
  MOTOR_TYPE_UNKNOWN = 0,
#define ARES_MOTOR_XMACRO(x) MOTOR_TYPE_##x,
  ARES_MOTOR_SUPPORT
#undef ARES_MOTOR_XMACRO
      MOTOR_TYPE_COUNT,
} MotorType;

typedef enum MotorLifeCycle {
  MOTOR_LC_UNINIT = 0,   // default mode
  MOTOR_LC_LOSS,         // waiting for connection
  MOTOR_LC_UNCALIBRATED, // not calibrated
  MOTOR_LC_ENGAGED,      // engaged
} MotorLifeCycle;

typedef struct Motor {
  MotorType            type : 4;
  MotorLifeCycle       lifeCycle : 4;
  uint32_t             cmdSupportMask;
  uint32_t             statSupportMask;
  MotorStdCmdIfaceType cmdType;
  MotorStdCmdIface     cmd;
  MotorStdStatIface    stat;
  //TODO: add health check
} Motor;

extern inline float Motor_getPos(Motor *self) {
  if (self->statSupportMask & CTRL_INTERFACE_MASK(STAT_POS)) {
    return CTRL_INTERFACE_RD(&self->stat.iface, STAT_POS);
  }
  return NAN;
}

extern inline float Motor_getPosAbs(Motor *self) {
  if (self->statSupportMask & CTRL_INTERFACE_MASK(STAT_POS_ABS)) {
    return CTRL_INTERFACE_RD(&self->stat.iface, STAT_POS_ABS);
  }
  return NAN;
}

extern inline float Motor_getVel(Motor *self) {
  if (self->statSupportMask & CTRL_INTERFACE_MASK(STAT_VEL)) {
    return CTRL_INTERFACE_RD(&self->stat.iface, STAT_VEL);
  }
  return NAN;
}

extern inline float Motor_getTorq(Motor *self) {
  if (self->statSupportMask & CTRL_INTERFACE_MASK(STAT_TORQ)) {
    return CTRL_INTERFACE_RD(&self->stat.iface, STAT_TORQ);
  }
  return NAN;
}

extern inline int Motor_setVel(Motor *self, float vel) {
  if (self->cmdSupportMask & CTRL_INTERFACE_MASK(CMD_VEL)) {
    CTRL_INTERFACE_WR(&self->cmd.iface, CMD_VEL, vel);
    self->cmdType = CMD_VEL;
    return ARES_SUCCESS;
  }
  return ARES_NOT_IMPLEMENTED;
}

extern inline int Motor_setTorq(Motor *self, float torq) {
  if (self->cmdSupportMask & CTRL_INTERFACE_MASK(CMD_TORQ)) {
    CTRL_INTERFACE_WR(&self->cmd.iface, CMD_TORQ, torq);
    self->cmdType = CMD_TORQ;
    return ARES_SUCCESS;
  }
  return ARES_NOT_IMPLEMENTED;
}
extern inline int Motor_setPos(Motor *self, float pos) {
  if (self->cmdSupportMask & CTRL_INTERFACE_MASK(CMD_POS)) {
    CTRL_INTERFACE_WR(&self->cmd.iface, CMD_POS, pos);
    self->cmdType = CMD_POS;
    return ARES_SUCCESS;
  }
  return ARES_NOT_IMPLEMENTED;
}
extern inline int Motor_setPosAbs(Motor *self, float posAbs) {
  if (self->cmdSupportMask & CTRL_INTERFACE_MASK(CMD_POS_ABS)) {
    CTRL_INTERFACE_WR(&self->cmd.iface, CMD_POS_ABS, posAbs);
    self->cmdType = CMD_POS_ABS;
    return ARES_SUCCESS;
  }
  return ARES_NOT_IMPLEMENTED;
}

extern inline int Motor_setPosVel(Motor *self, float posAbs, float vel) {
  if (self->cmdSupportMask & CTRL_INTERFACE_MASK(CMD_POS_VEL)) {
    CTRL_INTERFACE_WR(&self->cmd.iface, CMD_POS_ABS, posAbs);
    CTRL_INTERFACE_WR(&self->cmd.iface, CMD_VEL, vel);
    self->cmdType = CMD_POS_VEL;
    return ARES_SUCCESS;
  }
  return ARES_NOT_IMPLEMENTED;
}
extern inline MotorStdCmdIface *Motor_exposeCmdIface(Motor *self, MotorStdCmdIfaceType type) {
  self->cmdType = type;
  return &self->cmd;
}
extern inline MotorStdStatIface *Motor_exposeStatIface(Motor *self) { return &self->stat; }

#define ARES_MOTOR_XMACRO(x) extern int Motor##x##_setPosOrigin(Motor *self);
ARES_MOTOR_SUPPORT
#undef ARES_MOTOR_XMACRO
extern inline int Motor_SetPosOrigin(Motor *self) {
  switch (self->type) {
#define ARES_MOTOR_XMACRO(x)                                                                       \
  case MOTOR_TYPE_##x: return Motor##x##_setPosOrigin(self);
    ARES_MOTOR_SUPPORT
#undef ARES_MOTOR_XMACRO
  default: return ARES_NOT_IMPLEMENTED;
  }
}

// extern int Motor_addTransmission(Motor *self, Transmission *transmission);
#endif // MOTOR_H