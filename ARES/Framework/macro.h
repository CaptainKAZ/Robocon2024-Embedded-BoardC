#ifndef MACRO_H
#define MACRO_H

#include <stdint.h>
#include <stddef.h>
#define __must_be_array(x) __builtin_types_compatible_p(typeof(x), typeof(&x[0]))
#define ARRAY_LEN(x)       sizeof((x)) / sizeof((x)[0]) + __must_be_array(x)
#define FOR_ARRAY(item, x)                                                                         \
  for (register typeof((x)[0]) *(item) = (x); (item) < (x) + ARRAY_LEN(x); (item)++)
#define FOR_RANGE(idx, start, end) for (register size_t idx = start; idx < end; idx++)

typedef unsigned char bool_t;
typedef enum AresErrno {
  ARES_SUCCESS = 0,
  ARES_NO_RESOURCE,
  ARES_BAD_PARAM,
  ARES_DECODE_ERR,
  ARES_NOT_IMPLEMENTED,
  ARES_TIMEOUT,
  ERRNO_NUM,
} AresErrno;

#define memcpy __builtin_memcpy
#define memset __builtin_memset

#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) & ((TYPE *)0)->MEMBER)
#endif

#define container_of(ptr, type, member)                                                            \
  ({                                                                                               \
    const typeof(((type *)0)->member) *__mptr = (const typeof(((type *)0)->member) *)(ptr);        \
    (type *)((char *)__mptr - offsetof(type, member));                                             \
  })

#define likely(x)   __builtin_expect((x), 1)
#define unlikely(x) __builtin_expect((x), 0)

#define ARES_ASSERT(x, ...)                                                                        \
  {                                                                                                \
    if (unlikely(!(x))) {                                                                          \
      LOG_E("%s:%d "__VA_ARGS__, __FILE__, __LINE__);                                              \
      for (;;)                                                                                     \
        ;                                                                                          \
    }                                                                                              \
  }

#include "ares_section.h"

struct NamedFunc {
  int   (*callback)(void);
  char *name;
};

struct ArgedFunc {
  int   (*callback)(void *);
  void *arg;
};

#define NAMED_FUNC(function)                                                                       \
  { .callback = function, .name = #function }
#define ARGED_FUNC(function, argument)                                                             \
  { .callback = function, .arg = argument }

#define RUN_ARGED_FUNC(func)                                                                       \
  ({                                                                                               \
    ARES_ASSERT(IS_FUNCTION((func).callback), "try to run a wrong function pointer(0x%x)!",        \
                (func).callback);                                                                  \
    (func).callback(func.arg);                                                                     \
  })
#define RUN_NAMEND_FUNC(func)                                                                      \
  ({                                                                                               \
    ARES_ASSERT(IS_FUNCTION((func).callback), "try to run a wrong function pointer(0x%x)!",        \
                (func).callback);                                                                  \
    (func).callback();                                                                             \
  })

#define BITMASK(length) ((1 << (length)) - 1)
#define SIGN(x)         (((signed char *)&x)[sizeof(x) - 1] >> 7 | 1)
#define SIGNBIT(x)      (((signed char *)&x)[sizeof(x) - 1] >> 7)

#define RPM2DPS (6.0f)

#endif