#ifndef M2006_H
#define M2006_H
#include "macro.h"

#define M2006_P36_GEAR_RATIO (36.0f)
#define M2006_MAX_TORQ            (1.75f / M2006_P36_GEAR_RATIO) // Nm
#define M2006_MAX_CURR            (10000.0f)
#define M2006_MAX_VEL             (500.0f * M2006_P36_GEAR_RATIO * RPM2DPS)
#define M2006_FEEDBACK2TORQ       (M2006_MAX_TORQ / M2006_MAX_CURR)

#endif