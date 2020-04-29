/*!\file window.c
 * \brief GL4Dummies, exemple simple 2D avec GL4Dummies
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

/*!\brief fonction appelée à chaque draw par la gl4duwMainLoop.*/
static void dessin(void) {
  int x, y;
  Uint32 c;
  for(y = 0; y < 16; y++)
    for(x = 0; x < 16; x++) {
      /* pour chacun des 256 pixels, calculer l'intensité */
      c = y * 16 + x;
      /* appliquer cette intensité sur chacune des composantes rouge,
       * vert, bleu et alpha du pixel */
      c = c | (c << 8) | (c << 16) | (c << 24);
      /* met cette couleur comme couleur de dessin courante */
      gl4dpSetColor(c);
      /* affecte le pixel x,y avec la couleur en cours */
      gl4dpPutPixel(x, y);
    }
  gl4dpUpdateScreen(NULL);
}

/*!\brief créé la fenêtre, un screen 2D effacé en noir et lance une
 *  boucle infinie.*/
int main(int argc, char ** argv) {
  /* tentative de création d'une fenêtre pour GL4Dummies */
  if(!gl4duwCreateWindow(argc, argv, /* args du programme */
			 "GL4Dummies' Hello Pixels", /* titre */
			 10, 10, 320, 320, /* x,y, largeur, heuteur */
			 GL4DW_SHOWN) /* état visible */) {
    /* ici si échec de la création souvent lié à un problème d'absence
     * de contexte graphique ou d'impossibilité d'ouverture d'un
     * contexte OpenGL (au moins 3.2) */
    return 1;
  }
  /* création d'un screen GL4Dummies (texture dans laquelle nous
   * pouvons dessiner) à des dimensions différentes à celles de la
   * fenêtre */
  gl4dpInitScreenWithDimensions(16, 16);
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
