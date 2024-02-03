#ifndef NVIC_AL_H
#define NVIC_AL_H
#include "main.h"
#include "slist.h"

typedef struct Nvic_IrqHandler{
  void (*isr_callback)(void *);
  void * param;
  SList list;
}Nvic_IrqHandler;

extern int Nvic_requestEOI(IRQn_Type irqn, Nvic_IrqHandler *handler);
extern int Nvic_requestSOI(IRQn_Type irqn, Nvic_IrqHandler *handler);
#endif