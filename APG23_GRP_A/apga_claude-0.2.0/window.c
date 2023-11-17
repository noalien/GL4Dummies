/*!\file window.c
 * \brief test de la lib claude
 * \author Farès Belhadj et le groupe A de L2 (23/24)
 * \date octobre-novembre 2023
 */

#include "claude.h"
#include "primitives.h"

static void dis(void) {
  static const triangle_t t = {
    {
      { /* vertex 1 */
	0.0f, 0.0f, 0.0f, 1.0f, /* x, y, z, w */
	1.0f, 0.0f, 0.0f, 1.0f, /* r, g, b, a */
	0.0f, 0.0f, /* s, t */
	0.0f, 0.0f, 1.0f, /* nx, ny */
	0, 0 /* xe, ye */
      },
      { /* vertex 2 */
	0.9f, 0.4f, 0.0f, 1.0f, /* x, y, z, w */
	0.0f, 1.0f, 0.0f, 1.0f, /* r, g, b, a */
	1.0f, 0.0f, /* s, t */
	0.0f, 0.0f, 1.0f, /* nx, ny */
	900, 300 /* xe, ye */
      },
      { /* vertex 3 */
	0.01f, 0.95f, 0.0f, 1.0f, /* x, y, z, w */
	0.0f, 0.0f, 1.0f, 1.0f, /* r, g, b, a */
	0.0f, 1.0f, /* s, t */
	0.0f, 0.0f, 1.0f, /* nx, ny */
	10, 700 /* xe, ye */
      }
    }
  };
  /* on dessine un triangle dessus */
  fill_triangle(&t);
  update_screen();  
}

/*!\brief créé la fenêtre, un screen 2D effacé en noir et lance une
 *  boucle infinie.*/
int main(int argc, char ** argv) {
  /* tentative de création d'une fenêtre pour GL4Dummies */
  if(!claude_init(argc, argv, /* args du programme */
		  "Claude' Hello World", /* titre */
		  960, 720, 960, 720) /* largeur_f, hauteur_f, largeur_e, hauteur_e */) {
    /* ici si échec de la création souvent lié à un problème d'absence
     * de contexte graphique ou d'impossibilité d'ouverture d'un
     * contexte OpenGL (au moins 3.2) */
    return 1;
  }

  
  gl4duwDisplayFunc(dis);
  apply_texture("images/leo.bmp");
  /* boucle infinie pour éviter que le programme ne s'arrête et ferme
   * la fenêtre immédiatement */
  gl4duwMainLoop();
  return 0;
}
