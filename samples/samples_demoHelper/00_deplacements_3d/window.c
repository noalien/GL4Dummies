/*!\file window.c
 * \brief géométries lumière diffuse et transformations de base en GL4Dummies
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date April 15 2016 */
#include <stdio.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4df.h>
#include <GL4D/gl4duw_SDL2.h>
/* Prototypes des fonctions statiques contenues dans ce fichier C */
static void init(void);
static void resize(int w, int h);
static void draw(void);
static void quit(void);
/*!\brief dimensions de la fenêtre */
static int _wW = 800, _wH = 600;
/*!\brief identifiant du programme GLSL */
static GLuint _pId = 0;
/*!\brief quelques objets géométriques */
static GLuint _sphere = 0, _cube = 0, _quad = 0, _torus = 0;
/*!\brief La fonction principale créé la fenêtre d'affichage,
 * initialise GL et les données, affecte les fonctions d'événements et
 * lance la boucle principale d'affichage.*/
int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "GL4Dummies", 0, 0, 
			 _wW, _wH, GL4DW_RESIZABLE | GL4DW_SHOWN))
    return 1;
  init();
  atexit(quit);
  gl4duwResizeFunc(resize);
  gl4duwDisplayFunc(draw);
  gl4duwMainLoop();
  return 0;
}
/*!\brief initialise les paramètres OpenGL et les données */
static void init(void) {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glClearColor(1.0f, 0.7f, 0.7f, 0.0f);
  _pId  = gl4duCreateProgram("<vs>shaders/dep3d.vs", "<fs>shaders/dep3d.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  resize(_wW, _wH);
  _sphere = gl4dgGenSpheref(30, 30);
  _cube = gl4dgGenCubef();
  _quad = gl4dgGenQuadf();
  _torus = gl4dgGenTorusf(300, 30, 0.1f);
}
/*!\brief Cette fonction paramétre la vue (viewport) OpenGL en
 * fonction des dimensions de la fenêtre.*/
static void resize(int w, int h) {
  _wW  = w; _wH = h;
  glViewport(0, 0, _wW, _wH);
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.5, 0.5, -0.5 * _wH / _wW, 0.5 * _wH / _wW, 1.0, 1000.0);
  //gl4duOrthof(-3.5, 3.5, -3.5 * _wH / _wW, 3.5 * _wH / _wW, 1.0, 1000.0);
  gl4duBindMatrix("modelViewMatrix");
}
/*!\brief dessine dans le contexte OpenGL actif. */
static void draw(void) {
  static GLfloat a = 0;
  GLfloat rouge[] = {1, 0, 0, 1}, vert[] = {0, 1, 0, 1}, bleu[] = {0, 0, 1, 1}, jaune[] = {1, 1, 0, 1};
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();
  glUseProgram(_pId);
  gl4duTranslatef(0, 0, -10.0);
  gl4duPushMatrix(); {
    gl4duTranslatef(0, -2.0, 0.0);
    gl4duRotatef(-90, 1, 0, 0);
    gl4duScalef(3.5f, 3, 3);
    gl4duSendMatrices();
  } gl4duPopMatrix();
  glUniform4fv(glGetUniformLocation(_pId, "couleur"), 1, vert);
  gl4dgDraw(_quad);

  gl4duTranslatef(0, -1, 0);
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_pId, "couleur"), 1, rouge);
  gl4dgDraw(_sphere);
  gl4duPopMatrix();
  gl4dgDraw(_sphere);

  gl4duRotatef(a, 0, 1, 0);
  gl4duTranslatef(3, 0, 0);
  gl4duPushMatrix(); {
    gl4duScalef(0.4f, 0.4f, 0.4f);
    gl4duSendMatrices();
  } gl4duPopMatrix();
  glUniform4fv(glGetUniformLocation(_pId, "couleur"), 1, bleu);
  gl4dgDraw(_cube);
  gl4duRotatef(-3 * a, 1, 0, 0);
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_pId, "couleur"), 1, jaune);
  gl4dgDraw(_torus);
  /* Décommenter pour avoir un rendu sympathique pour pas cher :) */
  /*   gl4dfBlur(0, 0, 5, 1, 0, GL_FALSE); */
  /*   gl4dfSobelSetMixMode(GL4DF_SOBEL_MIX_MULT); */
  /*   gl4dfSobel(0, 0, GL_FALSE); */
  a++;
}
/*!\brief appelée au moment de sortir du programme (atexit), libère les éléments utilisés */
static void quit(void) {
  gl4duClean(GL4DU_ALL);
}
