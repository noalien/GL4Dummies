/*!\file window.c
 * \brief géométries lumière diffuse et transformations de base en
 * GL4Dummies + simulation de mobiles et gestion du picking des objets
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date March 10 2017 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4df.h>
#include <GL4D/gl4duw_SDL2.h>
#include "mobile.h"
/* Prototypes des fonctions statiques contenues dans ce fichier C */
static void init(void);
static void mouse(int button, int state, int x, int y);
static void motion(int x, int y);
static void draw(void);
static void quit(void);
/*!\brief dimensions de la fenêtre */
static int _windowWidth = 800, _windowHeight = 600;
/*!\brief identifiant du programme GLSL */
static GLuint _pId = 0;
/*!\brief quelques objets géométriques */
static GLuint _sphere = 0, _quad = 0;
/*!\brief scale du plan */
static GLfloat _plan_s = 5.0f;
/*!\brief Framebuffer Object */
static GLuint _fbo = 0;
/*!\brief Texture recevant la couleur */
static GLuint _colorTex = 0;
/*!\brief Texture recevant la profondeur */
static GLuint _depthTex = 0;
/*!\brief Texture recevant les identifiants d'objets */
static GLuint _idTex = 0;
/*!\brief nombre de mobiles créés dans la scène */
static GLuint _nb_mobiles = 10;
/*!\brief identifiant du mobile sélectionné */
static int _picked_mobile = -1;
/*!\brief copie CPU de la mémoire texture d'identifiants */
static GLfloat * _pixels = NULL;
/*!\brief La fonction principale créé la fenêtre d'affichage,
 * initialise GL et les données, affecte les fonctions d'événements et
 * lance la boucle principale d'affichage.*/
int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "GL4D - Picking", 0, 0, _windowWidth, _windowHeight, GL4DW_SHOWN))
    return 1;
  init();
  atexit(quit);
  gl4duwMouseFunc(mouse);
  gl4duwMotionFunc(motion);
  gl4duwIdleFunc(mobileMove);
  gl4duwDisplayFunc(draw);
  gl4duwMainLoop();
  return 0;
}
/*!\brief initialise les paramètres OpenGL */
static void init(void) {
  glEnable(GL_DEPTH_TEST);
  _pId  = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");

  glViewport(0, 0, _windowWidth, _windowHeight);
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.5, 0.5, -0.5 * _windowHeight / _windowWidth, 0.5 * _windowHeight / _windowWidth, 1.0, 1000.0);
  gl4duBindMatrix("modelViewMatrix");

  _sphere = gl4dgGenSpheref(30, 30);
  _quad = gl4dgGenQuadf();
  mobileInit(_nb_mobiles, _plan_s, _plan_s);

  /* Création et paramétrage de la Texture recevant la couleur */
  glGenTextures(1, &_colorTex);
  glBindTexture(GL_TEXTURE_2D, _colorTex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _windowWidth, _windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

  /* Création et paramétrage de la Texture recevant la profondeur */
  glGenTextures(1, &_depthTex);
  glBindTexture(GL_TEXTURE_2D, _depthTex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, _windowWidth, _windowHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);

  /* Création et paramétrage de la Texture recevant les identifiants d'objets */
  glGenTextures(1, &_idTex);
  glBindTexture(GL_TEXTURE_2D, _idTex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, _windowWidth, _windowHeight, 0, GL_RED, GL_UNSIGNED_INT, NULL);

  /* Création et paramétrage (attacher les textures) du Framebuffer Object */
  glGenFramebuffers(1, &_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,    GL_TEXTURE_2D, _colorTex, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,    GL_TEXTURE_2D,    _idTex, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,     GL_TEXTURE_2D, _depthTex, 0);

  _pixels = malloc(_windowWidth * _windowHeight * sizeof *_pixels);
  assert(_pixels);
}

static void mouse(int button, int state, int x, int y) {
  if(button == GL4D_BUTTON_LEFT) {
    y = _windowHeight - y;
    /* ça n'est plus glReadPixels */
    glBindTexture(GL_TEXTURE_2D, _idTex);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, _pixels);
    if(x >= 0 && x < _windowWidth && y >=0 && y < _windowHeight)
      _picked_mobile = (int)((_nb_mobiles + 1.0f) * _pixels[y * _windowWidth + x]) - 2;
    if(_picked_mobile >= 0 && _picked_mobile < _nb_mobiles)
      mobileSetFreeze(_picked_mobile, state);
    if(!state)
      _picked_mobile = -1;
  }
}

