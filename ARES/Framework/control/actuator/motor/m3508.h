#ifndef M3508_H
#define M3508_H
#include "macro.h"

#define M3508_P19_GEAR_RATIO (3591.0f / 187.0f)
#define M3508_MAX_TORQ       (4.5f / M3508_P19_GEAR_RATIO) // Nm
#define M3508_MAX_CURR       (16384.0f)
#define M3508_MAX_VEL        (482.0f * M3508_P19_GEAR_RATIO * RPM2DPS)
#define M3508_FEEDBACK2TORQ  (M3508_MAX_TORQ / M3508_MAX_CURR)

#endif