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
	  {0.0f, 0.0f} /* s, t */
	},
	{ /* vertex 1 */
	  {1.0f, -1.0f, 0.0f, 1.0f}, /* x, y, z, w */
	  {0.0f, 1.0f, 0.0f, 1.0f}, /* r, g, b, a */
	  {0.0f, 0.0f, 1.0f}, /* nx, ny, nz */
	  {1.0f, 0.0f} /* s, t */
	},
	{ /* vertex 2 */
	  {-1.0f, 1.0f, 0.0f, 1.0f}, /* x, y, z, w */
	  {0.0f, 0.0f, 1.0f, 1.0f}, /* r, g, b, a */
	  {0.0f, 0.0f, 1.0f}, /* nx, ny, nz */
	  {0.0f, 1.0f} /* s, t */
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
	  {0.0f, 1.0f} /* s, t */
	},
	{ /* vertex 1 */
	  {1.0f, -1.0f, 0.0f, 1.0f}, /* x, y, z, w */
	  {0.0f, 1.0f, 0.0f, 1.0f}, /* r, g, b, a */
	  {0.0f, 0.0f, 1.0f}, /* nx, ny, nz */
	  {1.0f, 0.0f} /* s, t */
	},
	{ /* vertex 3 */
	  {1.0f, 1.0f, 0.0f, 1.0f}, /* x, y, z, w */
	  {1.0f, 0.0f, 0.0f, 1.0f}, /* r, g, b, a */
	  {0.0f, 0.0f, 1.0f}, /* nx, ny, nz */
	  {1.0f, 1.0f} /* s, t */
	}
      }
    }
  };


  surface_t s = { 2, t };
  mat4 projection, view, model;
  /* on teste les matrices de projection */
  /* _ortho2D(projection, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f); */
  _frustum(projection, -1.0f, 1.0f, -1.0f, 1.0f, 2.0f, 10.0f);


  /* on teste la vue avec un simple translate */
  _mat4identite(view);
  _translate(view, 0.0f, 0.0f, -4.5f);

  static float a = 0.0f, b = 0.0f, c = 0.0f;
  elClear();

  /* on teste plusieurs "model" pour la même surface */

  _mat4identite(model);
  _rotate(model, a += 0.2f, 0.0f, 1.0f, 0.0f);
  elTransformations(&s, model, view, projection);
  elDraw(&s);
  
  _mat4identite(model);
  _translate(model, -2.0f, 2.0f, -2.5f);
  _rotate(model, b += 0.04f, 0.0f, 0.0f, 1.0f);
  _rotate(model, c += 0.4f, 0.0f, 1.0f, 0.0f);
  elTransformations(&s, model, view, projection);
  elDraw(&s);
  
  elUpdate();
}

int main(int argc, char ** argv) {
  if(!elInit(argc, argv, /* args du programme */
	     "Ellule' Hello World", /* titre */
	     1280, 960, 800, 600) /* largeur_f, hauteur_f, largeur_e, hauteur_e */) {
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
  /* voir si des choses à libérer ... */
}
