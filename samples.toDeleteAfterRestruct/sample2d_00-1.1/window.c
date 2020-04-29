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
static void dessine(void) {
  /* sélection au hasard d'une intensité de rouge, de vert et de bleu */
  GLubyte r = rand() % 256, g = rand() % 256, b = rand() % 256;
  /* effacement du screen en cours en utilisant une couleur au hasard */
  gl4dpClearScreenWith(RGB(r, g, b));
  /* mise à jour du screen côté OpenGL */
  gl4dpUpdateScreen(NULL);
}

/*!\brief créé la fenêtre, un screen 2D effacé en noir et lance une
 *  boucle infinie.*/
int main(int argc, char ** argv) {
  /* tentative de création d'une fenêtre pour GL4Dummies */
  if(!gl4duwCreateWindow(argc, argv, /* args du programme */
			 "GL4Dummies' Hello World", /* titre */
			 10, 10, 320, 240, /* x,y, largeur, heuteur */
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
  gl4duwDisplayFunc(dessine);
  /* boucle infinie pour éviter que le programme ne s'arrête et ferme
   * la fenêtre */
  gl4duwMainLoop();
  return 0;
}
