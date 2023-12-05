/*!\file window.c
 * \brief test de la lib claude
 * \author Farès Belhadj et le groupe A de L2 (23/24)
 * \date octobre-décembre 2023
 */

#include "claude.h"
#include "primitives.h"

#define WW 640
#define WH 480

static void _quit(void);

static surface_t * _sphere_11_11 = NULL, * _plan = NULL;

static void dis(void) {
  static const int viewport[] = { 0, 0, WW, WH};
  surface_t * s = _sphere_11_11, * sp = duplicate_surface(s);
  surface_t * p = _plan, * pp = duplicate_surface(p);
  
  claude_clear();

  mat4 projection, view, model;
  frustum(projection, -1.0f, 1.0f, -1.0f * 3.0f / 4.0f, 1.0f * 3.0f / 4.0f, 2.0f, 5.0f);
  mat4identite(view);
  mat4identite(model);

  static float a = 0.0f, b = 0.0f, c = 0.0f;
  translate(view, 0.0f, 0.0f, -4.0f);
  rotate(view, 20, 1.0f, 0.0f, 0.0f);
  rotate(model, a += 1.0f, 0.0f, 1.0f, 0.0f);
  
  claude_apply_transforms(model, view, projection, s, sp);
  claude_draw(sp, viewport);
  
  mat4identite(model);
  translate(model, 0.0f, 0.0f, 1.0f);
  rotate(model, b += 0.1f, 0.0f, 0.0f, 1.0f);
  rotate(model, c += 0.5f, 0.0f, 1.0f, 0.0f);

  claude_apply_transforms(model, view, projection, p, pp);
  claude_draw(pp, viewport);
  
  update_screen();

  free_surface(sp);
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
  apply_texture("images/leo.bmp");
  _sphere_11_11 = gen_sphere(11, 11);
  _plan = gen_quad();
  atexit(_quit);
  /* boucle infinie pour éviter que le programme ne s'arrête et ferme
   * la fenêtre immédiatement */
  gl4duwMainLoop();
  return 0;
}

static void _quit(void) {
  if(_sphere_11_11) {
    free_surface(_sphere_11_11);
    _sphere_11_11 = NULL;
  }
  if(_plan) {
    free_surface(_plan);
    _plan = NULL;
  }
}
