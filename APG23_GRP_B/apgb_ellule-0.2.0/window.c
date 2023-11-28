/*!\file window.c
 * \brief 
 * \author 
 * \date 
 */

#include "elluleRaster.h"

static void quit(void);


static void dis(void) {
  static triangle_t t[2] = {
    { /* Triangle 0 */
      {
	{ /* vertex 0 */
	  {-1.0f, -1.0f, 0.0f, 1.0f}, /* x, y, z, w */
	  {1.0f, 0.0f, 0.0f, 1.0f}, /* r, g, b, a */
	  {0.0f, 0.0f, 1.0f}, /* nx, ny, nz */
	  {0.0f, 0.0f}, /* s, t */
	  {0, 0} /* xe, ye */
	},
	{ /* vertex 1 */
	  {1.0f, -1.0f, 0.0f, 1.0f}, /* x, y, z, w */
	  {0.0f, 1.0f, 0.0f, 1.0f}, /* r, g, b, a */
	  {0.0f, 0.0f, 1.0f}, /* nx, ny, nz */
	  {1.0f, 0.0f}, /* s, t */
	  {0, 0} /* xe, ye */
	},
	{ /* vertex 2 */
	  {-1.0f, 1.0f, 0.0f, 1.0f}, /* x, y, z, w */
	  {0.0f, 0.0f, 1.0f, 1.0f}, /* r, g, b, a */
	  {0.0f, 0.0f, 1.0f}, /* nx, ny, nz */
	  {0.0f, 1.0f}, /* s, t */
	  {0, 0} /* xe, ye */
	}
      }
    },
    {
      /* Triangle 1 */
      {
	{ /* vertex 2 */
	  {-1.0f, 1.0f, 0.0f, 1.0f}, /* x, y, z, w */
	  {0.0f, 0.0f, 1.0f, 1.0f}, /* r, g, b, a */
	  {0.0f, 0.0f, 1.0f}, /* nx, ny, nz */
	  {0.0f, 1.0f}, /* s, t */
	  {0, 0} /* xe, ye */
	},
	{ /* vertex 1 */
	  {1.0f, -1.0f, 0.0f, 1.0f}, /* x, y, z, w */
	  {0.0f, 1.0f, 0.0f, 1.0f}, /* r, g, b, a */
	  {0.0f, 0.0f, 1.0f}, /* nx, ny, nz */
	  {1.0f, 0.0f}, /* s, t */
	  {0, 0} /* xe, ye */
	},
	{ /* vertex 3 */
	  {1.0f, 1.0f, 0.0f, 1.0f}, /* x, y, z, w */
	  {1.0f, 0.0f, 0.0f, 1.0f}, /* r, g, b, a */
	  {0.0f, 0.0f, 1.0f}, /* nx, ny, nz */
	  {1.0f, 1.0f}, /* s, t */
	  {0, 0} /* xe, ye */
	}
      }
    }
  };


  surface_t s = { 2, t };
  mat4 projection, view, model;
  /* on teste les matrices */
  _ortho2D(projection, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 5.0f);
  _mat4identite(view);
  _mat4identite(model);

  /* on teste la rotation (décommentez) */
  static float a = 0.0f;
  _rotate(model, a += 1.0f, 0.0f, 0.0f, 1.0f);

  elTransformations(&s, model, view, projection);
  elDraw(&s);
  
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
