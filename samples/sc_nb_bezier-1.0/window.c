/*!\file window.c 
 *\brief Squelette pour implémenter le dessin d'une courbe de Bézier
 * \author Farès BELHADJ, amsi@up8.edu
 * \date November 30 2022
 */
#include "SDL_opengl.h"
#include <GL4D/gl4dp.h>
#include <GL4D/gl4duw_SDL2.h>

/* type et structure pour les points */
typedef struct point2df point2df_t;
struct point2df {
  GLfloat x, y;
};
/* fonction de dessin appelée par la callback */
static void dessin(void);
/* fonction au click appelée par la callback */
static void mouse(int button, int state, int x, int y);
/* fonction appelée par la callback en mode drag n drop */
static void motion(int x, int y);

/* largeur et hauteur de la fenetre */
static int _ww = 800, _wh = 600;
/* index du point de controle sélectionné à la souris. Aucun si égal à -1 */
static int _selected_cp = -1;
/* nombre de points de controle */
static int _nbcp = 5;
/* points de controle */
static point2df_t _cp[64] = { {10, 10}, {700, 580}, {50, 500}, {770, 20}, {80, 80} };

int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, /* args du programme */
			 "GL4Dummies' Bezier", /* titre */
			 10, 10, _ww, _wh, /* x,y, largeur, heuteur */
			 GL4DW_SHOWN) /* état visible */) {
    /* ici si échec de la création souvent lié à un problème d'absence
     * de contexte graphique ou d'impossibilité d'ouverture d'un
     * contexte OpenGL (au moins 3.2) */
    return 1;
  }
  gl4dpInitScreen();

  gl4duwDisplayFunc(dessin);
  gl4duwMouseFunc(mouse);
  gl4duwMotionFunc(motion);
  gl4duwMainLoop();
  return 0;
}

static inline point2df_t lerp(point2df_t a, point2df_t b, GLfloat t) {
  point2df_t m;
  m.x = (1.0f - t) * a.x + t * b.x;
  m.y = (1.0f - t) * a.y + t * b.y;
  return m;
}  

static point2df_t casteljau(point2df_t *p, int n, GLfloat t) {
  assert(n > 0);
  if (n == 1) /* si ne reste qu'un, c'est lui */
    return p[0];
  point2df_t *np = malloc((n - 1) * sizeof *np);
  assert(np);
  for (int i = 0; i < n - 1; i++) {
    np[i] = lerp(p[i], p[i + 1], t);
  }
  point2df_t f = casteljau(np, n - 1, t);
  free(np);
  return f;
}  

void dessin(void) {
  int i;
  gl4dpClearScreenWith(RGB(255, 255, 255));
  for(i = 0; i < _nbcp; ++i) {
    gl4dpSetColor(RGB(255 - i, 0, 0));
    gl4dpFilledCircle((int)_cp[i].x, (int)_cp[i].y, 7);
  }
  gl4dpSetColor(0);
  for (GLfloat t = 0.0f, dt = 0.02f; t <= 1.0f - dt; t += dt) {
    point2df_t a = casteljau(_cp, _nbcp, t);
    point2df_t b = casteljau(_cp, _nbcp, t + dt);
    gl4dpLine(a.x, a.y, b.x, b.y);
  }
  /* fonction permettant de raffraîchir l'ensemble de la fenêtre*/
  gl4dpUpdateScreen(NULL);
}

void mouse(int button, int state, int x, int y) {
  if(!state)
    _selected_cp = -1;
  else if(button == GL4D_BUTTON_LEFT) { /* si bouton gauche down */
    y = _wh - 1 - y;
    if(IN_SCREEN(x, y)) {
      GLuint coul = gl4dpGetPixel(x, y);
      if(GREEN(coul) == 0 && BLUE(coul) == 0)
	_selected_cp = 255 - RED(coul);
    }
  }
}

void motion(int x, int y) {
  if(_selected_cp >= 0) {
    y = _wh - 1 - y;
    if(IN_SCREEN(x, y)) {
      _cp[_selected_cp].x = x;
      _cp[_selected_cp].y = y;
    }
  }
}
