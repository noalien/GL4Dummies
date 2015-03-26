/*!\file vector.c
 * \brief Fonctions de gestion de vecteur.
 * \author Kevin HAVRANEK mzartek@hotmail.fr
 * \date Mars 17, 2015
 *
*/

#include "list.h"
#include <string.h>
#include <assert.h>

list_t list_new (size_t data_size) {
     list_t list;

     list.begin = malloc(sizeof *list.begin);
     list.end = list.begin;
     list.size = 0;
     list.data_size = data_size;

     list.begin->data = malloc(data_size);
     list.begin->next = NULL;
	  
     assert(list.begin);
     assert(list.begin->data);

     return list;
}

/* void list_delete (list_t list) { */
/* } */

/* void list_insert  (list_t *list, void *el, size_t index) { */
/* } */

/* void list_erase (list_t *list, size_t index) { */
/* } */

/* void list_set (list_t list, void *el, size_t index) { */
/* } */

/* void *list_get (list_t list, size_t index) { */
/* } */
