#ifndef ARES_SECTION_H
#define ARES_SECTION_H

#define AT_SECTION(sec) __attribute__((section(sec)))
#define INIT_CALL(stage) __attribute__((__used__)) AT_SECTION(".init." #stage)

#define PURE_INIT INIT_CALL(pure_init)
#define DEVICE_INIT INIT_CALL(device_init)
#define POST_OS_INIT INIT_CALL(post_os_init)

extern unsigned int __pure_init_start;
extern unsigned int __pure_init_end;

extern unsigned int __device_init_start;
extern unsigned int __device_init_end;

extern unsigned int __post_os_init_start;
extern unsigned int __post_os_init_end;

extern unsigned int _etext;
#define IS_IN_SECTION_TEXT(x) ((x) > 0 && (x) < &_etext)

#endif