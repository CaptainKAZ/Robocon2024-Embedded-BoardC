#ifndef SLIST_H
#define SLIST_H
#include "main.h"

struct sList {
  struct sList *next;
};
typedef struct sList sList;

inline void SList_init(sList *item) { item->next = NULL; }

inline void SList_pushBack(sList *head, sList *item) {
  sList *cur = head;
  while (cur->next != NULL) {
    cur = cur->next;
  }
  item->next = NULL;
  cur->next  = item;
}

inline sList *SList_pushFront(sList *head, sList *item) {
  item->next = head;
  return item;
}

#define Slist_foreach(item, slist)                                                                 \
  for (register sList *item = slist; item != NULL; item = item->next)

#endif