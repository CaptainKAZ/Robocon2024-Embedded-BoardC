#ifndef MACRO_H
#define MACRO_H

typedef enum {
  ARES_SUCCESS = 0,
  ARES_NO_RESORCE,
  ARES_DECODE_ERR,
  ERRNO_NUM,
} Errno;

#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) & ((TYPE *)0)->MEMBER)
#endif

#define container_of(ptr, type, member)                                                                                       \
  ({                                                                                                                          \
    const typeof(((type *)0)->member) *__mptr = (const typeof(((type *)0)->member) *)(ptr);                                   \
    (type *)((char *)__mptr - offsetof(type, member));                                                                        \
  })

#define likely(x) __builtin_expect((x), 1)
#define unlikely(x) __builtin_expect((x), 0)

#endif