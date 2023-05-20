#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4dg.h>
#include <stdio.h>

static void init(void);
/* TODO : gérer le retaillage de la fenêtre */
/* static void resize(int width, int height); */
static void draw(void);
static void sortie(void);

static GLuint _wW = 640, _wH = 480;
static GLuint _sphereId = 0;
static GLuint _quadId = 0;
static GLuint _pId = 0;
static GLuint _pId_conv = 0;
static GLuint _fbo = 0;
static GLuint _tex = 0;


int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "Ateliers API8 - convolution", GL4DW_POS_CENTERED, GL4DW_POS_CENTERED,
			 _wW, _wH, GL4DW_OPENGL | GL4DW_RESIZABLE | GL4DW_SHOWN)) {
    fprintf(stderr, "Erreur lors de la création de la fenêtre\n");
    return 1;
  }
  init();
  atexit(sortie);
  gl4duwDisplayFunc(draw);
  gl4duwMainLoop();
  return 0;
}

void init(void) {
  glClearColor(0.5f, 0.0f, 0.0f, 1.0f);
  _sphereId = gl4dgGenSpheref(7, 7);
  _quadId = gl4dgGenQuadf();
  _pId = gl4duCreateProgram("<vs>shaders/hello.vs", "<fs>shaders/hello.fs", NULL);
  _pId_conv = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/sobel.fs", NULL);

  glGenFramebuffers(1, &_fbo);
  glGenTextures(1, &_tex);
  glBindTexture(GL_TEXTURE_2D, _tex);
  /* paramétrer la texture, voir la doc de la fonction glTexParameter
   * sur
   * https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexParameter.xhtml */
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  /* envoi de la donnée texture depuis la RAM CPU vers la RAM GPU voir
   * la doc de glTexImage2D (voir aussi glTexImage1D et glTexImage3D)
   * sur
   * https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml */
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _wW, _wH, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glBindTexture(GL_TEXTURE_2D, 0);


  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _tex,  0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);


}

void draw(void) {
  GLfloat pas[] = { 1.0f / (_wW - 1.0f), 1.0f / (_wH - 1.0f)};
  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(_pId);
  glUniform1f(glGetUniformLocation(_pId, "weight"), 2.0);
  gl4dgDraw(_sphereId);
  glUseProgram(0);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glUseProgram(_pId_conv);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _tex);
  glUniform1i(glGetUniformLocation(_pId_conv, "tex"), 0);
  glUniform2fv(glGetUniformLocation(_pId_conv, "pas"), 1, pas);
  gl4dgDraw(_quadId);
  glUseProgram(0);

  /* glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); */
  /* glBlitFramebuffer(0, 0, _wW - 1, _wH - 1, 0, 0, _wW - 1, _wH - 1, GL_COLOR_BUFFER_BIT, GL_NEAREST); */

  /* glBindFramebuffer(GL_FRAMEBUFFER, 0); */
}

void sortie(void) {
  gl4duClean(GL4DU_ALL);
  if(_fbo) {
    glDeleteFramebuffers(1, &_fbo);
    _fbo = 0;
  }
  if(_tex) {
    glDeleteTextures(1, &_tex);
    _tex = 0;
  }
}
