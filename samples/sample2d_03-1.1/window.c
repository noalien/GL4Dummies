/*!\file window.c
 *
 * \brief Mobiles simulation with 100% GPU rendering.
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr 
 * \date February 15 2017
 */
#include <assert.h>
#include <GL4D/gl4dg.h>
#include <GL4D/gl4duw_SDL2.h>
#include "mobile.h"

static void init(void);
static void resize(int w, int h);
static void keydown(int keycode);
static void draw(void);
static void quit(void);

/*!\brief window's width and height */
static int _windowWidth = 600, _windowHeight = 600;
/*!\brief GLSL program Id */
static GLuint _pId = 0;
/*!\brief mobiles' texture Id for storing mobile coordinates */
static GLuint _tId = 0;
/*!\brief A generated Quad Id */
static GLuint _quad = 0;
/*!\brief number of mobiles to simulate */
static const int _nb_mobiles = 200;
/*!\brief toggle Voronoï mode */
static GLboolean _voronoi = GL_FALSE;

/*!\brief main function, creates the window, initialise OpenGL
 *  parameters and objects, sets GL4Dummies callback function and
 *  starts the main loop.
 */
int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "GL4Dummies", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
			 _windowWidth, _windowHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN))
    return 1;
  init();
  atexit(quit);
  gl4duwResizeFunc(resize);
  gl4duwKeyDownFunc(keydown);
  gl4duwDisplayFunc(draw);
  gl4duwMainLoop();
  return 0;
}

/*!\brief initialise OpenGL parameters and data. */
static void init(void) {
  /* setting OpenGL clear color (for next glClear) */
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  /* loading and compiling a GLSL program composed with a vertex shader and a fragment shader */
  _pId = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic.fs", NULL);
  /* setting OpenGL viewPort by calling resize (see below) */
  resize(_windowWidth, _windowHeight);
  /* generating a Quad (Vertex coords + Vertex Normal + Vertex TexCoord */
  _quad = gl4dgGenQuadf();
  /* creating a 1D texture to store mobile coords */
  glGenTextures(1, &_tId);
  glBindTexture(GL_TEXTURE_1D, _tId);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32F, 1, 0, GL_RGBA, GL_FLOAT, NULL);
  glBindTexture(GL_TEXTURE_1D, 0);
  /* calling mobileInit */
  mobileInit(_nb_mobiles, _windowWidth, _windowHeight);
}

/*!\brief sets the OpenGL viewport according to the window width and height.
 */
static void resize(int w, int h) {
  _windowWidth  = w;
  _windowHeight = h;
  glViewport(0, 0, _windowWidth, _windowHeight);
}

/*!\brief manages keyboard-down event */
static void keydown(int keycode) {
  switch(keycode) {
  case SDLK_ESCAPE:
  case 'q':
    exit(0);
  case 'v':
    _voronoi = !_voronoi;
    break;
  case 'r':
    mobileInit(_nb_mobiles, _windowWidth, _windowHeight);
    break;
  }
}

/*!\brief mobile simulation and draw
 */
static void draw(void) {
  static GLfloat toto = 0;
  GLfloat * f = malloc(_nb_mobiles * 8 * sizeof *f), step = 1.0 / (_nb_mobiles * 2);
  assert(f);
  glDisable(GL_DEPTH_TEST);
  glUseProgram(_pId);

  glClear(GL_COLOR_BUFFER_BIT);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_1D, _tId);
  mobileMove();
  mobile2texture(f);
  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32F, 2 * _nb_mobiles, 0, GL_RGBA, GL_FLOAT, f);
  free(f);
  glUniform1i(glGetUniformLocation(_pId, "voronoi"), _voronoi);
  glUniform1i(glGetUniformLocation(_pId, "mobiles"), 0);
  glUniform1f(glGetUniformLocation(_pId, "step"), step);
  glUniform1f(glGetUniformLocation(_pId, "toto"), 0.5 + (sin(toto) + 1.0) / 4.0);

  gl4dgDraw(_quad);
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_1D, 0);
  glUseProgram(0);
  toto += 0.01;
}

/*!\brief called at exit and delete and clean used data. */
static void quit(void) {
  if(_tId) 
    glDeleteTextures(1, &_tId);
  gl4duClean(GL4DU_ALL);
}
