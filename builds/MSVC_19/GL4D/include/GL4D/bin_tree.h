/*!\file bin_tree.h
 * \brief Fonctions de gestion d'arbres binaires.
 * \author Fares BELHADJ amsi@ai.univ-paris8.fr
 * \date January 16, 2006
 *
*/

#ifndef _BIN_TREE_H

#define _BIN_TREE_H
#include "gl4dummies.h"

# ifdef __cplusplus
extern "C" {
# endif

  typedef struct bin_tree_t bin_tree_t;

#ifndef STRUCT_PAIR_T
# define STRUCT_PAIR_T
  typedef struct pair_t pair_t;
  struct pair_t {
    void ** ptr;
    int compResult;
  };
#endif
  
  struct bin_tree_t {
    void * data;
    struct bin_tree_t * lc, * rc, ** next;
  };
  
  GL4DAPI pair_t        GL4DAPIENTRY btInsert (bin_tree_t ** tree, void * data, int (*compar)(const void * newData, const void * nodeData));
  GL4DAPI pair_t        GL4DAPIENTRY btFind   (bin_tree_t ** tree, const void * data, int (*compar)(const void * newData, const void * nodeData));
  GL4DAPI bin_tree_t ** GL4DAPIENTRY btFirst  (bin_tree_t ** ptr);
  GL4DAPI bin_tree_t ** GL4DAPIENTRY btLast   (bin_tree_t ** ptr);
  GL4DAPI void          GL4DAPIENTRY btDelete (bin_tree_t ** ptr, void (GL4DAPIENTRY *freeData)(void *));
  GL4DAPI bin_tree_t ** GL4DAPIENTRY btNext   (bin_tree_t ** ptr);
  GL4DAPI void          GL4DAPIENTRY btFree   (bin_tree_t ** tree, void (GL4DAPIENTRY  *freeData)(void *));
  GL4DAPI void          GL4DAPIENTRY btForAll (bin_tree_t * ptr, void (*todo)(void *, void **), void ** ldata);

# ifdef __cplusplus
}
# endif

#endif
