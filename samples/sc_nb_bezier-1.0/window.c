/*!\file window.c 
 *\brief Squelette pour implémenter le dessin d'une courbe de Bézier
 * \author Farès BELHADJ, amsi@up8.edu
 * \date November 30 2022
 */
#include "SDL_opengl.h"
#include <GL4D/gl4dp.h>
#include <GL4D/gl4duw_SDL2.h>

#include <GL4D/gl4du.h>
#include <GL4D/gl4duw_SDL2.h>
/* type et structure pour les points */
typedef struct point3df point3df_t;
struct point3df {
  GLfloat x, y, z;
};
/* fonction de dessin appelée par la callback */
static void dessin(void);
static void init(void); 
/* fonction au click appelée par la callback */
static void mouse(int button, int state, int x, int y);
/* fonction appelée par la callback en mode drag n drop */
static void motion(int x, int y);

/* largeur et hauteur de la fenetre */
static int _ww = 800, _wh = 600;
/* index du point de controle sélectionné à la souris. Aucun si égal à -1 */
static int _selected_cp = -1;
/* nombre de points de controle */
static int _nbcp = 4;
static GLuint _sphereId = 0; 
/* points de controle */
static point3df_t _cp[64] = {
  { -0.974969, -0.966611 },
  { 0.752190, 0.936561 },
  { -0.874844, 0.669449 },
  { 0.927409, -0.933222 } };

static point3df_t _cp_anse[64] = {
    { -15.893f, -6.998f, -0.419f },
    {   8.463f, -16.127f,  4.954f },
    {   3.418f, -4.704f, 19.922f },
    {  -7.930f, -5.648f, 15.210f }
};

static point3df_t _cp_bec[64] = {
    { -9.129f, -10.929f,  0.010f },
    {  1.469f, -10.475f,  0.054f },
    { -1.086f, -0.751f,  -0.234f },
    { -4.839f,  10.697f, -1.198f },
    {  5.253f,  11.916f, -4.056f }
};



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
  init();
  gl4dpInitScreen();

  gl4duwDisplayFunc(dessin);
  gl4duwMouseFunc(mouse);
  gl4duwMotionFunc(motion);
  gl4duwMainLoop();
  return 0;
}

void init(void) {
  /* Création du programme shader (voir le dossier shader) */
  // _pId = gl4duCreateProgram("<vs>shaders/light.vs", "<fs>shaders/light.fs", NULL);
  /* Créer un cube */
  _sphereId = gl4dgGenSpheref(32,32);
  /* Set de la couleur (RGBA) d'effacement OpenGL */
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
  /* activation du test de profondeur afin de prendre en compte la
   * notion de devant-derrière. */
  //  glEnable(GL_DEPTH_TEST);
  /* Création des matrices GL4Dummies, une pour la projection, une
   * pour la modélisation et une pour la vue */
  /* gl4duGenMatrix(GL_FLOAT, "projectionMatrix"); */
  /* gl4duGenMatrix(GL_FLOAT, "modelMatrix"); */
  /* gl4duGenMatrix(GL_FLOAT, "viewMatrix"); */
  /* resize(_ww, _wh); */
}



static inline point3df_t lerp(point3df_t a, point3df_t b, GLfloat t) {
  point3df_t m;
  m.x = (1.0f - t) * a.x + t * b.x;
  m.y = (1.0f - t) * a.y + t * b.y;
  return m;
}  

static point3df_t casteljau(point3df_t *p, int n, GLfloat t) {
  assert(n > 0);
  if (n == 1) /* si ne reste qu'un, c'est lui */
    return p[0];
  point3df_t *np = malloc((n - 1) * sizeof *np);
  assert(np);
  for (int i = 0; i < n - 1; i++) {
    np[i] = lerp(p[i], p[i + 1], t);
  }point3df_t f = casteljau(np, n - 1, t);
  free(np);
  return f;
}  

void dessin(void) {
  int i;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // glUseProgram(_pId); 
  
  gl4dpClearScreenWith(RGB(255, 255, 255));
  for(i = 0; i < _nbcp; ++i) {
    int x = (_ww - 1) * (_cp[i].x + 1.0f) / 2.0f;
    int y = (_wh - 1) * (_cp[i].y + 1.0f) / 2.0f;
    gl4dpSetColor(RGB(255 - i, 0, 0));
    gl4dpFilledCircle(x, y, 7);
  }


    for(i = 0; i < _nbcp; ++i) {
    int x = (_ww - 1) * (_cp_anse[i].x + 1.0f) / 2.0f;
    int y = (_wh - 1) * (_cp_anse[i].y + 1.0f) / 2.0f;
    gl4dpSetColor(RGB(255 - i, 0, 0));
    gl4dpFilledCircle(x, y, 7);
  }

    for(i = 0; i < _nbcp; ++i) {
    int x = (_ww - 1) * (_cp_bec[i].x + 1.0f) / 2.0f;
    int y = (_wh - 1) * (_cp_bec[i].y + 1.0f) / 2.0f;
    gl4dpSetColor(RGB(255 - i, 0, 0));
    gl4dpFilledCircle(x, y, 7);
  }



  gl4dpSetColor(0);
  for (GLfloat t = 0.0f, dt = 0.02f; t <= 1.0f - dt; t += dt) {
    point3df_t a = casteljau(_cp, _nbcp, t);
    point3df_t b = casteljau(_cp, _nbcp, t + dt);
    int x0 = (_ww - 1) * (a.x + 1.0f) / 2.0f;
    int y0 = (_wh - 1) * (a.y + 1.0f) / 2.0f;
    int x1 = (_ww - 1) * (b.x + 1.0f) / 2.0f;
    int y1 = (_wh - 1) * (b.y + 1.0f) / 2.0f;
    gl4dpLine(x0, y0, x1, y1);
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
      _cp[_selected_cp].x = 2.0f * x / (_ww - 1.0f) - 1.0f;
      _cp[_selected_cp].y = 2.0f * y / (_wh - 1.0f) - 1.0f;
    }
  }
}
