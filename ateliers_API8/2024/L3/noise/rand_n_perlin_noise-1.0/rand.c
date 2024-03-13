/*!\file rand.c
 * \brief bruit 1D et 2D
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date March 3 2017
 */
#include <string.h>

/*!\brief valeur aléatoire maximale (incluse) */
#define URMAX 0x7fffffff

/*!\brief la graine de la série aléatoire */
static int _seed = 7;

/*!\brief générateur de bruit à distribution uniforme. On utilise un
   générateur congruentiel linéaire (voir Donald E. Knuth, The Art of
   Computer Programming, ou wikipédia) */
static int urand(void) {
  int val;
  val = ((_seed * 1103515245) + 12345) & URMAX;
  _seed = val;
  return val;
}

/*!\brief bruit paramétrique 1D : une valeur (x) donne un nombre
   "aléatoire", toujours le même. Essayez en commentant un ou
   plusieurs des trois premières lignes de la fonction.
*/
static int x_urand(int x) {
  x = (x << 12) ^ x;
  x = ((x * 1103515245) + 12345) & URMAX;
  x = (x << 10) ^ x;
  return ((x * 1103515245) + 12345) & URMAX;
}

/*!\brief bruit paramétrique 2D : un couple (x, y) donne un nombre
   "aléatoire", toujours le même. Elle utilise le bruit paramétrique
   1D. Changez la constante et regardez le résultat, selon la valeur
   de la constante on voit ou pas la cyclicité du bruit.
*/
static int xy_urand(int x, int y) {
  x = x_urand(x) * 975321;
  return x_urand(x + y);
}

/*!\brief Test statistique sur série de bruit (comptage d'occurrences).*/
int test1(int * t, int n, int nn) {
  int i, r, max = 0;
  memset(t, 0, n * sizeof *t);
  for(i = 0; i < nn; i++) {
    r = urand()%n;
    /* ou ça r = n * (urand() / (URMAX + 1.0)); */
    t[r]++;
    if(max < t[r])
      max = t[r];
  }
  return max;
}

/*!\brief Remplissage d'un tableau avec le bruit paramétrique 1D */
int test2(int * t, int n) {
  int i;
  for(i = 0; i < n; i++) {
    t[i] = x_urand(i)%n;
  }
  return n;
}

/*!\brief Test statistique sur série de bruit (comptage
   d'occurrences). Ici pour être visualisé en 2D. */
int test3(int * t, int side, int nn) {
  int i, r, max = 0, ss = side * side;
  memset(t, 0, side * side * sizeof *t);
  for(i = 0; i < nn; i++) {
    r = urand()%ss;
    t[r]++;
    if(max < t[r])
      max = t[r];
  }
  return max;
}

/*!\brief Remplissage d'une grille avec le bruit paramétrique 2D */
int test4(int * t, int side) {
  int x, y, ys, ysx, max = 0;
  for(y = 0; y < side; y++) {
    ys = y * side;
    for(x = 0; x < side; x++) {
      t[ysx = ys + x] = xy_urand(x, y);
      if(max < t[ysx])
	max = t[ysx];
    }
  }
  return max;
}
