/*!\file linked_list.h
 * \brief Fonctions de gestion de listes chainees.
 * \author Fares BELHADJ amsi@ai.univ-paris8.fr
 * \date January 16, 2006 - June 02, 2010 - February 22, 2016
 */

#ifndef _LINKED_LIST_H

#define _LINKED_LIST_H

#include "gl4dummies.h"

# ifdef __cplusplus
extern "C" {
# endif

typedef struct llnode_t llnode_t;
typedef struct linked_list_t linked_list_t;

#ifndef STRUCT_PAIR_T
# define STRUCT_PAIR_T
  typedef struct pair_t pair_t;
  struct pair_t {
    void ** ptr;
    int compResult;
  };
#endif
  
struct llnode_t {
  void * data;
  struct llnode_t * next;
};

struct linked_list_t {
  struct llnode_t *  head;
  struct llnode_t ** tail;
};

  GL4DAPI linked_list_t * GL4DAPIENTRY llNew      (void);
  GL4DAPI void            GL4DAPIENTRY llPushBack (linked_list_t * ll, void * data);
  GL4DAPI void            GL4DAPIENTRY llPush     (linked_list_t * ll, void * data);
  GL4DAPI void *          GL4DAPIENTRY llPop      (linked_list_t * ll);
  GL4DAPI int             GL4DAPIENTRY llEmpty    (linked_list_t * ll);
  GL4DAPI pair_t          GL4DAPIENTRY llInsert   (linked_list_t * ll, void * data, int (*compar)(const void * newData, const void * nodeData));
  GL4DAPI void            GL4DAPIENTRY llInsertAt (llnode_t **  here, void * data);
  GL4DAPI pair_t          GL4DAPIENTRY llFind     (linked_list_t * ll, const void * data, int (*compar)(const void * newData, const void * nodeData));
  GL4DAPI void            GL4DAPIENTRY llDelete   (linked_list_t * ll, llnode_t ** node, void (__cdecl *freeData)(void *));
  GL4DAPI llnode_t **     GL4DAPIENTRY llNext     (llnode_t ** node);
  GL4DAPI llnode_t **     GL4DAPIENTRY llFirst    (linked_list_t * ll);
  GL4DAPI llnode_t **     GL4DAPIENTRY llEnd      (linked_list_t * ll);
  GL4DAPI void            GL4DAPIENTRY llFree     (linked_list_t * ll, void (*freeData)(void *));

# ifdef __cplusplus
}
# endif

#endif
