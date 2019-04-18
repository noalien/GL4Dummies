/*!\file fixed_heap.c
 * \brief gestion de tas d'éléments à taille fixe.
 * \author Farès BELHADJ amsi@ai.univ-paris8.fr
 * \date November 30, 2016
 *
*/

#include "fixed_heap.h"
#include <stdlib.h>
#include <assert.h>

typedef struct fheap_t fheap_t;

struct fheap_t {
  size_t nmem, size;
  char * heap;
  size_t * stack, head;
};

static fheap_t _newheap(size_t nmem, size_t size);
static void    _moreheap(fheap_t * h);
static inline void   _push(size_t * stack, size_t * head, size_t value);
static inline size_t _pop(size_t * stack, size_t * head);
static inline int    _empty(size_t head);

static fheap_t _heap = { 16, sizeof _heap, NULL, NULL, 0 };

/*!\brief créé un tas de \a nmem éléments de taille \a size et
 * retourne son id (<>0).
 * \param nmem nombre initial d'éléments dans le tas.
 * \param size taille, en octets, d'éléments du tas.
 * \return identifiant du tas créé (différent de 0).
 */
size_t fheapCreate(size_t nmem, size_t size) {
  fheap_t * h;
  size_t heap_id;
  static int ft = 1;
  if(_heap.heap == NULL) {
    _heap = _newheap(_heap.nmem, _heap.size);
    if(ft) gl4duAtExit(fheapClean);
    ft = 0;
  }
  heap_id = _pop(_heap.stack, &(_heap.head));
  if(_empty(_heap.head))
    _moreheap(&_heap);
  h = &(((fheap_t * )_heap.heap)[heap_id - 1]);
  *h =_newheap(nmem, size);
  return heap_id;
}

/*!\brief insert, en copiant le contenu, un élément dans le tas et
 * retourne son id (<>0).
 * \param heap_id identifiant du tas dans lequel se passe l'insertion.
 * \param element pointeur vers la donnée à insérer (copier).
 * \return identifiant de l'élément inséré dans le tas (différent de 0).
 */
size_t fheapPut(size_t heap_id, void * element) {
  fheap_t * h = &(((fheap_t * )_heap.heap)[heap_id - 1]);
  size_t element_id = _pop(h->stack, &(h->head));
  if(_empty(h->head))
    _moreheap(h);
  memcpy(&(h->heap[(element_id - 1) * h->size]), element, h->size);
  return element_id;
}

/*!\brief récupère un élément du tas et retourne le pointeur vers la
 * donnée. Attention, cette fonction n'efface pas la donnée du tas.
 * \param heap_id identifiant du tas duquel est extraite la donnée.
 * \param element_id identifiant de la donnée à extraire.
 * \return pointeur vers la donnée extraite.
 */
void * fheapGet(size_t heap_id, size_t element_id) {
  fheap_t * h = &(((fheap_t * )_heap.heap)[heap_id - 1]);
  return &(h->heap[--element_id * h->size]);
}

/*!\brief libère une donnée du tas (étiquette une donnée du tas comme
 * espace libre).
 * \param heap_id identifiant du tas duquel la donnée est libérée.
 * \param element_id identifiant de la donnée à libérer.
 */
void fheapDelete(size_t heap_id, size_t element_id) {
  fheap_t * h = &(((fheap_t * )_heap.heap)[heap_id - 1]);
  _push(h->stack, &(h->head), --element_id);
}

/*!\brief libère l'ensemble du tas et sa mémoire.
 * \param heap_id identifiant du tas à libérer.
 */
void fheapDestroy(size_t heap_id) {
  fheap_t * h = &(((fheap_t * )_heap.heap)[heap_id - 1]);
  free(h->heap); 
  h->heap = NULL;
  free(h->stack);
  h->stack = NULL;
  _push(_heap.stack, &(_heap.head), heap_id - 1);
}

/*!\brief libère tous les tas créés par cette bibliothèque.
 */
void fheapClean(void) {
  size_t i;
  fheap_t * h;
  if(!_heap.heap) return;
  for(i = 0; i < _heap.nmem; i++) {
    h = &(((fheap_t * )_heap.heap)[i]);
    if(h->heap)
      fheapDestroy(i + 1);
  }
  free(_heap.heap);
  _heap.heap = NULL;
  free(_heap.stack);
  _heap.stack = NULL;
  _heap.head = 0;
  _heap.size = 16;
}

static fheap_t _newheap(size_t nmem, size_t size) {
  size_t i;
  fheap_t h;
  h.nmem = nmem;
  h.size = size;
  h.heap = calloc(nmem, size * sizeof *(h.heap));
  assert(h.heap);
  h.stack = malloc(nmem * sizeof *(h.stack));
  assert(h.stack);
  h.head = 0;
  for(i = 0; i < nmem; ++i)
    _push(h.stack, &(h.head), nmem - 1 - i);
  return h;
}

static void _moreheap(fheap_t * h) {
  size_t i, onmem = h->nmem;
  h->nmem <<= 1;
  h->heap = realloc(h->heap, h->nmem * h->size * sizeof *(h->heap));
  assert(h->heap);
  memset(&(h->heap[(h->nmem >> 1) * h->size]), 0, (h->nmem >> 1) * h->size);
  h->stack = realloc(h->stack, h->nmem * sizeof *(h->stack));
  assert(h->stack);
  h->head = 0;
  for(i = onmem; i < h->nmem; ++i)
    _push(h->stack, &(h->head), onmem + h->nmem - 1 - i);
}

static inline void _push(size_t * stack, size_t * head, size_t value) {
  stack[(*head)++] = value;
}

static inline size_t _pop(size_t * stack, size_t * head) {
  return stack[--(*head)] + 1;
}

static inline int _empty(size_t head) {
  return head == 0;
}

/* Tests standalone de la lib */
#ifdef SA_FH /* SA_FH = Stand Alone Fixed Heap */
#include <stdio.h>
size_t unObj(size_t hid) {
  static int i = 0;
  char t[16] = "Hello world 0 !";
  t[12] += i;
  i++;
  return fheapPut(hid, t);
}

int main(void) {
  size_t hId = 0, a, b, c, d, i;
  a = fheapCreate(10, 16);
  b = unObj(a);
  c = unObj(a);
  d = unObj(a);
  printf("%s\n", (char *)fheapGet(a, b));
  printf("%s\n", (char *)fheapGet(a, c));
  printf("%s\n", (char *)fheapGet(a, d));
  fheapDestroy(a);
  for(i = 0; i < 10245; i++) {
    hId = fheapCreate(10, 14);
  }
  printf("%zd\n", hId);
  fheapDestroy(hId);
  fheapClean();
  return 0;
}
#endif
