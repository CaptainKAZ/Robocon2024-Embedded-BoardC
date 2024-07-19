#include "rm_motor.h"
#include "can_al.h"
#include "chrono.h"
#include "cmsis_os2.h"
#include "control/actuator/motor/motor.h"
#include "control/controller/controller.h"
#include "control/controller/pid.h"
#include "control/interface.h"
#include "m2006.h"
#include "m3508.h"
#include "macro.h"
#include <stdint.h>

static MotorRM *MotorRM_Id2Motor[CAN_DEV_NUM][8]  = {0};
static uint8_t  MotorRM_waitMask[CAN_DEV_NUM]     = {0};
static uint8_t  MotorRM_ReceivedMask[CAN_DEV_NUM] = {0};

static ControlInputLim  MotorRM_angleInputLim      = {.low = -180.0f, .high = 180.0f};
static ControlOutputLim MotorRM_m2006VelOutputLim  = {.min = -M2006_MAX_VEL, .max = M2006_MAX_VEL};
static ControlOutputLim MotorRM_m3508VelOutputLim  = {.min = -M3508_MAX_VEL, .max = M3508_MAX_VEL};
static ControlOutputLim MotorRM_m2006TorqOutputLim = {.min = -M2006_MAX_TORQ,
                                                      .max = M2006_MAX_TORQ};
static ControlOutputLim MotorRM_m3508TorqOutputLim = {.min = -M3508_MAX_TORQ,
                                                      .max = M3508_MAX_TORQ};

static ControllerPIDParam MotorRM_defaultVelPidParam = {
    .p       = {.kP = 0.01f},
    .i       = {.kI = 0.0f, .integrator = 0},
    .d       = {.kD = 0.0f, .N = 0.0f},
    .timeout = 0.1f,
};

static ControllerPIDParam MotorRM_defaultPosPidParam = {
    .p       = {.kP = 0.5f},
    .i       = {.kI = 0.0f, .integrator = 0},
    .d       = {.kD = 0.0f, .N = 0.0f},
    .timeout = 0.1f,
};

int MotorRM_register(MotorRM *self, MotorRMType rmType, CanDev *bus, uint8_t id, uint32_t timeout) {
  memset(self, 0, sizeof(MotorRM));

  self->bus = *bus;
  self->id  = id;

  self->motor.type            = MOTOR_TYPE_RM;
  self->motor.lifeCycle       = MOTOR_LC_LOSS;
  self->motor.cmdSupportMask  = MOTOR_STD_CMD_IFACE_SUPPORT_ALL;
  self->motor.statSupportMask = MOTOR_STD_STAT_IFACE_SUPPORT_ALL;

  self->motor.cmdType                     = CMD_TORQ;
  self->motor.cmd.iface.update            = NULL;
  self->motor.cmd.iface.dim               = MotorStdCmdIface_DIM;
  self->motor.cmd.iface.data[CMD_TORQ]    = &self->cmd.torq;
  self->motor.cmd.iface.data[CMD_VEL]     = &self->cmd.vel;
  self->motor.cmd.iface.data[CMD_POS]     = &self->cmd.pos;
  self->motor.cmd.iface.data[CMD_POS_ABS] = &self->cmd.posAbs;

  self->motor.stat.iface.update             = NULL;
  self->motor.stat.iface.dim                = MotorStdStatIface_DIM;
  self->motor.stat.iface.data[STAT_POS]     = &self->stat.pos;
  self->motor.stat.iface.data[STAT_POS_ABS] = &self->stat.posAbs;
  self->motor.stat.iface.data[STAT_VEL]     = &self->stat.vel;
  self->motor.stat.iface.data[STAT_TORQ]    = &self->stat.torq;
  self->motor.stat.iface.data[STAT_TEMP]    = &self->stat.temp;

  self->rmType = rmType;
  ControlOutputLim *velOutputLim;
  ControlOutputLim *torqOutputLim;
  switch (rmType) {
  case RM_MOTOR_M3508:
    self->cur2torq = M3508_FEEDBACK2TORQ;
    velOutputLim   = &MotorRM_m3508VelOutputLim;
    torqOutputLim  = &MotorRM_m3508TorqOutputLim;
    break;
  case RM_MOTOR_M2006:
    self->cur2torq = M2006_FEEDBACK2TORQ;
    velOutputLim   = &MotorRM_m2006VelOutputLim;
    torqOutputLim  = &MotorRM_m2006TorqOutputLim;
    break;
  default: ARES_ASSERT(0, "invalid rm type");
  }
  self->torq2cur = 1 / self->cur2torq;

  ControllerPID_init(&(self->velPid.controller), CTRL_INTERFACE_MASK(STAT_VEL),
                     CTRL_INTERFACE_MASK(CMD_VEL), CTRL_INTERFACE_MASK(CMD_TORQ),
                     &self->motor.stat.iface, &self->motor.cmd.iface, &self->motor.cmd.iface, NULL,
                     torqOutputLim, &MotorRM_defaultVelPidParam);

  ControllerPID_init(&(self->posPid.controller), CTRL_INTERFACE_MASK(STAT_POS),
                     CTRL_INTERFACE_MASK(CMD_POS), CTRL_INTERFACE_MASK(CMD_VEL),
                     &self->motor.stat.iface, &self->motor.cmd.iface, &self->motor.cmd.iface,
                     &MotorRM_angleInputLim, velOutputLim, &MotorRM_defaultPosPidParam);

  ARES_ASSERT(MotorRM_Id2Motor[self->bus][self->id] == NULL, "motor already registered");
  MotorRM_Id2Motor[self->bus][self->id] = self;
  while (timeout > 0) {
    if (self->motor.lifeCycle > MOTOR_LC_LOSS) {
      return ARES_SUCCESS;
    }
    osDelay(1);
    timeout--;
  }
  return ARES_TIMEOUT;
}

