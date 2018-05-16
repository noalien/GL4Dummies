/*!\file window.c
 * \brief effet 2D en shader
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date May 14 2018 */
#include <stdio.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4df.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL_image.h>
/* Prototypes des fonctions statiques contenues dans ce fichier C */
static void init(void);
static void loadTexture(GLuint id, const char * filename);
static void resize(int w, int h);
static void draw(void);
static void quit(void);
/*!\brief dimensions de la fenêtre */
static int _wW = 800, _wH = 600;
/*!\brief identifiant du programme GLSL */
static GLuint _pId = 0;
/*!\brief identifiants ... */
static GLuint _quad = 0, _tex = 0;
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
  _pId  = gl4duCreateProgram("<vs>shaders/effet.vs", "<fs>shaders/effet.fs", NULL);
  resize(_wW, _wH);
  _quad = gl4dgGenQuadf();
  /* génération d'un identifiant de texture */
  glGenTextures(1, &_tex);
  /* chargement et transfert d'image dans une texture OpenGL */
  loadTexture(_tex, "images/water.jpg");
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
/*!\brief Cette fonction paramétre la vue (viewport) OpenGL en
 * fonction des dimensions de la fenêtre.*/
static void resize(int w, int h) {
  _wW  = w; _wH = h;
  glViewport(0, 0, _wW, _wH);
}
/*!\brief dessine dans le contexte OpenGL actif. */
static void draw(void) {
  double temps = gl4dGetElapsedTime();
  glUseProgram(_pId);
  glActiveTexture(GL_TEXTURE0);
  glUniform1i(glGetUniformLocation(_pId, "tex"), 0);
  glUniform1f(glGetUniformLocation(_pId, "temps"), (GLfloat)temps);
  glBindTexture(GL_TEXTURE_2D, _tex);
  gl4dgDraw(_quad);
}
/*!\brief appelée au moment de sortir du programme (atexit), libère les éléments utilisés */
static void quit(void) {
  /* suppression d'identifiant de texture */
  glDeleteTextures(1, &_tex);
  gl4duClean(GL4DU_ALL);
}
