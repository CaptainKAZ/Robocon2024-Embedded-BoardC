#ifndef RM_MOTOR_H
#define RM_MOTOR_H

#include "chrono.h"
#include "control/controller/pid.h"
#include "motor.h"
#include "can_al.h"

typedef enum MotorRMType {
  RM_MOTOR_M3508 = 0,
  RM_MOTOR_M2006,
} MotorRMType;

typedef struct MotorRM {
  Motor       motor;
  MotorRMType rmType;
  CanDev      bus;
  uint8_t     id;
  struct {
    float pos;
    float posAbs;
    float vel;
    float torq;
  } cmd;
  struct {
    float        pos;
    float        posAbs;
    float        vel;
    float        torq;
    float        temp;
    float        posOrigin;
    int32_t      turns;
    ChronoTick32 lastUpdate;
  } stat;
  float         cur2torq;
  float         torq2cur;
  ControllerPID velPid;
  ControllerPID posPid;
} MotorRM;

#define RM_MOTOR_ECD2DEG      (360.0f / 8192.0f)
#define RM_MOTOR_FRAME_HEAD_1 0x200
#define RM_MOTOR_FRAME_HEAD_2 0x1FF

/**
 * @brief init a MotorRM struct
 * 
 * @param self the motor to init
 * @param bus can bus dev
 * @param id motor can id
 * @param rm_type motor type (RM_MOTOR_M3508 or RM_MOTOR_M2006)
 * @param timeout try init timeout
 * @return int ARES_Erro
 *
 * @note this function should be called under thread context, block until can frame is received or timeout
 */
extern int MotorRM_register(MotorRM *motorRM, MotorRMType rm_type, CanDev *bus, uint8_t id,
                            uint32_t timeout);

#endif // RM_MOTOR_H