int MotorRM_setPosOrigin(Motor *self) {
  ARES_ASSERT(self->type == MOTOR_TYPE_RM, "not RM motor");
  MotorRM *motorRM        = container_of(self, MotorRM, motor);
  motorRM->stat.posOrigin = motorRM->stat.pos;
  motorRM->stat.turns     = 0;
  motorRM->stat.posAbs    = 0;
  motorRM->stat.pos       = 0;
  return ARES_SUCCESS;
}

static int MotorRM_parseFrame(MotorRM *self, CanFrame *frame) {
  ARES_ASSERT(self->motor.type == MOTOR_TYPE_RM, "not RM motor");
  self->stat.lastUpdate = Chrono_get32();
  float lastPos         = self->stat.pos;

  self->stat.pos = (float)((uint16_t)((frame->data[0]) << 8 | (frame->data[1]))) * RM_MOTOR_ECD2DEG;
  self->stat.vel = (float)((uint16_t)((frame->data[2]) << 8 | (frame->data[3]))) * RPM2DPS;
  self->stat.torq =
      self->cur2torq
          ? ((float)((uint16_t)((frame->data[4]) << 8 | (frame->data[5]))) * self->cur2torq)
          : ((float)(uint16_t)((frame->data[4]) << 8 | (frame->data[5])));
  self->stat.temp = (float)(frame->data[6]);

  if (self->motor.lifeCycle == MOTOR_LC_LOSS) {
    self->motor.cmdType = CMD_TORQ;
    Controller_clear((Controller *)&self->velPid);
    Controller_clear((Controller *)&self->posPid);
    self->stat.posOrigin  = self->stat.pos;
    lastPos               = 0;
    self->stat.turns      = 0;
    self->motor.lifeCycle = MOTOR_LC_UNCALIBRATED;
  }

  /* calc turns and abs pos */
  self->stat.pos -= self->stat.posOrigin;
  if (self->stat.pos - lastPos > 180.0f) {
    self->stat.turns--;
  } else if (self->stat.pos - lastPos < -180.0f) {
    self->stat.turns++;
  }
  self->stat.posAbs = self->stat.pos + self->stat.turns * 360.0f;

  return ARES_SUCCESS;
}

static int16_t MotorRM_update(MotorRM *self) {
  ARES_ASSERT(self->motor.type == MOTOR_TYPE_RM, "not RM motor");
  uint32_t clearMask      = MOTOR_STD_CMD_IFACE_SUPPORT_ALL;
  float    velFeedForward = 0;
  int16_t  output         = 0;
  // clear all none cmd signals
  switch (self->motor.cmdType) {
  case CMD_TORQ ... CMD_POS_ABS:
    CLEAR_BIT(clearMask, CTRL_INTERFACE_MASK(self->motor.cmdType));
    break;
  case CMD_POS_VEL:
    velFeedForward = CTRL_INTERFACE_RD(&self->motor.cmd.iface, CMD_VEL);
    CLEAR_BIT(clearMask, CTRL_INTERFACE_MASK(CMD_POS));
    break;
  default: break;
  }
  FOR_RANGE(i, 0, MotorStdCmdIface_DIM) {
    if (READ_BIT(clearMask, CTRL_INTERFACE_MASK(i)) && self->motor.cmd.iface.data[i] != NULL) {
      self->motor.cmd.iface.data[i] = 0;
    }
  }
  switch (self->motor.cmdType) {
  case CMD_POS_VEL:
  case CMD_POS_ABS: {
    self->posPid.controller.inputLim = NULL;
    self->posPid.controller.ref.mask = CTRL_INTERFACE_MASK(STAT_POS_ABS);
    self->posPid.controller.set.mask = CTRL_INTERFACE_MASK(CMD_POS_ABS);
    goto calcPosPID;
  }
  case CMD_POS: {
    self->posPid.controller.inputLim = &MotorRM_angleInputLim;
    self->posPid.controller.ref.mask = CTRL_INTERFACE_MASK(STAT_POS);
    self->posPid.controller.set.mask = CTRL_INTERFACE_MASK(CMD_POS);
  }
  calcPosPID:
    Controller_update(&self->posPid.controller);
    if (self->motor.cmdType == CMD_POS_VEL)
      self->cmd.vel += velFeedForward;
  case CMD_VEL: Controller_update(&self->velPid.controller);
  case CMD_TORQ: output = self->torq2cur * self->cmd.torq; break;
  default: ARES_ASSERT(0, "invalid cmd type");
  }
  return output;
}

static int MotorRM_canHook(CanFrame *frame) {
  if (!(frame->id >= 0x201 && frame->id <= 0x208)) {
    return ARES_DECODE_ERR;
  }
  uint8_t  id      = frame->id - 0x201;
  MotorRM *motorRM = MotorRM_Id2Motor[frame->bus][id];
  if (!motorRM) {
    return ARES_NOT_IMPLEMENTED;
  }
  int ret = MotorRM_parseFrame(motorRM, frame);
  if (ret == ARES_SUCCESS) {
    SET_BIT(MotorRM_ReceivedMask[frame->bus], id);
  }
  return ret;
}
Can_registerRxHookStatic(MotorRM_canHook);