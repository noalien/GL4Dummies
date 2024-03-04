#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4dg.h>
#include <SDL_image.h>
#include <stdio.h>

static void init(void);
/* TODO : gérer le retaillage de la fenêtre */
/* static void resize(int width, int height); */
static void draw(void);
static void sortie(void);

static GLuint _wW = 1024, _wH = 768;
static GLuint _quadId = 0;
static GLuint _gridId = 0;
static GLuint _pId_conv = 0, _pId_hm = 0;
static GLuint _fbo = 0;
static GLuint _tex[2] = { 0, 0 };


int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "Ateliers API8 - convolution", GL4DW_POS_CENTERED, GL4DW_POS_CENTERED,
			 _wW, _wH, GL4DW_OPENGL | GL4DW_RESIZABLE | GL4DW_SHOWN | GL4DW_FULLSCREEN_DESKTOP)) {
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
  _pId_hm = gl4duCreateProgram("<vs>shaders/phong.vs", "<fs>shaders/phong.fs", NULL);

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
  assert(s);
  _gridId = gl4dgGenGrid2df(s->w, s->h);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, s->w, s->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, s->pixels);

  glBindTexture(GL_TEXTURE_2D, _tex[1]);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, s->w, s->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

  SDL_FreeSurface(s);







  glBindTexture(GL_TEXTURE_2D, 0);

  gl4duGenMatrix(GL_FLOAT, "modView");
  gl4duGenMatrix(GL_FLOAT, "proj"); 
  gl4duBindMatrix("proj");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.1f, 0.1f, (-0.1f * _wH) / _wW, (0.1f * _wH) / _wW, 0.1f, 1000.0f);
}

void draw(void) {
  static int i = 0;
  GLfloat pas[] = { 1.0f / (100 - 1.0f), 1.0f / (75 - 1.0f)};

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glViewport(0, 0, 100, 75);

  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _tex[(i + 1) % 2],  0);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(_pId_conv);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _tex[i % 2]);
  glUniform1i(glGetUniformLocation(_pId_conv, "tex"), 0);
  glUniform2fv(glGetUniformLocation(_pId_conv, "pas"), 1, pas);
  gl4dgDraw(_quadId);
  
  glViewport(0, 0, _wW, _wH);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);


  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(_pId_hm);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _tex[i % 2]);
  glUniform1i(glGetUniformLocation(_pId_hm, "tex"), 0);
  //glUniform2fv(glGetUniformLocation(_pId_hm, "pas"), 1, pas);
  gl4duBindMatrix("modView");
  gl4duLoadIdentityf();
  gl4duLookAtf(-1.0f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
  gl4duSendMatrices();
  gl4dgDraw(_gridId);


  ++i;
  //SDL_Delay(500);
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
