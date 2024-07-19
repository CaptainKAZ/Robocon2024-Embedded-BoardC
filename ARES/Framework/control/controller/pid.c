#include "pid.h"

void ControllerPID_Clear(ControllerPID *self) {
  self->err[0] = self->err[1] = self->err[2] = 0;
  self->dOut[0] = self->dOut[1] = self->iOut = self->pOut = 0;
  self->out[1] = self->out[0] = 0;
  self->dInt                  = 0;
  self->lastTick              = Chrono_get32();
}

int ControllerPID_initInternal(ControllerPID *self) {
  ControllerPID_Clear(self);
  return ARES_SUCCESS;
}

int ControllerPID_update(Controller *self, float *err, float **out) {
  ControllerPID      *pid   = (ControllerPID *)self;
  ControllerPIDParam *param = self->params;
  TickType_t          now   = Chrono_get32();
  float               dt    = Chrono_diff32(pid->lastTick, now);
  if (param->timeout && dt > param->timeout) {
    ControllerPID_Clear(pid);
    return ARES_TIMEOUT;
  }
  pid->lastTick = now;
  pid->err[1]   = pid->err[0];
  pid->err[0]   = err[0];
  pid->out[1]   = pid->out[0];

  pid->pOut   = param->p.kP * pid->err[0];
  float iTerm = param->i.kI * 0.5f * (pid->err[0] + pid->err[1]) * dt; // trapezoidal
  if (param->i.integMask & INTEG_VAR_SPEED) {
    if (pid->err[0] * pid->iOut > 0) {
      // Integral still increasing
      float absErr = __builtin_fabsf(pid->err[0]);
      if (absErr <= (param->i.coeffA + param->i.coeffB))
        iTerm *= (param->i.coeffA - absErr + param->i.coeffB) / param->i.coeffA;
      else
        iTerm = 0;
    }
  }
  if (param->i.integMask & INTEG_BACK_CALC) {
    if (pid->out[1] > self->outputLim[0].max)
      iTerm += param->i.kC * (self->outputLim[0].max - pid->out[1]);
    else if (pid->out[1] < self->outputLim[0].min)
      iTerm += param->i.kC * (self->outputLim[0].min - pid->out[1]);
  }
  if (param->i.integMask & INTEG_CLAMP) {
    if (pid->out[1] > self->outputLim[0].max || pid->out[1] < self->outputLim[0].min) {
      if (pid->err[0] * pid->iOut > 0) {
        iTerm = 0;
      }
    }
    float tempIOut = pid->iOut + iTerm;
    pid->iOut      = CLAMP(tempIOut, param->i.clampIMin, param->i.clampIMax);
  } else {
    pid->iOut += iTerm;
  }
  if (param->d.N != 0) {
    pid->dOut[1] = pid->dOut[0];
    pid->dOut[0] = param->d.kD * (param->d.N * pid->err[0] - pid->dInt);
    pid->dInt += 0.5f * dt * (pid->dOut[0] + pid->dOut[1]);
  } else {
    if (dt > 0.0001) {
      pid->dOut[0] = pid->dOut[1] = param->d.kD * (pid->err[0] - pid->err[1]) / dt;
    } else {
      pid->dOut[0] = pid->dOut[1] = 0;
    }
  }

  pid->out[0] = pid->pOut + pid->dOut[0] + pid->iOut;
  *out[0]     = pid->out[0];
  return ARES_SUCCESS;
}