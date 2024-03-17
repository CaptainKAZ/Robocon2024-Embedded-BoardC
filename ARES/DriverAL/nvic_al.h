#ifndef NVIC_AL_H
#define NVIC_AL_H
#include "main.h"
#include "slist.h"
#include "macro.h"

typedef struct Nvic_IrqHandler{
  struct ArgedFunc func;
  SList list;
}Nvic_IrqHandler;

extern int Nvic_requestEOI(IRQn_Type irqn, Nvic_IrqHandler *handler);
extern int Nvic_requestSOI(IRQn_Type irqn, Nvic_IrqHandler *handler);
#endif