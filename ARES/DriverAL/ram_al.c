#include "ram_al.h"
#include "stddef.h"
#include "initcall.h"
#include "main.h"

int Ram_init(void) {
  memset((void *)CCMDATARAM_BASE, 0, CCMDATARAM_END - CCMDATARAM_BASE);
  size_t len = (void *)&_eccmram - (void *)&_sccmram;
  memcpy(&_sccmram, &_siccmram, len);
  return 0;
}

Initcall_registerPure(Ram_init);