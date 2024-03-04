#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4dg.h>
#include <SDL_image.h>
#include <stdio.h>

static void init(void);
/* TODO : gérer le retaillage de la fenêtre */
/* static void resize(int width, int height); */
static void draw(void);
static void sortie(void);

static GLuint _wW = 800, _wH = 600;
static GLuint _quadId = 0;
static GLuint _pId_conv = 0;
static GLuint _fbo = 0;
static GLuint _tex[2] = { 0, 0 };


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
  SDL_Surface * s;
  glClearColor(0.5f, 0.0f, 0.0f, 1.0f);
  _quadId = gl4dgGenQuadf();
  _pId_conv = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/blur.fs", NULL);

  glGenFramebuffers(1, &_fbo);
  glGenTextures(2, _tex);
  glBindTexture(GL_TEXTURE_2D, _tex[0]);
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
  s = IMG_Load("images/gl4d.png");
  if(s != NULL) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, s->w, s->h, 0, GL_RGB, GL_UNSIGNED_BYTE, s->pixels);
    SDL_FreeSurface(s);
  } else
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _wW, _wH, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);



  glBindTexture(GL_TEXTURE_2D, _tex[1]);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _wW, _wH, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);




  glBindTexture(GL_TEXTURE_2D, 0);
}

void draw(void) {
  static int i = 0;
  GLfloat pas[] = { 1.0f / (_wW - 1.0f), 1.0f / (_wH - 1.0f)};

  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _tex[(i + 1) % 2],  0);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(_pId_conv);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _tex[i % 2]);
  glUniform1i(glGetUniformLocation(_pId_conv, "tex"), 0);
  glUniform2fv(glGetUniformLocation(_pId_conv, "pas"), 1, pas);
  gl4dgDraw(_quadId);
  glUseProgram(0);

  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glBlitFramebuffer(0, 0, _wW - 1, _wH - 1, 0, 0, _wW - 1, _wH - 1, GL_COLOR_BUFFER_BIT, GL_NEAREST);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  ++i;
  SDL_Delay(10);
}

void sortie(void) {
  gl4duClean(GL4DU_ALL);
  if(_fbo) {
    glDeleteFramebuffers(1, &_fbo);
    _fbo = 0;
  }
  if(_tex[0]) {
    glDeleteTextures(2, _tex);
    _tex[0] = 0;
  }
}
