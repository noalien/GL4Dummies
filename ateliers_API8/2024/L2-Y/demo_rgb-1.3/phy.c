#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4dm.h>
#include <GL4D/gl4dg.h>
#include <GL4D/gl4dp.h>
#include <GL4D/gl4dh.h>
#include "mobile.h"
#include "audioHelper.h"

static void init(void);
static void draw(void);
static void quit(void);
static void scene(GLuint pId, float a);

/* on créé une variable pour stocker l'identifiant du programme GPU */
static GLuint _pId = 0;
/* on créé une variable pour stocker l'identifiant de la géométrie d'un cube GL4D */
static GLuint _cube = 0;
/* on créé une variable pour stocker l'identifiant de la géométrie d'une sphère */
static GLuint _sphere = 0;

void phy(int state) {
  static float volumes[4] = { 0.0f }, volume = 0.0f;
  switch(state) {
  case GL4DH_INIT:
    init();
    return;
  case GL4DH_FREE:
    quit();
    return;
  case GL4DH_UPDATE_WITH_AUDIO: {
    int i, l = ahGetAudioStreamLength() / 2;
    float moyenne;
    Sint16 * stream = (Sint16 *)ahGetAudioStream();
    for(i = 0, moyenne = 0.0f; i < l; ++i) {
      moyenne += abs(stream[i]) / (float)(1 << 15);
    }
    moyenne /= l;
    for(i = 3; i > 0; --i)
      volumes[i - 1] = volumes[i];
    volume = pow(moyenne, 0.5f);
    volumes[3] = volume;
    /* pondérations de la gaussienne */
    volume = 0.012f * volumes[0] + 0.122f * volumes[1] + 0.484f * volumes[2] + 0.766f * volumes[3];
    mobile_pulse(volume);
    return;
  }
  default: /* GL4DH_DRAW */
    mobile_simu();
    draw();
    return;
  }
}

/* initialise des paramètres GL et GL4D */
void init(void) {
  /* paramétrage de la transparence */
  glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
  /* activer le BACKFACE_CULLING */
  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);
  /* on active le test de profondeur */
  glEnable(GL_DEPTH_TEST);
  /* générer un cube en GL4D */
  _cube = gl4dgGenCubef();
  /* générer une sphère en GL4D */
  _sphere = gl4dgGenSpheref(6, 6);
  /* créer un programme GPU pour OpenGL (en GL4D) */
  _pId = gl4duCreateProgram("<vs>shaders/phy.vs", "<fs>shaders/phy.fs", NULL);
  /* créer dans GL4D une matrice qui s'appelle model ; matrice de
     modélisation qu'on retrouvera dans le vertex shader */
  gl4duGenMatrix(GL_FLOAT, "model");
  /* créer dans GL4D une matrice qui s'appelle view ; matrice de
     vue qu'on retrouvera dans le vertex shader */
  gl4duGenMatrix(GL_FLOAT, "view");
  /* créer dans GL4D une matrice qui s'appelle proj ; matrice de
     projection qu'on retrouvera dans le vertex shader */
  gl4duGenMatrix(GL_FLOAT, "proj");

  mobile_init(100);
}

void draw(void) {
  /* une variable pour stocker un ange qui incrémente */
  static float a = 0;
  /* set la couleur d'effacement OpenGL */
  glClearColor(0.7f, 0.7f, 0.0f, 1.0f);
  /* effacer le buffer de couleur (image) et le buffer de profondeur d'OpenGL */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  /* utiliser le programme GPU "_pId" */
  glUseProgram(_pId);

  int WW, WH;
  GLint vp[4];
  /* récupérer la largeur et hauteur de la fenêtre depuis le viewport */
  glGetIntegerv(GL_VIEWPORT, vp);
  WW = vp[2];
  WH = vp[3];

  /****************************/
  /* VUE EN PERSPECTIVE       */
  /* dans portion bas-droite  */
  /****************************/

  /* binder (mettre au premier plan, "en courante" ou "en active") la matrice proj */
  gl4duBindMatrix("proj");
  /* mettre la matrice identité (celle qui ne change rien) dans la matrice courante */
  gl4duLoadIdentityf();
  /* combiner la matrice courante avec une matrice de projection en
     perspective. Voir le support de cours pour les six paramètres :
     left, right, bottom, top, near, far */
  gl4duFrustumf(-0.1f, 0.1f, -(0.1f * WH) / WW, (0.1f * WH) / WW, 0.1f, 1000.0f);

  /* binder (mettre au premier plan, "en courante" ou "en active") la
     matrice view */
  gl4duBindMatrix("view");
  /* mettre la matrice identité (celle qui ne change rien) dans la matrice courante */
  gl4duLoadIdentityf();
  gl4duLookAtf(0.5f, 1.4f, 2.2f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

  scene(_pId, a);

  /* n'utiliser aucun programme GPU (pas nécessaire) */
  glUseProgram(0);
  /* augmenter l'ange a de 1 */
  a += 1;
}

/* appelée lors du exit */
void quit(void) {
  mobile_quit();
}

void scene(GLuint pId, float a) {
  GLboolean dt = glIsEnabled(GL_DEPTH_TEST);
  GLboolean bl = glIsEnabled(GL_BLEND);
  if(!dt) glEnable(GL_DEPTH_TEST);

  /* binder (mettre au premier plan, "en courante" ou "en active") la
     matrice model */
  gl4duBindMatrix("model");
  gl4duLoadIdentityf();
  mobile_draw(pId, _sphere);


  if(!bl) glEnable(GL_BLEND);
  /* on dé-bind la texture */
  glBindTexture(GL_TEXTURE_2D, 0);
  gl4duTranslatef(0.0f, 3.0f, 0.0f);
  gl4duScalef(1.0f, 3.0f, 1.0f);
  glUniform4f(glGetUniformLocation(pId, "couleur"), 0.0f, 0.8f, 0.8f, 0.5f);
  gl4duSendMatrices();
  
  gl4dgDraw(_cube);

  if(!bl) glDisable(GL_BLEND);
  if(!dt) glDisable(GL_DEPTH_TEST);
}