static void motion(int x, int y) {
  if(_picked_mobile >= 0 && _picked_mobile < _nb_mobiles) {
    GLfloat m[16], tmpp[16], tmpm[16], * gl4dm;
    /* p est la coordonnée de la souris entre -1 et +1 */
    GLfloat p[] = { 2.0f * x / (GLfloat)_windowWidth - 1.0f,
		    -(2.0f * y / (GLfloat)_windowHeight - 1.0f), 
		    0.0f, 1.0 }, ip[4], mcoords[4] = {0, 0, 0, 1}, mscr[4];
    /* récupération des coordonnées spaciales du mobile */
    mobileGetCoords(_picked_mobile, mcoords);
    /* copie de la matrice de projection dans tmpp */
    gl4duBindMatrix("projectionMatrix");
    gl4dm = gl4duGetMatrixData();
    memcpy(tmpp, gl4dm, sizeof tmpp);
    /* copie de la matrice de modelisation dans tmpm */
    gl4duBindMatrix("modelViewMatrix");
    gl4dm = gl4duGetMatrixData();
    memcpy(tmpm, gl4dm, sizeof tmpm);
    /* m est tmpp x tmpm */
    MMAT4XMAT4(m, tmpp, tmpm);
    /* modelisation et projection de la coordonnée du mobile dans mscr */
    MMAT4XVEC4(mscr, m, mcoords);
    MVEC4WEIGHT(mscr);
    /* ajout de la profondeur à l'écran du mobile comme profondeur du click */
    p[2] = mscr[2];
    /* inversion de m */
    MMAT4INVERSE(m);
    /* ip est la tranformée inverse de la coordonnée du click (donc coordonnée spaciale du click) */
    MMAT4XVEC4(ip, m, p);
    MVEC4WEIGHT(ip);
    /* affectation de ip comme nouvelle coordonnée spaciale du mobile */
    mobileSetCoords(_picked_mobile, ip);
  }
}

/*!\brief dessine dans le contexte OpenGL actif. */
static void draw(void) {
  GLfloat vert[] = {0, 1, 0, 1};
  GLenum renderings[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
  /* un seul rendu GL_COLOR_ATTACHMENT1 + effacement 0 */
  glDrawBuffers(1, &renderings[1]);
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT);
  /* un seul rendu GL_COLOR_ATTACHMENT0 + effacement couleur et depth */
  glDrawBuffers(1, renderings);
  glClearColor(1.0f, 0.7f, 0.7f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  /* deux rendus GL_COLOR_ATTACHMENT0 et GL_COLOR_ATTACHMENT1 */
  glDrawBuffers(2, renderings);

  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();
  glUseProgram(_pId);
  gl4duLookAtf(13, 2, 15, 0, 0, 0, 0, 1, 0);
  //gl4duTranslatef(0, -2.0, -15.0);
  gl4duPushMatrix(); {
    gl4duRotatef(-90, 1, 0, 0);
    gl4duScalef(_plan_s, _plan_s, _plan_s);
    gl4duSendMatrices();
  } gl4duPopMatrix();
  glUniform4fv(glGetUniformLocation(_pId, "couleur"), 1, vert);
  glUniform1i(glGetUniformLocation(_pId, "id"), 1);
  glUniform1i(glGetUniformLocation(_pId, "nb_mobiles"), _nb_mobiles);
  gl4dgDraw(_quad);
  mobileDraw(_sphere);

  glBindTexture(GL_TEXTURE_2D, 0);
  glBindTexture(GL_TEXTURE_2D, _colorTex);

  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glBlitFramebuffer(0, 0, _windowWidth, _windowHeight, 0, 0, _windowWidth, _windowHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
  glBlitFramebuffer(0, 0, _windowWidth, _windowHeight, 0, 0, _windowWidth, _windowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
/*   gl4dfScattering(0, 0, 5, 0, 0, GL_FALSE); */
/*   gl4dfMedian(0, 0, 5, GL_FALSE); */
/*   gl4dfBlur(0, 0, 70, 1, 0, GL_FALSE); */
/*   gl4dfSobelSetMixMode(GL4DF_SOBEL_MIX_MULT); */
/*   gl4dfSobel(0, 0, GL_FALSE); */
}
/*!\brief appelée au moment de sortir du programme (atexit), libère les éléments utilisés */
static void quit(void) {
  if(_fbo) {
    glDeleteTextures(1, &_colorTex);
    glDeleteTextures(1, &_depthTex);
    glDeleteTextures(1, &_idTex);
    glDeleteFramebuffers(1, &_fbo);
    _fbo = 0;
  }
  if(_pixels) {
    free(_pixels);
    _pixels = NULL;
  }
  gl4duClean(GL4DU_ALL);
}
