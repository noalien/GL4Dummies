/*!\file bin_tree.c
 * \brief Fonctions de gestion d'arbres binaires.
 * \author Fares BELHADJ amsi@ai.univ-paris8.fr
 * \date January 16, 2006 - June 01, 2010
 *
*/

#include "bin_tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

pair_t btInsert(bin_tree_t ** tree, void * data, int (*compar)(const void * newData, const void * nodeData)) {
  bin_tree_t ** next = *tree ? (*tree)->next : NULL /* correspond au parent du dernier fils gauche */;
  pair_t res = {(void **)tree, -1};
  while(*((bin_tree_t **)res.ptr)) {
    if((res.compResult = compar(data, (*((bin_tree_t **)res.ptr))->data)) < 0) {
      next = (bin_tree_t **)res.ptr;
      res.ptr = (void **)&((*((bin_tree_t **)res.ptr))->lc);
    } else if(res.compResult > 0) {
      res.ptr = (void **)&((*((bin_tree_t **)res.ptr))->rc);
    } else
      return res;
  }
  (*((bin_tree_t **)res.ptr)) = calloc(1, sizeof (bin_tree_t));
  assert(*res.ptr);
  (*((bin_tree_t **)res.ptr))->data = data;
  (*((bin_tree_t **)res.ptr))->next = next;
  return res;
}

pair_t btFind(bin_tree_t ** tree, const void * data, int (*compar)(const void * newData, const void * nodeData)) {
  pair_t res = {(void **)tree, -1};
  while(*((bin_tree_t **)res.ptr)) {
    if((res.compResult = compar(data, (*((bin_tree_t **)res.ptr))->data)) < 0)
      res.ptr = (void **)&((*((bin_tree_t **)res.ptr))->lc);
    else if(res.compResult > 0)
      res.ptr = (void **)&((*((bin_tree_t **)res.ptr))->rc);
    else
      return res;
  }
  return res;
}

bin_tree_t ** btFirst(bin_tree_t ** ptr) {
  bin_tree_t ** f = ptr;
  while(*ptr) {
    f = ptr;
    ptr = &((*ptr)->lc);
  }
  return f;
}

bin_tree_t ** btLast(bin_tree_t ** ptr) {
  bin_tree_t ** l = ptr;
  while(*ptr) {
    l = ptr;
    ptr = &((*ptr)->rc);
  }
  return l;
}

void btDelete(bin_tree_t ** ptr, void (*freeData)(void *)) {
  bin_tree_t * todelete = *ptr, ** ptr2 = ptr, * lrc;
  if(todelete->lc) {
    *ptr = todelete->lc;
    (*ptr)->next = todelete->next;
    lrc = (*ptr)->lc;
    while(lrc) {
      lrc->next = ptr;
      lrc = lrc->rc;
    }
    while((*ptr2)->rc) {
      (*ptr2)->rc->next = todelete->next;
      ptr2 = &((*ptr2)->rc);
    }
    (*ptr2)->rc = todelete->rc;
    if(todelete->rc) {
      lrc = todelete->rc->lc;
      while(lrc) {
	lrc->next = &((*ptr2)->rc);
	lrc = lrc->rc;
      }
    }
  } else {
    *ptr = todelete->rc;
    if(todelete->rc) {
      lrc = todelete->rc->lc;
      while(lrc) {
    	lrc->next = ptr;
    	lrc = lrc->rc;
      }
    }
  }
  if(freeData)
    freeData(todelete->data);
  free(todelete);
}

bin_tree_t ** btNext(bin_tree_t ** ptr) {
  return (*ptr)->rc ? btFirst(&((*ptr)->rc)) : (*ptr)->next;
}

void btFree(bin_tree_t ** tree, void (*freeData)(void *)) {
  if(!(*tree)) return;
  if((*tree)->lc) btFree(&((*tree)->lc), freeData);
  if((*tree)->rc) btFree(&((*tree)->rc), freeData);
  freeData((*tree)->data);
  free(*tree);
  (*tree) = NULL;
}

void btForAll(bin_tree_t * ptr, void (*todo)(void *, void **), void ** ldata) {
  if(ptr->lc)
    btForAll(ptr->lc, todo, ldata);
  todo(ptr->data, ldata);
  if(ptr->rc)
    btForAll(ptr->rc, todo, ldata);
}

