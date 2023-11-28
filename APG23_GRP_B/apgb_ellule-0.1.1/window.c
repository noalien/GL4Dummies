/*!\file window.c
 * \brief 
 * \author 
 * \date 
 */

#include "elluleRaster.h"

static void quit(void);


static void dis(void) {
  triangle_t t = { { {10, 10, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f}, {700, 100, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f}, {30, 530, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f}  } };
  elFillTriangle(&t);
  elUpdate();
}

int main(int argc, char ** argv) {
  if(!elInit(argc, argv, /* args du programme */
	     "Ellule' Hello World", /* titre */
	     800, 600, 800, 600) /* largeur_f, hauteur_f, largeur_e, hauteur_e */) {
    /* ici si échec de la création souvent lié à un problème d'absence
     * de contexte graphique ou d'impossibilité d'ouverture d'un
     * contexte OpenGL (au moins 3.2) */
    return 1;
  }
  elUseTexture("images/terre.bmp");
  atexit(quit);
  gl4duwDisplayFunc(dis);
  gl4duwMainLoop();
  return 0;
}

void quit(void) {
  elUseTexture("");  /* pour libérer la mémoire texture */
}
