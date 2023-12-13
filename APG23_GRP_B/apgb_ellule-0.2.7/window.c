/*!\file window.c
 * \brief Test de la lib Ellule réalisée dans le cadre du cours de L2
 * <<algo prog graphique>> en collaboration avec le groupe L2-B.
 * \author Farès Belhadj
 * \date septembre à décembre 2023
 */

#include "elluleRaster.h"

#define SW 640 /* screen width */
#define SH 480 /* screen height */

static void _quit(void);

static surface_t * _quad = NULL, * _cube = NULL, * _sphere = NULL;
/* les textures utilisées dans cet exemple */
static texture_t * _terre = NULL, * _feuilles = NULL, * _parquet = NULL;

static void dis(void) {
  mat4 projection, view, model;
  /* on teste les matrices de projection */
  /* _ortho2D(projection, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f); */
  _frustum(projection, -1.0f, 1.0f, -0.666f, 0.666f, 2.0f, 10.0f);


  /* on teste la vue avec un simple translate */
  _mat4identite(view);
  _translate(view, 0.0f, 0.0f, -5.5f);

  static float a = 0.0f, b = 0.0f, c = 0.0f;
  elClear();

  /* on teste plusieurs "model" pour des surfaces différentes */
  
  elDisable(EL_ALPHA);
  elDisable(EL_BACKFACE_CULLING);
  _mat4identite(model);
  _translate(model, -2.0f, 2.0f, -2.5f);
  _rotate(model, b += 0.04f, 0.0f, 0.0f, 1.0f);
  _rotate(model, c += 0.4f, 0.0f, 1.0f, 0.0f);
  elTransformations(_quad, model, view, projection);
  elDraw(_quad);
  
  elEnable(EL_BACKFACE_CULLING);
  _mat4identite(model);
  _translate(model, 1.2f, 0.7f, 1.5f);
  _rotate(model, -b, 0.0f, 0.0f, 1.0f);
  _rotate(model, c, 0.0f, 1.0f, 0.0f);
  _scale(model, 0.3f, 0.2f, 0.3f);
  elTransformations(_sphere, model, view, projection);
  elDraw(_sphere);
  

  elEnable(EL_ALPHA);
  _mat4identite(model);
  _rotate(model, a += 0.2f, 0.0f, 1.0f, 0.0f);
  elTransformations(_cube, model, view, projection);
  elDraw(_cube);

  elUpdate();
}

int main(int argc, char ** argv) {
  const vec4 rouge = { 1.0f, 0.5f, 0.5f, 1.0f };
  const vec4 jaune = { 1.5f, 1.0f, 0.3f, 0.6f };
  const vec4 bleu  = { 0.4f, 0.4f, 1.0f, 1.0f };
  if(!elInit(argc, argv, /* args du programme */
	     "Ellule' Hello World", /* titre */
	     1100, 825, SW, SH) /* largeur_f, hauteur_f, largeur_e, hauteur_e */) {
    /* ici si échec de la création souvent lié à un problème d'absence
     * de contexte graphique ou d'impossibilité d'ouverture d'un
     * contexte OpenGL (au moins 3.2) */
    return 1;
  }
  elEnable(EL_SHADING);
  elEnable(EL_TEXTURE);
  _terre    = elGenTexture("images/terre.bmp");
  _feuilles = elGenTexture("images/feuilles.bmp");
  _parquet  = elGenTexture("images/parquet.bmp");
  _quad = elQuad();
  _cube = elCube();
  _sphere = elSphere(11, 11);
  /* on affecte des textures aux surfaces */
  elSetTexture(_quad, _terre);
  elSetTexture(_cube, _feuilles);
  elSetTexture(_sphere, _parquet);
  /* on change les couleurs par défaut */
  /* possible de le faire dans dis */
  elSetColor(_quad, rouge);
  elSetColor(_cube, jaune);
  elSetColor(_sphere, bleu);
  atexit(_quit);
  gl4duwDisplayFunc(dis);
  gl4duwMainLoop();
  return 0;
}

void _quit(void) {
  /* des choses à libérer ... */
  if(_quad) {
    elFreeSurface(_quad);
    _quad = NULL;
  }
  if(_cube) {
    elFreeSurface(_cube);
    _cube = NULL;
  }
  if(_sphere) {
    elFreeSurface(_sphere);
    _sphere = NULL;
  }
  if(_terre) {
    elFreeTexture(_terre);
    _terre = NULL;
  }
  if(_feuilles) {
    elFreeTexture(_feuilles);
    _feuilles = NULL;
  }
  if(_parquet) {
    elFreeTexture(_parquet);
    _parquet = NULL;
  }
}
