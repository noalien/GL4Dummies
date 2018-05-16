/*!\file window.c
 * \brief géométries lumière diffuse et transformations de base en GL4Dummies
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date April 15 2016 */
#include <stdio.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4df.h>
#include <GL4D/gl4duw_SDL2.h>
/* Prototypes des fonctions statiques contenues dans ce fichier C */
static void initGL(void);
static void initData(void);
static void resize(int w, int h);
static void draw(void);
static void quit(void);
/*!\brief dimensions de la fenêtre */
static int _windowWidth = 800, _windowHeight = 600;
/*!\brief identifiant du programme GLSL */
static GLuint _pId = 0;
/*!\brief quelques objets géométriques */
static GLuint _sphere = 0, _cube = 0, _quad = 0;
/*!\brief La fonction principale créé la fenêtre d'affichage,
 * initialise GL et les données, affecte les fonctions d'événements et
 * lance la boucle principale d'affichage.*/
int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "GL4Dummies", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
			 _windowWidth, _windowHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN))
    return 1;
  initGL();
  initData();
  atexit(quit);
  gl4duwResizeFunc(resize);
  gl4duwDisplayFunc(draw);
  gl4duwMainLoop();
  return 0;
}
/*!\brief initialise les paramètres OpenGL */
static void initGL(void) {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glClearColor(1.0f, 0.7f, 0.7f, 0.0f);
  _pId  = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  resize(_windowWidth, _windowHeight);
}
/*!\brief initialise les données */
static void initData(void) {
  _sphere = gl4dgGenSpheref(30, 30);
  _cube = gl4dgGenCubef();
  _quad = gl4dgGenQuadf();
}
/*!\brief Cette fonction paramétre la vue (viewport) OpenGL en
 * fonction des dimensions de la fenêtre.*/
static void resize(int w, int h) {
  _windowWidth  = w; _windowHeight = h;
  glViewport(0, 0, _windowWidth, _windowHeight);
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.5, 0.5, -0.5 * _windowHeight / _windowWidth, 0.5 * _windowHeight / _windowWidth, 1.0, 1000.0);
  //gl4duOrthof(-0.5, 0.5, -0.5 * _windowHeight / _windowWidth, 0.5 * _windowHeight / _windowWidth, 0.0, 1000.0);
  gl4duBindMatrix("modelViewMatrix");
}
/*!\brief dessine dans le contexte OpenGL actif. */
static void draw(void) {
  static GLfloat a = 0;
  GLfloat rouge[] = {1, 0, 0, 1}, vert[] = {0, 1, 0, 1}, bleu[] = {0, 0, 1, 0.7};
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();
  glUseProgram(_pId);
  gl4duTranslatef(0, 0, -10.0);
  gl4duPushMatrix(); {
    gl4duTranslatef(0, -2.0, 0.0);
    gl4duRotatef(-90, 1, 0, 0);
    gl4duScalef(3, 3, 3);
    gl4duSendMatrices();
  } gl4duPopMatrix();
  glUniform4fv(glGetUniformLocation(_pId, "couleur"), 1, vert);
  gl4dgDraw(_quad);

  gl4duTranslatef(0, -1, 0);
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_pId, "couleur"), 1, rouge);
  gl4dgDraw(_sphere);

  gl4duRotatef(a++, 0, 1, 0);
  gl4duTranslatef(3, 0, 0);
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_pId, "couleur"), 1, bleu);
  gl4dgDraw(_cube);
  /* Décommenter pour avoir un rendu sympathique pour pas cher :)
     gl4dfBlur(0, 0, 5, 1, 0, GL_FALSE);
     gl4dfSobelSetMixMode(GL4DF_SOBEL_MIX_MULT);
     gl4dfSobel(0, 0, GL_FALSE); */
}
/*!\brief appelée au moment de sortir du programme (atexit), libère les éléments utilisés */
static void quit(void) {
  gl4duClean(GL4DU_ALL);
}
