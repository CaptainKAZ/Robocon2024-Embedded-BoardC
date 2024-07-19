#ifndef SQUEUE_H
#define SQUEUE_H

#include "log.h"
#include "macro.h"
#include <stdint.h>
#include <stdlib.h>

#define CONFIG_SQUEUE_SIZE_BITS 7

typedef struct {
  uint8_t front;
  uint8_t rear;
  uint8_t size;
  uint8_t full : 8 - CONFIG_SQUEUE_SIZE_BITS;
  uint8_t itemSize : CONFIG_SQUEUE_SIZE_BITS;
  void   *buf;
} sQueue;

inline void SQueue_init(sQueue *q, uint8_t itemSize, void *buf, uint8_t size) {
  ARES_ASSERT(q != NULL, "queue is null");
  ARES_ASSERT(buf != NULL, "queue buf is null");
  ARES_ASSERT(size < BITMASK(CONFIG_SQUEUE_SIZE_BITS), "queue size is too large");
  q->front    = 0;
  q->rear     = 0;
  q->full     = 0;
  q->size     = size;
  q->itemSize = itemSize;
  q->buf      = buf;
}

inline int SQueue_push(sQueue *q, void *item) {
  ARES_ASSERT(q != NULL, "queue is null");
  ARES_ASSERT(q->buf != NULL, "queue buf is null");
  if (q->full) {
    return ARES_NO_RESOURCE;
  }
  memcpy((char *)q->buf + (q->rear * q->itemSize), item, q->itemSize);
  q->rear = (q->rear + 1) % q->size;
  if (q->rear == q->front) {
    q->full = 1;
  }
  return ARES_SUCCESS;
}

inline int SQueue_pop(sQueue *q, void *item) {
  ARES_ASSERT(q != NULL, "queue is null");
  ARES_ASSERT(q->buf != NULL, "queue buf is null");
  if (q->front == q->rear && !q->full) {
    return ARES_NO_RESOURCE;
  }
  memcpy(item, (char *)q->buf + (q->front * q->itemSize), q->itemSize);
  q->front = (q->front + 1) % q->size;
  q->full  = 0;
  return ARES_SUCCESS;
}

#endif