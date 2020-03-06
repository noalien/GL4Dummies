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

  /**
   *  \typedef vector_t
   *  `vector_t` agit comme un alias pour `struct vector_t`
   */
  typedef struct vector_t vector_t;

  /**
   *  \struct vector_t
   *  \brief Tableau dynamique (vecteur)
   *
   *  Vecteur similaire au `vector` du C++, offrant la possibilité de stocker
   * dans un tableau des éléments de tout type.
   */
  struct vector_t {
    void *data; /*!< Pointeur sur le premier élément du tableau du vecteur */
    size_t size; /*!< Taille du tableau */
    size_t data_size; /*!< Taille des objets stockés dans le vecteur */
    size_t limit_size; /*!< Taille maximale du tableau */
  };

  /// \brief Créé un nouvel objet \ref vector_t
  GL4DAPI vector_t GL4DAPIENTRY vector_new(size_t data_size);

  /// \brief Détruit un objet de type \ref vector_t
  GL4DAPI void GL4DAPIENTRY vector_delete(vector_t vector);

  /// \brief Insère dans un vecteur un élément à l'index spécifié
  GL4DAPI void GL4DAPIENTRY vector_insert(vector_t *vector, void *el,
                                          size_t index);

  /// \brief Supprime un élément à l'index spécifié d'un vecteur
  GL4DAPI void GL4DAPIENTRY vector_erase(vector_t *vector, size_t index);

  /// \brief Remplace l'élément spécifié d'un vecteur par une autre valeur
  GL4DAPI void GL4DAPIENTRY vector_set(vector_t vector, void *el, size_t index);

  /// \brief Renvoie l'élément d'un vecteur se situant à un index spécifié
  GL4DAPI void *GL4DAPIENTRY vector_get(vector_t vector, size_t index);

#ifdef __cplusplus
}
#endif

#endif
