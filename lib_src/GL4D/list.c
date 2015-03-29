/*!\file list.c
 * \brief Fonctions de gestion de liste.
 * \author Kevin HAVRANEK mzartek@hotmail.fr
 * \date Mars 17, 2015
 *
 */

#include "list.h"
#include <string.h>
#include <assert.h>

list_t list_new(size_t data_size) {
	list_t list;

	list.begin = malloc(sizeof *list.begin);
	list.size = 0;
	list.data_size = data_size;

	list.begin->data = malloc(data_size);
	list.begin->next = NULL;

	assert(list.begin);
	assert(list.begin->data);

	return list;
}

void list_delete(list_t list) {
	struct _list_t *current, *next;

	for (current = list.begin; current != NULL; current = next) {
		next = current->next;
		free(current->data);
		free(current);
	}
}

void list_insert(list_t *list, void *el, size_t index) {
	struct _list_t *current, *prev, *new_ptr;
	size_t i;

	list->size = (index < list->size) ? (list->size + 1) : (index + 1);

	prev = NULL;
	current = list->begin;
	for (i = 0; i < index; i++) {
		if (current->next == NULL) {
			current->next = malloc(sizeof *current->next);
			current->next->data = malloc(list->data_size);
			current->next->next = NULL;

			assert(current->next);
			assert(current->next->data);
		}

		prev = current;
		current = current->next;
	}

	new_ptr = malloc(sizeof *new_ptr);
	new_ptr->data = malloc(list->data_size);

	assert(new_ptr);
	assert(new_ptr->data);

	memcpy(new_ptr->data, el, list->data_size);

	if (prev != NULL)
		prev->next = new_ptr;
	else
		list->begin = new_ptr;

	new_ptr->next = current;
}

void list_erase(list_t *list, size_t index) {
	struct _list_t *current, *prev;
	size_t i;

	assert(index < list->size);

	list->size--;

	prev = NULL;
	current = list->begin;
	for (i = 0; i < index; i++) {
		prev = current;
		current = current->next;
	}

	if (prev != NULL)
		prev->next = current->next;
	else
		list->begin = current->next;

	free(current->data);
	free(current);
}

void list_set(list_t list, void *el, size_t index) {
	struct _list_t *current;
	size_t i;

	assert(index < list.size);

	current = list.begin;
	for (i = 0; i < index; i++) {
		current = current->next;
	}

	memcpy(current->data, el, list.data_size);
}

void *list_get (list_t list, size_t index) {
	struct _list_t *current;
	size_t i;

	assert(index < list.size);

	current = list.begin;
	for (i = 0; i < index; i++) {
		current = current->next;
	}

	return current->data;
}
