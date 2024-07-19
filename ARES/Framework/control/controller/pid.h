#ifndef PID_H
#define PID_H
#include "controller.h"
#include <stdint.h>
#include "chrono.h"
#include "portmacro.h"

typedef struct ControllerPIDParam {
  struct {
    float kP;
  } p;
  struct {
    enum {
      INTEGRATOR_NORMAL    = 0,
      INTEGRATOR_CLAMP     = 1 << 1,
      INTEGRATOR_BACK_CALC = 1 << 2,
      INTEGRATOR_VAR_SPEED = 1 << 3,
    } integrator;
    float kI;
    float clampIMax; // clamp interator max
    float clampIMin; // clamp interator min
    float coeffA;    // var speed interator coeffA
    float coeffB;    // var speed interator coeffB
    float kC;        // back calc interator kC
  } i;
  struct {
    float kD;
    float N; // d filter(cut off frequency), 0 means disable
  } d;
  float timeout;
} ControllerPIDParam;

struct ControllerPID {
  Controller controller;
  TickType_t lastTick;
  float      pOut;
  float      iOut;
  float      dOut[2]; // dOut and last dOut
  float      dInt;
  float      out[2]; // out and last out
  float      err[2]; // err and last err
};

#endif