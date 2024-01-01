#ifndef CHRONO_H
#define CHRONO_H

#include "main.h"
#include "stdint.h"
#include "dwt_al.h"

typedef DwtTick ChronoTick;

extern ChronoTick Chrono_get(void);
extern float      Chrono_diff(ChronoTick start, ChronoTick end);

#define STOPWATCH_TIC(name) ChronoTick stopwatch_##name = Chrono_get()
#define STOPWATCH_TOC(name) Chrono_diff(stopwatch_##name, Chrono_get())
#define GET_STAMP() Chrono_diff(0, Chrono_get())

/* depending on different optimization level, offset can be different */
#define CHRONO_DELAY_OFFSET 5
#endif