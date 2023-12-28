#ifndef MACRO_H
#define MACRO_H

#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) & ((TYPE *)0)->MEMBER)
#endif

#define container_of(ptr, type, member)                                                                                       \
  ({                                                                                                                          \
    const typeof(((type *)0)->member) *__mptr = (const typeof(((type *)0)->member) *)(ptr);                                   \
    (type *)((char *)__mptr - offsetof(type, member));                                                                        \
  })

#endif