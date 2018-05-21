/*!\file list.h
 * \brief Fonctions de gestion de liste.
 * \author Kevin HAVRANEK mzartek@hotmail.fr
 * \date Mars 17, 2015
 */

#ifndef _LIST_H
#define _LIST_H

#include "gl4dummies.h"

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

  /*
   * \typedef list_t
   * `list_t` agit comme alias pour `struct list_t`
   */
  typedef struct list_t list_t;

  /*
   * \struct _list_t
   * \brief Wrapper pour les éléments de \ref list_t
   *
   * Contient un pointeur vers l’élément stocké par le wrapper et un pointeur vers
   * le prochain élément
   */
  struct _list_t {
    void *data;           /*!< Élément stocké */
    struct _list_t *next; /*!< Pointeur vers le prochain élément */
  };

  /**
   * \struct list_t
   * \brief Liste chaînée dynamique
   *
   * Wrapper de liste chaînée dynamique
   */
  struct list_t {
    struct _list_t *begin; /*!< premier élément de la liste */
    size_t size;           /*!< Nombre d’éléments dans la chaîne */
    size_t data_size;      /*!< Taille d’un élément */
  };

  /// \brief Créé une nouvelle liste
  GL4DAPI list_t GL4DAPIENTRY list_new(size_t data_size);

  /// \brief Détruit une liste
  GL4DAPI void GL4DAPIENTRY list_delete(list_t list);

  /// \brief Insère un élément dans une liste
  GL4DAPI void GL4DAPIENTRY list_insert(list_t *list, void *el, size_t index);

  /// \brief Supprime un élément d’une liste
  GL4DAPI void GL4DAPIENTRY list_erase(list_t *list, size_t index);

  /// \brief Modifie un élément dans une liste
  GL4DAPI void GL4DAPIENTRY list_set(list_t list, void *el, size_t index);

  /// \brief Retourne un pointeur vers un élément d’une liste
  GL4DAPI void *GL4DAPIENTRY list_get(list_t list, size_t index);

#ifdef __cplusplus
}
#endif

#endif
