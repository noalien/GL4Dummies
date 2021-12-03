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

static inline GLuint rgba(GLubyte r, GLubyte g, GLubyte b, GLubyte a) {
  return RGBA(r, g, b, a);
}

/*!\brief fonction appelée à chaque draw par la gl4duwMainLoop.*/
static void dessin(void) {
  const int w = gl4dpGetWidth();
  const int h = gl4dpGetHeight();
  GLuint * pixels = gl4dpGetPixels();
  for(int i = 0; i < 1000; ++i) {
    const int x = rand() % w;
    const int y = rand() % h;
    pixels[ y * w + x ] = rgba(rand() & 255, rand() & 255, rand() & 255, 255);
  }
  gl4dpScreenHasChanged();
  gl4dpUpdateScreen(NULL);
}

/*!\brief créé la fenêtre, un screen 2D effacé en noir et lance une
 *  boucle infinie.*/
int main(int argc, char ** argv) {
  /* tentative de création d'une fenêtre pour GL4Dummies */
  if(!gl4duwCreateWindow(argc, argv, /* args du programme */
			 "GL4Dummies' Hello Pixels", /* titre */
			 10, 10, 600, 600, /* x,y, largeur, heuteur */
			 GL4DW_SHOWN) /* état visible */) {
    /* ici si échec de la création souvent lié à un problème d'absence
     * de contexte graphique ou d'impossibilité d'ouverture d'un
     * contexte OpenGL (au moins 3.2) */
    return 1;
  }
  /* création d'un screen GL4Dummies (texture dans laquelle nous
   * pouvons dessiner) à des dimensions différentes à celles de la
   * fenêtre */
  gl4dpInitScreenWithDimensions(1024, 1024);
  /* ajoute la fonction quitte à la pile des choses à faire en sortant
   * du programme */
  SDL_GL_SetSwapInterval(0);
  atexit(quitte);
  /* met en place une fonction de display au sein de la boucle
   * event-simu-draw gl4duwMainLoop */
  gl4duwDisplayFunc(dessin);
  /* boucle infinie pour éviter que le programme ne s'arrête et ferme
   * la fenêtre */
  gl4duwMainLoop();
  return 0;
}
