/*!\file window.c
 * \brief 
 * \author 
 * \date 
 */

#include "elluleRaster.h"

static void quit(void);

static surface_t * _quad = NULL;

static void dis(void) {
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
  elTransformations(_quad, model, view, projection);
  elDraw(_quad);
  
  _mat4identite(model);
  _translate(model, -2.0f, 2.0f, -2.5f);
  _rotate(model, b += 0.04f, 0.0f, 0.0f, 1.0f);
  _rotate(model, c += 0.4f, 0.0f, 1.0f, 0.0f);
  elTransformations(_quad, model, view, projection);
  elDraw(_quad);
  
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
  /* à venir elSetTexture(_tex) et elSetColor(_couleur) */
  elUseTexture("images/terre.bmp");
  _quad = elQuad();
  atexit(quit);
  gl4duwDisplayFunc(dis);
  gl4duwMainLoop();
  return 0;
}

void quit(void) {
  /* voir si des choses à libérer ... */
  if(_quad) {
    elFreeSurface(_quad);
    _quad = NULL;
  }
}
