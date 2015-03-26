/*!\file list.h
 * \brief Fonctions de gestion de liste.
 * \author Kevin HAVRANEK mzartek@hotmail.fr
 * \date Mars 17, 2015
 *
*/

#ifndef _LIST_H
#define _LIST_H

#include "gl4dummies.h"

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

     typedef struct list_t list_t;

     struct _list_t {
	  void *data;
	  struct _list_t *next;
     };

     struct list_t {
	  struct _list_t *begin;
	  size_t size;
	  size_t data_size;
     };

     GL4DAPI list_t   GL4DAPIENTRY list_new (size_t data_size);
     GL4DAPI void     GL4DAPIENTRY list_delete (list_t list);
	 GL4DAPI void     GL4DAPIENTRY list_insert(list_t *list, void *el, size_t index);
     GL4DAPI void     GL4DAPIENTRY list_erase (list_t *list, size_t index);
     GL4DAPI void     GL4DAPIENTRY list_set (list_t list, void *el, size_t index);
     GL4DAPI void*    GL4DAPIENTRY list_get (list_t list, size_t index);

#ifdef __cplusplus
}
#endif

#endif
