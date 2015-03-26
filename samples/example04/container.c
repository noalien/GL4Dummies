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

     el = 111;
     vector_set(test_v, &el, 10);

     int i;
     for (i=0; i < test_v.size; i++)
	  printf("%d ", *(int *)vector_get(test_v, i));
     putchar('\n');

     vector_delete(test_v);
}

int main(void)
{
     test_vector();
     return 0;
}
