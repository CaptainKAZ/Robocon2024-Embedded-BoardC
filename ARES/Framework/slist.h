#ifndef SLIST_H
#define SLIST_H
#include "main.h"

struct SList {
  struct SList *next;
};
typedef struct SList SList;

inline void SList_init(SList *item) { item->next = NULL; }
void        SList_pushBack(SList *head, SList *item);
SList      *SList_pushFront(SList *head, SList *item);

#define Slist_foreach(item, slist) for (SList *item = slist; item != NULL; item = item->next)

#endif