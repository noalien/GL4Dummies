/*!\file vector.h
 * \brief Fonctions de gestion de vecteur.
 * \author Kevin HAVRANEK mzartek@hotmail.fr
 * \date Mars 17, 2015
 *
*/

#ifndef _VECTOR_H
#define _VECTOR_H

#include "gl4dummies.h"

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

     typedef struct vector_t vector_t;

     struct vector_t {
	  void *data;
	  size_t size;
	  size_t data_size;
	  size_t limit_size;
     };

     GL4DAPI vector_t GL4DAPIENTRY vector_new (size_t data_size);
     GL4DAPI void     GL4DAPIENTRY vector_delete (vector_t vector);
     GL4DAPI void     GL4DAPIENTRY vector_insert (vector_t *vector, void *el, size_t index);
     GL4DAPI void     GL4DAPIENTRY vector_erase (vector_t *vector, size_t index);
     GL4DAPI void     GL4DAPIENTRY vector_set (vector_t vector, void *el, size_t index);
     GL4DAPI void*    GL4DAPIENTRY vector_get (vector_t vector, size_t index);

#ifdef __cplusplus
}
#endif

#endif
