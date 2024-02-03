#include "slist.h"

void SList_pushBack(SList *head, SList *item) {
  SList *cur = head;
  while (cur->next != NULL) {
    cur = cur->next;
  }
  item->next = NULL;
  cur->next  = item;
}

SList *SList_pushFront(SList *head, SList *item) {
  item->next = head;
  return item;
}