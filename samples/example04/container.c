#include <GL4D/vector.h>
#include <GL4D/list.h>

void test_vector()
{
  int el;
  vector_t test_v = vector_new(sizeof el);

  el = 10;
  vector_insert(&test_v, &el, 0);
  el = 123;
  vector_insert(&test_v, &el, 10);
  el = 20;
  vector_insert(&test_v, &el, 5);
  el = 33;
  vector_insert(&test_v, &el, 5);

  vector_erase(&test_v, 1);

  el = 1;
  vector_set(test_v, &el, 1);
  vector_set(test_v, &el, 2);
  vector_set(test_v, &el, 3);
  vector_set(test_v, &el, 6);
  vector_set(test_v, &el, 7);
  vector_set(test_v, &el, 8);
  vector_set(test_v, &el, 9);
  vector_set(test_v, &el, 10);

  int i;
  for (i=0; i < test_v.size; i++)
    printf("%d ", *(int *)vector_get(test_v, i));
  putchar('\n');

  vector_delete(test_v);
}

void test_list()
{
  int el;
  list_t test_l = list_new(sizeof el);

  el = 10;
  list_insert(&test_l, &el, 0);
  el = 123;
  list_insert(&test_l, &el, 10);
  el = 20;
  list_insert(&test_l, &el, 5);
  el = 33;
  list_insert(&test_l, &el, 5);

  list_erase(&test_l, 1);

  el = 1;
  list_set(test_l, &el, 1);
  list_set(test_l, &el, 2);
  list_set(test_l, &el, 3);
  list_set(test_l, &el, 6);
  list_set(test_l, &el, 7);
  list_set(test_l, &el, 8);
  list_set(test_l, &el, 9);
  list_set(test_l, &el, 10);

  int i;
  for (i = 0; i < test_l.size; i++)
    printf("%d ", *(int *)list_get(test_l, i));
  putchar('\n');

  list_delete(test_l);
}

int main(int argc, char **argv)
{
  test_vector();
  test_list();
  return 0;
}
