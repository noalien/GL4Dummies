/*!\file linked_list.c
 * \brief Fonctions de gestion de listes chainees.
 * \author Fares BELHADJ amsi@ai.univ-paris8.fr
 * \date January 16, 2006 - June 02, 2010 - February 22, 2016
 * \todo commenter
 */

#include "linked_list.h"
#include <stdlib.h>
#include <assert.h>

linked_list_t * llNew(void) {
  linked_list_t * ll = malloc(sizeof * ll);
  assert(ll);
  ll->head = NULL;
  ll->tail = &(ll->head);
  return ll;
}

void llPushBack(linked_list_t * ll, void * data) {
  *(ll->tail) = malloc(sizeof ** (ll->tail));
  assert(*(ll->tail));
  (*(ll->tail))->data = data;
  (*(ll->tail))->next = NULL;
  ll->tail = &((*(ll->tail))->next);
}

void llPush(linked_list_t * ll, void * data) {
  llnode_t * next = ll->head;
  ll->head = malloc(sizeof * ll->head);
  assert(ll->head);
  ll->head->data = data;
  ll->head->next = next;
  if(!next)
    ll->tail = &(ll->head->next);
}

void * llPop(linked_list_t * ll) {
  void * data = ll->head->data;
  llDelete(ll, &(ll->head), NULL);
  return data;
}

int llEmpty(linked_list_t * ll) {
  return ll->tail == &(ll->head);
}

pair_t llInsert(linked_list_t * ll, void * data, int (*compar)(const void * newData, const void * nodeData)) {
  pair_t res = llFind(ll, data, compar);
  if(res.compResult)
    llInsertAt((llnode_t **)res.ptr, data);
  return res;
}

void llInsertAt(llnode_t ** here, void * data) {
  llnode_t * next = *here;
  *here = malloc(sizeof ** here);
  assert(*here);
  (*here)->data = data;
  (*here)->next = next;
}

pair_t llFind(linked_list_t * ll, const void * data, int (*compar)(const void * newData, const void * nodeData)) {
  pair_t res = {(void **)&(ll->head), -1};
  while(*(llnode_t **)res.ptr) {
    if((res.compResult = compar(data, (*(llnode_t **)res.ptr)->data)) <= 0)
      return res;
    else
      res.ptr = (void **)&((*(llnode_t **)res.ptr)->next);
  }
  return res;
}

void llDelete(linked_list_t * ll, llnode_t ** node, void (*freeData)(void *)) {
  llnode_t * todelete = *node;
  if(ll->tail == &((*node)->next))
    ll->tail = node;
  *node = (*node)->next;
  if(freeData)
    freeData(todelete->data);
  free(todelete);
}

llnode_t ** llNext(llnode_t ** node) {
  return &((*node)->next);
}

llnode_t ** llFirst(linked_list_t * ll) {
  return &(ll->head);
}

llnode_t ** llEnd(linked_list_t * ll) {
  return ll->tail;
}

void llFree(linked_list_t * ll, void (*freeData)(void *)) {
  llnode_t * ptr = ll->head, * next;
  if(freeData)
    while(ptr) {
      next = ptr->next;
      freeData(ptr->data);
      free(ptr);
      ptr = next;
    }
  else
    while(ptr) {
      next = ptr->next;
      free(ptr);
      ptr = next;
    }
  free(ll);
}
