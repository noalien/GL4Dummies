/*!\file window.c
 * \brief GL4Dummies, exemple simple 2D avec GL4Dummies. Première
 * approche pour dessiner des segments de droites.
 *
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date February 04 2018
 */

/* inclusion des entêtes de fonctions de gestion de primitives simples
 * de dessin. La lettre p signifie aussi bien primitive que
 * pédagogique. */
#include <GL4D/gl4dp.h>

/* inclusion des entêtes de fonctions de création et de gestion de
 * fenêtres système ouvrant un contexte favorable à GL4dummies. Cette
 * partie est dépendante de la bibliothèque SDL2 */
#include <GL4D/gl4duw_SDL2.h>

/*!\brief appelée au moment de sortir du programme (atexit), elle
 *  libère les éléments utilisés par GL4Dummies.*/
static void quitte(void) {
  gl4duClean(GL4DU_ALL);
}

/*!\brief première version d'une fonction de dessin d'un segment de
 * droite allant de (x0, y0) vers (x1, y1). Cette fonction se limite
 * au dessin des droites dont la progresssion se fait dans le premier
 * et second octan (donc dans le quart de plan où x et y sont
 * positifs). 
 *
 * Exercice (corrigé en 1.4) : étendre la fonction à tous les
 * octans. */
static void ligneO1(int x0, int y0, int x1, int y1) {
  /* u est la différence en x pour recentrer le segment en 0 côté abscisse */
  int u = x1 - x0, x;
  /* v est la différence en y pour recentrer le segment en 0 côté ordonnées */
  int v = y1 - y0;
  if(u > v) { /* cas du premier octant (x augmente tout le temps, y de
	       * temps en temps en fonction de la pente */
    /* p est la pente de la droite y = p * x */
    float p = v / (float)u, y;
    for(x = 0, y = 0; x <= u; x++) {
      /* y augmente de manière proportionnelle à la pente mais ce
       * n'est que sa partie entière qui sera dessinée */
      y += p;
      /* nous recentrons le segment en (x0, y0) pour en faire le point
       * de départ */
      gl4dpPutPixel(x0 + x, y0 + ((int)y));
    }
  } else { /* cas du deuxième octant (y augmente tout le temps, x de
	    * temps en temps en fonction de la pente. Pour faire
	    * simple nous inversions l'usage de x et y et aussi de u
	    * et v car nous calculons la version symatrique du cas
	    * premier octan par rapport à l'axe y = x */
    float p = u / (float)v, y;
    for(x = 0, y = 0; x <= v; x++) {
      y += p;
      gl4dpPutPixel(x0 + ((int)y), y0 + x);
    }
  }
}

/*!\brief fonction appelée à chaque draw par la gl4duwMainLoop. */
static void dessin(void) {
  float a;
  /* partant du point (0, 0), nous dessinons des segments décrivant un
   * quart de cercle ; nous utilisons la formule trigonométrique */
  for(a = 0; a < M_PI / 2.0f; a += 0.02f) {
    /* choisissons au hasard 3 intensités de composantes primaires de
     * couleur (min est 0, max est 255 car sur un octet non signé). */
    GLubyte r = rand() % 256, g = rand() % 256, b = rand() % 256;
    /* nous mettons la couleur aléatoire */
    gl4dpSetColor(RGB(r, g, b));
    /* dessinons le segment de droite */
    ligneO1(0, 0, 
	    (gl4dpGetWidth()  - 1) * cos(a), 
	    (gl4dpGetHeight() - 1) * sin(a));
  }
  /* mise à jour de l'écran, essayez de mettre cet appel dans la
   * boule ci-dessus. */
  gl4dpUpdateScreen(NULL);
}

/*!\brief créé la fenêtre, un screen 2D effacé en noir et lance une
 *  boucle infinie.*/
int main(int argc, char ** argv) {
  /* tentative de création d'une fenêtre pour GL4Dummies */
  if(!gl4duwCreateWindow(argc, argv, /* args du programme */
			 "GL4Dummies' Hello Pixels", /* titre */
			 10, 10, 800, 800, /* x,y, largeur, heuteur */
			 GL4DW_SHOWN) /* état visible */) {
    /* ici si échec de la création souvent lié à un problème d'absence
     * de contexte graphique ou d'impossibilité d'ouverture d'un
     * contexte OpenGL (au moins 3.2) */
    return 1;
  }
  /* création d'un screen GL4Dummies (texture dans laquelle nous
   * pouvons dessiner) aux dimensions de la fenêtre */
  gl4dpInitScreen();
  /* ajoute la fonction quitte à la pile des choses à faire en sortant
   * du programme */
  atexit(quitte);
  /* met en place une fonction de display au sein de la boucle
   * event-simu-draw gl4duwMainLoop */
  gl4duwDisplayFunc(dessin);
  /* boucle infinie pour éviter que le programme ne s'arrête et ferme
   * la fenêtre */
  gl4duwMainLoop();
  return 0;
}
