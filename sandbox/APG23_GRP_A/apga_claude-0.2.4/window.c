/*!\file window.c
 * \brief test de la lib claude
 * \author Farès Belhadj et le groupe A de L2 (23/24)
 * \date octobre-novembre 2023
 */

#include "claude.h"
#include "primitives.h"

static void dis(void) {
  static triangle_t t[2] = {
    { /* Triangle 1 */
      {
	{ /* vertex 1 */
	  -1.0f, -1.0f, 0.0f, 1.0f, /* x, y, z, w */
	  1.0f, 0.0f, 0.0f, 1.0f, /* r, g, b, a */
	  0.0f, 0.0f, /* s, t */
	  0.0f, 0.0f, 1.0f, /* nx, ny */
	  0, 0 /* xe, ye */
	},
	{ /* vertex 2 */
	  1.0f, -1.0f, 0.0f, 1.0f, /* x, y, z, w */
	  0.0f, 1.0f, 0.0f, 1.0f, /* r, g, b, a */
	  1.0f, 0.0f, /* s, t */
	  0.0f, 0.0f, 1.0f, /* nx, ny */
	  0, 0 /* xe, ye */
	},
	{ /* vertex 3 */
	  -1.0f, 1.0f, 0.0f, 1.0f, /* x, y, z, w */
	  0.0f, 0.0f, 1.0f, 1.0f, /* r, g, b, a */
	  0.0f, 1.0f, /* s, t */
	  0.0f, 0.0f, 1.0f, /* nx, ny */
	  0, 0 /* xe, ye */
	}
      }
    },
    { /* Triangle 2 */
      {
	{ /* vertex 1 */
	  -1.0f, 1.0f, 0.0f, 1.0f, /* x, y, z, w */
	  0.0f, 0.0f, 1.0f, 1.0f, /* r, g, b, a */
	  0.0f, 1.0f, /* s, t */
	  0.0f, 0.0f, 1.0f, /* nx, ny */
	  0, 0 /* xe, ye */
	},
	{ /* vertex 2 */
	  1.0f, -1.0f, 0.0f, 1.0f, /* x, y, z, w */
	  0.0f, 1.0f, 0.0f, 1.0f, /* r, g, b, a */
	  1.0f, 0.0f, /* s, t */
	  0.0f, 0.0f, 1.0f, /* nx, ny */
	  0, 0 /* xe, ye */
	},
	{ /* vertex 3 */
	  1.0f, 1.0f, 0.0f, 1.0f, /* x, y, z, w */
	  1.0f, 1.0f, 0.0f, 1.0f, /* r, g, b, a */
	  1.0f, 1.0f, /* s, t */
	  0.0f, 0.0f, 1.0f, /* nx, ny */
	  0, 0 /* xe, ye */
	}
      }
    }
  };
  surface_t s = { 2, t }, * sp = duplicate_surface(&s);
  
  mat4 projection, view, model;
  /* on teste la perspective */
  mat4identite(projection);
  frustum(projection, -1.0f, 1.0f, -1.0f * 3.0f / 4.0f, 1.0f * 3.0f / 4.0f, 2.0f, 5.0f);
  //ortho(projection, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 5.0f);
  mat4identite(view);
  mat4identite(model);

  /* on teste la rotation (décommentez) */
  static float a = 0.0f;
  translate(model, 0.0f, 0.0f, -4.0f);
  rotate(model, 20, 1.0f, 0.0f, 0.0f);
  //rotate(model, a += 1.0f, 0.0f, 0.0f, 1.0f);
  rotate(model, a += 1.0f, 0.0f, 1.0f, 0.0f);

  
  
  /* on transforme */
  claude_apply_transforms(model, view, projection, &s, sp);
  /* on dessine un triangle */
  static const int viewport[] = { 0, 0, 640, 480};
  claude_clear();
  claude_draw(sp, viewport);
  update_screen();

  free_surface(sp);
}

/*!\brief créé la fenêtre, un screen 2D effacé en noir et lance une
 *  boucle infinie.*/
int main(int argc, char ** argv) {
  /* tentative de création d'une fenêtre pour GL4Dummies */
  if(!claude_init(argc, argv, /* args du programme */
		  "Claude' Hello World", /* titre */
		  1100, 825, 640, 480) /* largeur_f, hauteur_f, largeur_e, hauteur_e */) {
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
