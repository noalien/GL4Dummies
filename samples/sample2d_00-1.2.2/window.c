/*!\file window.c
 * \brief GL4Dummies, exemple de tri d'un tableau et son affichage
 * comme des barres représentant la valeur en chaque case du tableau.
 * Ce code est composé d'ajouts (la ou il y a un commentaire "nouveau,
 * ...") par rapport au projet sample2d_00-1.2.1. Ici le tri se fait
 * pas `a pas, toutes les étapes sont visibles.
 * \author Farès BELHADJ, amsi@up8.edu
 * \date April 13 2020
 */

/* inclusion des entêtes de fonctions de gestion de primitives simples
 * de dessin. La lettre p signifie aussi bien primitive que
 * pédagogique. */
#include <GL4D/gl4dp.h>

/* inclusion des entêtes de fonctions de création et de gestion de
 * fenêtres système ouvrant un contexte favorable à GL4dummies. Cette
 * partie est dépendante de la bibliothèque SDL2 */
#include <GL4D/gl4duw_SDL2.h>

/* inclusion des entêtes pour rand/srand  */
#include <stdlib.h>
/* inclusion des entêtes pour time  */
#include <time.h>

/*!\brief nombre d'élément du tableau `a trier */
#define N 128
/*!\brief le tableau `a trier */
static int _a_trier[N];
/*!\brief nouveau, les deux données qui échangent leurs places */
static int _to_swap[2];

/*!\brief initialise aléatoirement les valeurs du tableau passé en
 * argument avec des valeurs allant de 0 `a n - 1. */
static void init(int * t, int n) {
  int i;
  for(i = 0; i < n; ++i)
    t[i] = n * (rand() / (RAND_MAX + 1.0));
}

/*!\brief trie par sélection le tableau passé en argument. */
static void triSelection(int * t, int n) {
  static int i = 0; /* nouveau, i devient statique */
  int /*i,*/ j, min/*, v*/;
  /* nouveau, échanger si nécessaire les deux valeurs de l'étape
   *  précédente */
  if(_to_swap[0] != _to_swap[1]) {
    int v = t[_to_swap[1]];
    t[_to_swap[1]] = t[_to_swap[0]];
    t[_to_swap[0]] = v;
  }
  /* nouveau, dire, pour l'instant, qu'aucune donnée n'a été
   * échangée */
  _to_swap[0] = _to_swap[1] = -1;
  /* plus de boucle, on attend le prochain tour */
  /* for(i = 0; i < n - 1; ++i) { */
  if(i < n - 1) {
    min = i;
    for(j = i + 1; j < n; ++j)
      if(t[j] < t[min])
	min = j;
    /* nouveau, marquer les deux données `a échanger */
    _to_swap[0] = i; _to_swap[1] = min;
    /* if(i != min) {
      v = t[min];
      t[min] = t[i];
      t[i] = v;
      } */
    /* nouveau, i se prépare pour la prochaine fois */
    ++i;
    /* nouveau, ajouter un délai en ms (ralentir 1/4 de seconde) pour
     * avoir le temps de visualiser l'étape */
    SDL_Delay(250);
  }
}

/*!\brief appelée au moment de sortir du programme (atexit), elle
 *  libère les éléments utilisés par GL4Dummies.*/
static void quitte(void) {
  gl4duClean(GL4DU_ALL);
}

static void vLine(int x, int y0, int y1, GLuint color) {
  int w = gl4dpGetWidth(), h = gl4dpGetHeight(), i, p;
  GLuint * pixels = gl4dpGetPixels();
  /* tous les cas à éviter, ne donnant rien */
  if(x < 0 || x >= w || (y0 < 0 && y1 < 0) || (y0 >= h && y1 >= h))
    return;
  y0 = MIN(MAX(y0, 0), h - 1);
  y1 = MIN(MAX(y1, 0), h - 1);
  p = y0 < y1 ? w : -w;
  y0 = y0 * w + x;
  y1 = y1 * w + x;
  for(i = y0; i != y1; i += p)
    pixels[i] = color;
}

/*!\brief fonction appelée à chaque idle par la gl4duwMainLoop.*/
static void calcul(void) {
  triSelection(_a_trier, N);
}

/*!\brief fonction appelée à chaque draw par la gl4duwMainLoop.*/
static void dessin(void) {
  int i;
  /* nouveau, effacer l'écran `a chaque fois car la donnée peut etre
     différente d'une frame `a l'autre */
  gl4dpClearScreen();
  for(i = 0; i < N; ++i)
    vLine(i, 0, _a_trier[i], RGB(128, 128, 128));
  /* nouveau, surligner en rouge les deux positions ayant échangé
   * leurs places */
  if(_to_swap[0] >= 0) {
    vLine(_to_swap[0], 0, _a_trier[_to_swap[0]], RGB(255, 0, 0));
    vLine(_to_swap[1], 0, _a_trier[_to_swap[1]], RGB(255, 0, 0));
  }
  gl4dpUpdateScreen(NULL);
}

/*!\brief créé la fenêtre, un screen 2D effacé en noir et lance une
 *  boucle infinie.*/
int main(int argc, char ** argv) {
  /* initialisation de la chaine pseudo-aléatoire en utilisant le
   * temps */
  srand(time(NULL));
  /* remplir aléatoirement le tableau */
  init(_a_trier, N);
  /* tentative de création d'une fenêtre pour GL4Dummies */
  if(!gl4duwCreateWindow(argc, argv, /* args du programme */
			 "GL4Dummies' Hello Pixels", /* titre */
			 10, 10, 4 * N, 2 * N, /* x,y, largeur, heuteur */
			 GL4DW_SHOWN) /* état visible */) {
    /* ici si échec de la création souvent lié à un problème d'absence
     * de contexte graphique ou d'impossibilité d'ouverture d'un
     * contexte OpenGL (au moins 3.2) */
    return 1;
  }
  /* création d'un screen GL4Dummies (texture dans laquelle nous
   * pouvons dessiner) à des dimensions différentes à celles de la
   * fenêtre */
  gl4dpInitScreenWithDimensions(N, N);
  /* ajoute la fonction quitte à la pile des choses à faire en sortant
   * du programme */
  atexit(quitte);
  /* met en place une fonction de display au sein de la boucle
   * event-simu-draw gl4duwMainLoop */
  gl4duwDisplayFunc(dessin);
  /* met en place une fonction idle correspondant `a la phase
   * "simulation" au sein de la boucle event-simu-draw
   * gl4duwMainLoop. */
  gl4duwIdleFunc(calcul);
  /* boucle infinie pour éviter que le programme ne s'arrête et ferme
   * la fenêtre */
  gl4duwMainLoop();
  return 0;
}
