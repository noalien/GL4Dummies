/*!\file vector.c
 * \brief Fonctions de gestion de vecteur.
 * \author Kevin HAVRANEK mzartek@hotmail.fr
 * \date Mars 17, 2015
 *
 */

#include "vector.h"
#include <string.h>
#include <assert.h>

/**
 *  Créé un nouveau vecteur et renvoie l'objet créé.
 *
 *  \param data_size Taille des éléments à conserver dans le vecteur
 *  \return Retourne le nouveau vecteur créé de type \ref vector_t
 */
vector_t vector_new(size_t data_size) {
	vector_t vector;

	vector.data = malloc(data_size);
	vector.size = 0;
	vector.data_size = data_size;
	vector.limit_size = 1;

	assert(vector.data);

	return vector;
}

/**
 *  Supprime toutes les données du vecteur, puis détruit le vecteur lui-même.
 *
 *  \param vector Vecteur à supprimer
 */
void vector_delete(vector_t vector) {
	free(vector.data);
}

/**
 * Insère l'élément `el` dans le vecteur `vector` à l'index spécifié. Si
 * l'index pointe vers un objet déjà existant, ledit objet et les objets
 * suivants seront décalés d'une case dans le vecteur, et l'élément `el` sera
 * inséré à l'index `index`. Si l'argument `index` est supérieur à l'index
 * maximal initial, alors l'élément sera inséré à la fin du vecteur. Si la
 * capacité du vecteur n'est pas suffisamment importante pour ajouter un
 * élément, une réallocation mémoire sera effectuée, invalidant d'éventuels
 * pointeurs sur les données conservées dans le vecteur.
 *
 *  \param vector Vecteur dans lequel nous souhaitons insérer un nouvel élément
 *  \param el Élément que l'on souhaite insérer dans le vecteur
 *  \param index Index auquel nous souhaitons insérer le nouvel élément.
 */
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

/**
 *  Supprime l'élément d'un vecteur situé à l'index spécifié
 *
 *  \param vector Vecteur duquel on souhaite supprimer un élément
 *  \param index Index auquel se situe l'élément que l'on souhaite supprimer
 */
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

/**
 *  Remplace la valeur de l'élément du vecteur `vector` à l'index `index` par la
 * valeur de l'élément `el`.
 *
 *  \param vector Vecteur dans lequel on souhaite changer la valeur d'un élément
 *  \param el Élément dont la valeur sera copiée dans l'élément du vecteur
 * spécifié \param index Index dans le vecteur où se trouve l'élément dont on
 * souhaite changer la valeur
 */
void vector_set(vector_t vector, void *el, size_t index) {
	char *data = (char *)vector.data;

	assert(index < vector.size);

	memcpy(&data[vector.data_size * index], el, vector.data_size);
}

/**
 *  Renvoie un pointeur vers l'élément se situant à l'index `index` dans le vecteur `vector`
 *
 *  \param vector Vecteur dans lequel se situe l'élément souhaité
 *  \param index Index du vecteur où se trouve l'élément souhaité
 *  \return Pointeur vers l'élément souhaité
 */
void *vector_get(vector_t vector, size_t index) {
	char *data = (char *)vector.data;

	assert(index < vector.size);

	return (void *)&data[vector.data_size * index];
}
