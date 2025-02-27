#ifndef CHRONO_H
#define CHRONO_H

#include "main.h"
#include "stdint.h"
#include "dwt_al.h"

typedef DwtTick64 ChronoTick64;
/**
 * @brief 32-bit timestamp tick type, NOT corresponding to any physical time unit
 */
typedef DwtTick ChronoTick32;

extern ChronoTick64 InitalTick64;

extern float        Chrono_diff32(ChronoTick32 start, ChronoTick32 end);
extern float        Chrono_diff64(ChronoTick64 *start, ChronoTick64 *end);

#define Chrono_get32 Dwt_get
#define Chrono_get64 Dwt_get64
#define CHRONO64_NOW (ChronoTick64 *)NULL

#define DWT_OVERFLOW_DELAY_TOLERANCE (10U)
/* depending on different optimization level, offset can be different */
#define CHRONO_DELAY_OFFSET (6U)
extern void Chrono_usdelayOs(uint32_t us);
/**
 * @brief provide a name and start the named 32 bit stopwatch
 * @note the elapsed time should be under UINT32_MAX/SYSCLK_FREQ otherwise it overflows
 */
#define STOPWATCH32_TIC(name) ChronoTick32 stopwatch_##name = Chrono_get32()
/**
 * @brief stop the named 32 bit stopwatch and return the elapsed time in second
 * @note the elapsed time should be under UINT32_MAX/SYSCLK_FREQ otherwise it overflows
 */
#define STOPWATCH32_TOC(name) Chrono_diff32(stopwatch_##name, Chrono_get32())

/**
 * @brief provide a name and start the named 64 bit stopwatch
 * @note takes more time than STOPWATCH32_TICK but will not overflow
 */
#define STOPWATCH64_TIC(name) ChronoTick64 stopwatch_##name = Chrono_get64()

/**
 * @brief stop the named 64 bit stopwatch and return the elapsed time in second
 * @note takes more time than STOPWATCH32_TOC but will not overflow
 */
#define STOPWATCH64_TOC(name) Chrono_diff64(&stopwatch_##name, CHRONO64_NOW)

#define GET_STAMP() Chrono_diff64(&InitalTick64, CHRONO64_NOW)

#endif