/*!\file list.c
 * \brief Fonctions de gestion de liste.
 * \author Kevin HAVRANEK mzartek@hotmail.fr
 * \date Mars 17, 2015
 */

#include "list.h"
#include <string.h>
#include <assert.h>

/**
 *  Créé une liste vide d’éléments de taille \p data_size et renvoie un pointeur
 *  sur ladite liste
 *
 *  \param data_size
 *  \return Retourne un pointeur sur la liste créée
 */
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

/**
 *  Supprime chaque élément de la liste \p list avant de détruire la liste
 *  elle-même.\n
 *  Attention ! Si les éléments stockés doivent être détruits via un destructeur
 *  spécial, ce dernier ne sera pas exécuté lors de l’appel de cette fonction !
 *
 *  \param list
 */
void list_delete(list_t list) {
  struct _list_t *current, *next;

  for (current = list.begin; current != NULL; current = next) {
    next = current->next;
    free(current->data);
    free(current);
  }
}

/**
 *  Insère un nouvel élément \p el dans la liste \p list à l’index \p index,
 *  avant l’élément se trouvant précédemment à ce même index (ce dernier et les
 *  éléments suivant subissant alors un décalage d’un rang). L’élément \p el est
 *  copié dans une nouvelle cellule de la liste, et l’élément \p el passé en
 *  paramètre et l’élément dans la liste à la fin de l’exécution de cette
 *  fonction ne sont donc pas le même objet.
 *
 *  \param list Liste dans laquelle est insérée le nouvel élément
 *  \param el Nouvel élément à insérer
 *  \param index Index auquel insérer le nouvel élément
 */
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

  if (prev != NULL) {
    prev->next = new_ptr;
  } else {
    list->begin = new_ptr;
  }

  new_ptr->next = current;
}

/**
 *  Supprime un élément de la liste \p list situé à l’index \p index. Chaque
 *  élément suivant l’élément supprimé verra donc son index décrémenté
 *  de 1.\n
 *  Attention ! Si les éléments stockés doivent être détruits via un destructeur
 *  spécial, ce dernier ne sera pas exécuté lors de l’appel de cette fonction !
 *
 *  \param list Liste dans laquelle l’élément doit être détruit
 *  \param index Index de l’élément devant être détruit
 */
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

/**
 *  Cette fonction remplace la valeur de l’élément de la liste \p list situé à
 *  l’index \p index par la valeur de l’élément \p el qui sera copié.\n
 *  Attention ! Si l’élément précédent doit être détruit via un destructeur
 *  spécial, ce dernier ne sera pas exécuté via cette fonction !
 *
 *  \param list Liste dans laquelle l’élément doit être modifié
 *  \param el Élément dont les valeurs seront copiées
 *  \param index Index de l’élément à modifier
 */
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

/**
 *  \brief function description
 *
 *  Cette fonction renvoie un pointeur vers l’élément situé à l’index \p index
 *  dans la liste \p list.
 *
 *  \param list Liste contenant l’élément souhaité
 *  \param index Index de l’élément souhaité
 *  \return Renvoie un pointeur vers l’élément d’index \p index
 */
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
