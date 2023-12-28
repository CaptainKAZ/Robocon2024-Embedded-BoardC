#include "initcall.h"
#include "log.h"
#include <stdint.h>

int do_pure_initcall() {
  int ret = 0;
  for (InitCall *initCall = (void *)&__pure_init_start; initCall < (InitCall *)&__pure_init_end; initCall++) {
    ret = initCall->func();
    if (ret != 0) {
      LOG_E("pure init call %s failed", initCall->name);
    }
  }
  return 0;
}

int do_device_initcall() {
  int ret = 0;
  for (InitCall *initCall = (void *)&__device_init_start; initCall < (InitCall *)&__device_init_end; initCall++) {
    ret = initCall->func();
    if (ret != 0) {
      LOG_E("device init call %s failed", initCall->name);
    }
  }
  return 0;
}

int do_post_os_initcall() {
  int ret = 0;
  for (InitCall *initCall = (void *)&__post_os_init_start; initCall < (InitCall *)&__post_os_init_end; initCall++) {
    ret = initCall->func();
    if (ret != 0) {
      LOG_E("post os init call %s failed", initCall->name);
    }
  }
  return 0;
}