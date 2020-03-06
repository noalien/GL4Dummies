/*!\file fixed_heap.h
 * \brief gestion de tas d'éléments à taille fixe.
 * \author Farès BELHADJ amsi@ai.univ-paris8.fr
 * \date November 30, 2016
 *
*/

#ifndef _FIXED_HEAP_H

#define _FIXED_HEAP_H
#include "gl4du.h"

# ifdef __cplusplus
extern "C" {
# endif

  GL4DAPI size_t GL4DAPIENTRY fheapCreate(size_t nmem, size_t size);
  GL4DAPI size_t GL4DAPIENTRY fheapPut(size_t heap_id, void * element);
  GL4DAPI void * GL4DAPIENTRY fheapGet(size_t heap_id, size_t element_id);
  GL4DAPI void   GL4DAPIENTRY fheapDelete(size_t heap_id, size_t element_id);
  GL4DAPI void   GL4DAPIENTRY fheapDestroy(size_t heap_id);
  GL4DAPI void   GL4DAPIENTRY fheapClean(void);

# ifdef __cplusplus
}
# endif

#endif
