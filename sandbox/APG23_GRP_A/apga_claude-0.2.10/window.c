/*!\file window.c
 * \brief test de la lib claude
 * \author Farès Belhadj et le groupe A de L2 (23/24)
 * \date octobre-décembre 2023
 */

#include "claude.h"

#define WW 640
#define WH 480

static void _quit(void);

static int _laby[] = {
  1, 0, 1, 1, 1, 1, 1, 1, 1,
  1, 0, 0, 0, 0, 0, 0, 0, 1,  
  1, 1, 1, 1, 0, 1, 1, 1, 1,  
  1, 0, 0, 0, 0, 0, 0, 0, 1,  
  1, 1, 0, 1, 1, 1, 1, 1, 1,  
  1, 0, 0, 0, 0, 0, 0, 0, 0,  
  1, 1, 1, 1, 1, 1, 1, 1, 1
};

static int _lw = 9, _lh = 7;

static surface_t * _cube = NULL;
static texture_t * _craq = NULL;

static void dis(void) {
  static const int viewport[] = { 0, 0, WW, WH};
  static float a = 0.0f;
  surface_t * c = _cube, * cp = duplicate_surface(c);
  vec4 vert  = { 0.5f, 1.0f, 0.5f, 1.0f };
  claude_clear();

  mat4 projection, view, model;
  frustum(projection, -1.0f, 1.0f, -1.0f * 3.0f / 4.0f, 1.0f * 3.0f / 4.0f, 2.0f, 150.0f);
  mat4identite(view);

  /* même vue pour tous */
  translate(view, 0.0f, 0.0f, -4.0f);

  /* le cube est "petit" et devant à gauche et tourne sur xyz */
  for(int i = 0; i < _lh; ++i) {
    for(int j = 0; j < _lw; ++j) {
      if(_laby[i * _lw + j] == 0) continue;
      mat4identite(model);
      translate(model, (j - 4.5f) * 2.0f, (i - 3.5f) * 2.0f, -30.0f);
      rotate(model, a + (i * _lw + j) * 15.3f, 1, 0, 0);
      claude_apply_transforms(model, view, projection, c, cp);
      colorize_surface(cp, vert);
      apply_texture(_craq);
      claude_draw(cp, viewport);
    }
  }

  update_screen();

  free_surface(cp);

  static int t0 = 0;
  int t = SDL_GetTicks();
  float dt = (t - t0) / 1000.0f;
  t0 = t;
  /* toujours faire les déplacements en fonction du temps */
  a += 360.0f * dt / 5.0f; /* ici, pour illustrer, un tour complet toutes les 5 secodes */
}

/*!\brief créé la fenêtre, un screen 2D effacé en noir et lance une
 *  boucle infinie.*/
int main(int argc, char ** argv) {
  /* tentative de création d'une fenêtre pour GL4Dummies */
  if(!claude_init(argc, argv, /* args du programme */
		  "Claude' Hello World", /* titre */
		  1100, 825, WW, WH) /* largeur_f, hauteur_f, largeur_e, hauteur_e */) {
    /* ici si échec de la création souvent lié à un problème d'absence
     * de contexte graphique ou d'impossibilité d'ouverture d'un
     * contexte OpenGL (au moins 3.2) */
    return 1;
  }
  gl4duwDisplayFunc(dis);
  _craq = gen_texture("images/craq.bmp");
  _cube = gen_cube();
  atexit(_quit);
  /* boucle infinie pour éviter que le programme ne s'arrête et ferme
   * la fenêtre immédiatement */
  gl4duwMainLoop();
  return 0;
}

static void _quit(void) {
  if(_cube) {
    free_surface(_cube);
    _cube = NULL;
  }
  if(_craq) {
    free_texture(_craq);
    _craq = NULL;
  }
}
