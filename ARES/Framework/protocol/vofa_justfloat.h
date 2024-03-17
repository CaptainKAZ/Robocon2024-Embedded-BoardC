/**
 * @file vofa_justfloat.h
 * @author proton
 * @brief header-only warpper for vofa justfloat protocol
 * @version 0.1
 * @date 2024-03-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef VOFA_JUSTFLOAT_H
#define VOFA_JUSTFLOAT_H
#include <stdint.h>
#include <string.h>
#include "macro.h"
#include "main.h"

const static uint8_t justFloat_tail[4] = {0x00,0x00,0x80,0x7f};

typedef struct {
  uint8_t *buf;
  size_t   pos;
  size_t   size;
} JustFloatFrame;

inline void JustFloat_init(JustFloatFrame *frame, void *buf, size_t size) {
  frame->buf  = buf;
  frame->pos  = 0;
  frame->size = size;
}

inline int JustFloat_addFloat(JustFloatFrame *frame, float to_add) {
  if (frame->pos + sizeof(float) + ARRAY_LEN(justFloat_tail) >= frame->size) {
    return ARES_NO_RESORCE;
  }
  memcpy(&frame->buf[frame->pos], &to_add, sizeof(float)); // Little Endian promised
  frame->pos += sizeof(float);
  return ARES_SUCCESS;
}

inline size_t JustFloat_pack(JustFloatFrame *frame){
  if (frame->pos + ARRAY_LEN(justFloat_tail) >= frame->size || frame->pos==0){
    return 0;
  }
  memcpy(&frame->buf[frame->pos], justFloat_tail, sizeof(justFloat_tail));
  frame->pos += sizeof(justFloat_tail);
  return frame->pos;
}

#endif