#ifndef ARES_SECTION_H
#define ARES_SECTION_H

#define AT_SECTION(sec) __attribute__((section(sec)))

extern unsigned int __pure_init_start;
extern unsigned int __pure_init_end;

extern unsigned int __device_init_start;
extern unsigned int __device_init_end;

extern unsigned int __post_os_init_start;
extern unsigned int __post_os_init_end;

extern unsigned int _etext;
extern unsigned int _edata;
extern unsigned int _sdata;
extern int          _siccmram;
extern int          _sccmram;
extern int          _eccmram;
#define IS_FUNCTION(x)                                                                                                        \
  (((void *)(x) > (void *)FLASH_BASE && (void *)(x) < (void *)&_etext) ||                                                    \
   ((void *)(x) >= (void *)&_sdata && (void *)(x) < (void *)&_edata))

#endif