/*!\file window.c
 * \brief géométries lumière diffuse, transformations de base et chargement de textures en GL4Dummies
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date May 13 2018 */
#include <stdio.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4df.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL_image.h>
#include <GL4D/gl4dh.h>
#include "audioHelper.h"
/* Prototypes des fonctions statiques contenues dans ce fichier C */
static void init(void);
static void loadTexture(GLuint id, const char * filename);
static void draw(void);
static void quit(void);
/*!\brief identifiant du programme GLSL */
static GLuint _pId = 0, _pId2 = 0;
/*!\brief quelques objets géométriques */
static GLuint _sphere = 0, _cube = 0, _quad = 0, _torus = 0, _tex[5] = {0};
/*!\brief initialise les paramètres OpenGL et les données */
static void init(void) {
  GLint vp[4];
  glGetIntegerv(GL_VIEWPORT, vp);

  _pId  = gl4duCreateProgram("<vs>shaders/depTex.vs", "<fs>shaders/depTex.fs", NULL);
  _pId2  = gl4duCreateProgram("<vs>shaders/effet.vs", "<fs>shaders/effet.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  _sphere = gl4dgGenSpheref(30, 30);
  _cube = gl4dgGenCubef();
  _quad = gl4dgGenQuadf();
  _torus = gl4dgGenTorusf(300, 30, 0.1f);
  /* génération de plusieurs identifiants de texture */
  glGenTextures(sizeof _tex / sizeof *_tex, _tex);
  /* chargement et transfert des images dans des textures OpenGL */
  loadTexture(_tex[0], "images/sphere.jpg");
  loadTexture(_tex[1], "images/cube.jpg");
  loadTexture(_tex[2], "images/quad.jpg");
  loadTexture(_tex[3], "images/torus.jpg");
  /* une dernière texture vide pour y faire des calculs d'effets */
  glBindTexture(GL_TEXTURE_2D, _tex[4]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, vp[2] / 4, vp[3] / 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glEnable(GL_TEXTURE_2D);
}
static void loadTexture(GLuint id, const char * filename) {
  SDL_Surface * t;
  glBindTexture(GL_TEXTURE_2D, id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  if( (t = IMG_Load(filename)) != NULL ) {
#ifdef __APPLE__
    int mode = t->format->BytesPerPixel == 4 ? GL_BGRA : GL_BGR;
#else
    int mode = t->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;
#endif       
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t->w, t->h, 0, mode, GL_UNSIGNED_BYTE, t->pixels);
    SDL_FreeSurface(t);
  } else {
    fprintf(stderr, "can't open file %s : %s\n", filename, SDL_GetError());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  }
}

/*!\brief dessine dans le contexte OpenGL actif. */
static void draw(void) {
  static GLfloat a = 0;
  GLint vp[4];
  GLboolean dt = glIsEnabled(GL_DEPTH_TEST);
  glGetIntegerv(GL_VIEWPORT, vp);
  glEnable(GL_DEPTH_TEST);
  glClearColor(1.0f, 0.7f, 0.7f, 1.0f);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.5, 0.5, -0.5 * vp[3] / vp[2], 0.5 * vp[3] / vp[2], 1.0, 1000.0);
  gl4duBindMatrix("modelViewMatrix");

  gl4duLoadIdentityf();
  glUseProgram(_pId);
  glActiveTexture(GL_TEXTURE0);
  glUniform1i(glGetUniformLocation(_pId, "tex"), 0);
  gl4duTranslatef(0, 0, -10.0);
  gl4duPushMatrix(); {
    gl4duTranslatef(0, -2.0, 0.0);
    gl4duRotatef(-90, 1, 0, 0);
    gl4duScalef(3.5f, 3, 3);
    gl4duSendMatrices();
  } gl4duPopMatrix();
  glBindTexture(GL_TEXTURE_2D, _tex[2]);
  gl4dgDraw(_quad);

  gl4duTranslatef(0, -1, 0);
  gl4duSendMatrices();
  glBindTexture(GL_TEXTURE_2D, _tex[0]);
  gl4dgDraw(_sphere);

  gl4duRotatef(a, 0, 1, 0);
  gl4duTranslatef(3, 0, 0);
  gl4duPushMatrix(); {
    gl4duScalef(0.4f, 0.4f, 0.4f);
    gl4duSendMatrices();
  } gl4duPopMatrix();
  glBindTexture(GL_TEXTURE_2D, _tex[1]);
  gl4dgDraw(_cube);
  gl4duRotatef(-3 * a++, 1, 0, 0);
  gl4duSendMatrices();
  glBindTexture(GL_TEXTURE_2D, _tex[3]);
  gl4dgDraw(_torus);

  glUseProgram(0);

  gl4dfConvFrame2Tex(&_tex[4]);
  glUseProgram(_pId2);
  glBindTexture(GL_TEXTURE_2D, _tex[4]);
  glUniform1f(glGetUniformLocation(_pId2, "temps"), gl4dGetElapsedTime());
  gl4dgDraw(_quad);
  gl4dfConvFrame2Tex(&_tex[4]);
  gl4dfConvTex2Frame(_tex[4]);
  
  /* une série de filtres GL4D pour obtenir un effet sur la moitié de l'écran */
/*   glViewport(0, 0, _wW / 2, _wH); */
/*   gl4dfBlur(0, _tex[4], 3, 1, 0, GL_FALSE); */
/*   gl4dfSobelSetResultMode(GL4DF_SOBEL_RESULT_INV_LUMINANCE); */
/*   gl4dfSobelSetMixMode(GL4DF_SOBEL_MIX_NONE); */
/*   gl4dfSobel(_tex[4], _tex[4], GL_FALSE); */
/*   gl4dfScattering(0, 0, 5, 0, 0, GL_FALSE); */
/*   gl4dfMedian(0, 0, 3, GL_FALSE); */
/*   gl4dfOpSetOp(GL4DF_OP_MULT); */
/*   gl4dfOp(0, _tex[4], 0, GL_FALSE); */
/*   glViewport(0, 0, _wW, _wH); */

  if(!dt)
    glDisable(GL_DEPTH_TEST);
}
/*!\brief appelée au moment de sortir du programme (atexit), libère les éléments utilisés */
static void quit(void) {
  /* suppression de plusieurs identifiants de texture */
  glDeleteTextures(sizeof _tex / sizeof *_tex, _tex);
}


void mix0105(int state) {
  switch(state) {
  case GL4DH_INIT:
    init();
    return;
  case GL4DH_FREE:
    quit();
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    return;
  default: /* GL4DH_DRAW */
    draw();
    return;
  }
}
