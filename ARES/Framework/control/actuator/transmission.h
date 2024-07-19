/**
 * @file transmission.h
 * @author proton
 * @brief a bi-directional mapping form actuators to actual axis
 * @version 0.1
 * @date 2024-05-03
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef TRANSMISSION_H
#define TRANSMISSION_H
#include "control/interface.h"
#include "motor/motor.h"

#define ARES_TRANSMISSION TRANSMISSION_XMACRO(Simple)

#define TRANSMISSION_XMACRO(name)                                                                  \
  typedef struct Transmission##name Transmission##name;                                            \
  int Transmission##name##_init(Transmission##name *self, ControlIface *cmdIn,                     \
                                ControlIface *cmdOut, void *param);
ARES_TRANSMISSION
#undef TRANSMISSION_XMACRO

typedef enum TransmissionType {
  TRANSMISSION_TYPE_UNKNOWN = 0,
#define TRANSMISSION_XMACRO(name) TRANSMISSION_TYPE_##name
  ARES_TRANSMISSION
#undef TRANSMISSION_XMACRO
} TransmissionType;

typedef struct Transmission {
  TransmissionType type;
  ControlIface    *cmdIn;
  ControlIface    *cmdOut;
  ControlIface    *statIn;
  ControlIface    *statOut;
  void            *param;
} Transmission;

struct TransmissionSimple {
  Transmission transmission;
  MotorStdCmdIface cmdOut;
  MotorStdStatIface statOut;
};

#endif