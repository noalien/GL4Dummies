/*!\file vector.c
 * \brief Fonctions de gestion de vecteur.
 * \author Kevin HAVRANEK mzartek@hotmail.fr
 * \date Mars 17, 2015
 *
 */

#include "vector.h"
#include <string.h>
#include <assert.h>

vector_t vector_new(size_t data_size) {
	vector_t vector;

	vector.data = malloc(data_size);
	vector.size = 0;
	vector.data_size = data_size;
	vector.limit_size = 1;

	assert(vector.data);

	return vector;
}

void vector_delete(vector_t vector) {
	free(vector.data);
}

void vector_insert(vector_t *vector, void *el, size_t index) {
	size_t old_size = vector->size;

	vector->size = (index < vector->size) ? (vector->size + 1) : (index + 1);

	if (vector->size > vector->limit_size) {
		while (vector->size > vector->limit_size) {
			vector->limit_size <<= 1;
		}
		vector->data = realloc(vector->data, vector->data_size * vector->limit_size);
		assert(vector->data);
	}

	char *data = (char *)vector->data;

	if (index < old_size) {
		size_t mem_size = vector->data_size * (old_size - index);
		void *buffer = malloc(mem_size);

		memcpy(buffer, &data[vector->data_size * index], mem_size);
		memcpy(&data[vector->data_size * (index + 1)], buffer, mem_size);

		free(buffer);
	}
	memcpy(&data[vector->data_size * index], el, vector->data_size);
}

void vector_erase(vector_t *vector, size_t index) {
	char *data = (char *)vector->data;

	assert(index < vector->size);

	vector->size--;

	size_t mem_size = vector->data_size * (vector->size - index);
	void *buffer = malloc(mem_size);

	memcpy(buffer, &data[vector->data_size * (index + 1)], mem_size);
	memcpy(&data[vector->data_size * index], buffer, mem_size);

	free(buffer);
}

void vector_set(vector_t vector, void *el, size_t index) {
	char *data = (char *)vector.data;

	assert(index < vector.size);

	memcpy(&data[vector.data_size * index], el, vector.data_size);
}

void *vector_get(vector_t vector, size_t index) {
	char *data = (char *)vector.data;

	assert(index < vector.size);

	return (void *)&data[vector.data_size * index];
}
