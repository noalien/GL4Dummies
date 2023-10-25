/*!\file window.c
 * \brief 
 * \author 
 * \date 
 */

#include "elluleRaster.h"

static void dis(void) {
  triangle_t t = { { {10, 10}, {300, 100}, {30, 230}  } };
  elFillTriangle(&t);
  elUpdate();
}

int main(int argc, char ** argv) {
  if(!elInit(argc, argv, /* args du programme */
	     "Ellule' Hello World", /* titre */
	     320, 240, 320, 240) /* largeur_f, hauteur_f, largeur_e, hauteur_e */) {
    /* ici si échec de la création souvent lié à un problème d'absence
     * de contexte graphique ou d'impossibilité d'ouverture d'un
     * contexte OpenGL (au moins 3.2) */
    return 1;
  }
  gl4duwDisplayFunc(dis);
  gl4duwMainLoop();
  return 0;
}
