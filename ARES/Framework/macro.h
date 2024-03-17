#ifndef MACRO_H
#define MACRO_H

#define ARRAY_LEN(x) sizeof((x)) / sizeof((x)[0])

typedef enum {
  ARES_SUCCESS = 0,
  ARES_NO_RESORCE,
  ARES_DECODE_ERR,
  ERRNO_NUM,
} Errno;

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

#define ARES_ASSERT(x, message)                                                                    \
  {                                                                                                \
    if (unlikely(!(x))) {                                                                          \
      LOG_E(message);                                                                              \
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
    ARES_ASSERT(IS_FUNCTION((func).callback), "try to run a wrong function pointer!");             \
    (func).callback(func.arg);                                                                     \
  })
#define RUN_NAMEND_FUNC(func)                                                                      \
  ({                                                                                               \
    ARES_ASSERT(IS_FUNCTION((func).callback), "try to run a wrong function pointer!");             \
    (func).callback();                                                                             \
  })

#endif