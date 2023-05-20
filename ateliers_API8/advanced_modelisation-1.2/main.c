#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4dg.h>
#include <GL4D/gl4df.h>
#include <stdio.h>
#include <SDL_image.h>
#include "noise.h"

#define SHADOW_MAP_SIDE 1024


static void init(void);
/* TODO : gérer le retaillage de la fenêtre */
/* static void resize(int width, int height); */
static void draw(void);
static void sortie(void);

static GLuint _wW = 1024, _wH = 768;
static GLuint _coneId = 0;
static GLuint _quadId = 0;
static GLuint _sphereId = 0;
static GLuint _pId[3] = { 0 };
static GLuint _texId[3] = { 0 };

/*!\brief Framebuffer Object */
static GLuint _fbo = 0;
/*!\brief Texture recevant la shadow map */
static GLuint _smTex = 0;

int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "Ateliers API8 - modélisation", GL4DW_POS_CENTERED, GL4DW_POS_CENTERED,
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
  SDL_Surface * s = NULL;
  initNoiseTextures();
  glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
  SDL_GL_SetSwapInterval(1);
  glEnable(GL_DEPTH_TEST);
  /* Attention on change la valeur par défaut et on active la CULL FACE */
  //PB2 bien activer le cull face mais l'inverser pour SM
  glEnable(GL_CULL_FACE);
  _coneId = gl4dgGenConef(3, GL_TRUE);
  _quadId = gl4dgGenQuadf();
  _sphereId = gl4dgGenSpheref(10, 10);
  _pId[0] = gl4duCreateProgram("<vs>shaders/hello.vs", "<fs>shaders/hello.fs", NULL);
  _pId[1] = gl4duCreateProgram("<vs>shaders/hello.vs", "<fs>shaders/perlin.fs", NULL);
  _pId[2] = gl4duCreateProgram("<vs>shaders/shadowMap.vs", "<fs>shaders/shadowMap.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "model");
  gl4duGenMatrix(GL_FLOAT, "view");
  gl4duGenMatrix(GL_FLOAT, "lightViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "lightProjectionMatrix");

  gl4duBindMatrix("lightProjectionMatrix");
  gl4duLoadIdentityf();
  gl4duOrthof(-13, 13, -6, 6, 1, 40);

  gl4duGenMatrix(GL_FLOAT, "proj");
  gl4duBindMatrix("proj");
  gl4duLoadIdentityf();
  gl4duFrustumf(-1.0f, 1.0f, (-1.0f * _wH)  / _wW, (1.0f * _wH)  / _wW, 1.0f, 1000.0f);

  glGenTextures(sizeof _texId / sizeof *_texId, _texId);
  assert(_texId[0] && _texId[1]);

  glBindTexture(GL_TEXTURE_2D, _texId[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  s = IMG_Load("images/pyramide.png");
  if(s != NULL) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, s->pixels);
    SDL_FreeSurface(s);
  } else { /* si échec de chargement */
    GLuint p = {(0xFF << 24) | 0xFF};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &p);
  }

  glBindTexture(GL_TEXTURE_2D, _texId[1]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  s = IMG_Load("images/sol.jpg");
  if(s != NULL) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h, 0, GL_RGB, GL_UNSIGNED_BYTE, s->pixels);
    SDL_FreeSurface(s);
  } else { /* si échec de chargement */
    GLuint p = {(0xFF << 24) | 0xFF};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &p);
  }

  glBindTexture(GL_TEXTURE_2D, _texId[2]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  s = IMG_Load("images/sol_nm.jpg");
  if(s != NULL) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h, 0, GL_RGB, GL_UNSIGNED_BYTE, s->pixels);
    SDL_FreeSurface(s);
  } else { /* si échec de chargement */
    GLuint p = {(0xFF << 24) | 0xFF};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &p);
  }

 /* Création et paramétrage de la Texture de shadow map */
  glGenTextures(1, &_smTex);
  glBindTexture(GL_TEXTURE_2D, _smTex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_MAP_SIDE, SHADOW_MAP_SIDE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

  glBindTexture(GL_TEXTURE_2D, 0);

  /* Création du Framebuffer Object */
  glGenFramebuffers(1, &_fbo);

}

static double get_dt(void) {
  static double t0 = 0.0f;
  double t = gl4dGetElapsedTime(), dt = (t - t0) / 1000.0;
  t0 = t;
  return dt;
}

void draw(void) {
  static const GLfloat rouge[] = {0.6f, 0.0f, 0.0f, 1.0f}, bleu[] = {0.0f, 0.0f, 0.6f, 1.0f};
  static GLfloat a = 0.0f;
  GLfloat lumpos[] = {9.0f, 4.0f, 0.0f, 1.0f};
  lumpos[1] = 4.0f + 1.9f * sin(a);
  gl4duBindMatrix("lightViewMatrix");
  gl4duLoadIdentityf();
  gl4duLookAtf(lumpos[0], lumpos[1], lumpos[2], 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
  /* Attention le CULL FACE est sur les faces arrières */
  //PB6
  glCullFace(GL_FRONT);


  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _smTex, 0);
  //PB4 viewport adapté à la texture
  glViewport(0, 0, SHADOW_MAP_SIDE, SHADOW_MAP_SIDE);
  glClear(GL_DEPTH_BUFFER_BIT);
  glUseProgram(_pId[2]);
  /* cone */
  gl4duBindMatrix("model");
  gl4duLoadIdentityf();
  gl4duTranslatef(0.0f, 1.5f, 0.0f);
  gl4duRotatef(180.0f * a / M_PI, 0.0f, 1.0f, 0.0f);
  gl4duSendMatrices();
  gl4dgDraw(_coneId);
  /* quad */
  gl4duLoadIdentityf();
  gl4duRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
  gl4duScalef(13.0f, 13.0f, 13.0f);
  gl4duSendMatrices();
  gl4dgDraw(_quadId);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  //PB7
  glCullFace(GL_BACK);
  glDrawBuffer(GL_COLOR_ATTACHMENT0);

  //PB5 viewport adapté à la texture puis la fenêtre
  glViewport(0, 0, _wW, _wH);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(_pId[0]);
  glUniform4fv(glGetUniformLocation(_pId[0], "lumpos") , 1, lumpos);

  gl4duBindMatrix("view");
  gl4duLoadIdentityf();
  gl4duLookAtf(0.0f, 2.0f, 6.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

  /* cone */
  gl4duBindMatrix("model");
  gl4duLoadIdentityf();
  gl4duTranslatef(0.0f, 1.5f, 0.0f);
  gl4duRotatef(180.0f * a / M_PI, 0.0f, 1.0f, 0.0f);
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_pId[0], "couleur") , 1, bleu);
  glActiveTexture(GL_TEXTURE0 + 1);
  glBindTexture(GL_TEXTURE_2D, _smTex);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _texId[0]);
  glUniform1i(glGetUniformLocation(_pId[0], "tex"), 0);
  glUniform1i(glGetUniformLocation(_pId[0], "tex_inv"), 1);
  glUniform1f(glGetUniformLocation(_pId[0], "tex_zoom"), 2.0f);
  glUniform1i(glGetUniformLocation(_pId[0], "sm"), 1);
  glUniform1i(glGetUniformLocation(_pId[0], "use_tex"), 1);
  glUniform1i(glGetUniformLocation(_pId[0], "use_nm"), 0);
  gl4dgDraw(_coneId);

  /* quad */
  gl4duLoadIdentityf();
  gl4duRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
  gl4duScalef(13.0f, 13.0f, 13.0f);
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_pId[0], "couleur") , 1, rouge);

  //PB de rapidité
  //Pour le même shader, avoir placé sm en 1 puis en 2
  //ça l'a ralenti drastiquement !!!
  glActiveTexture(GL_TEXTURE0 + 2);
  glBindTexture(GL_TEXTURE_2D, _texId[2]);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _texId[1]);
  glUniform1i(glGetUniformLocation(_pId[0], "nm"), 2);
  glUniform1i(glGetUniformLocation(_pId[0], "use_nm"), 1);
  glUniform1f(glGetUniformLocation(_pId[0], "tex_zoom"), 10.0f);
  gl4dgDraw(_quadId);

  glBindTexture(GL_TEXTURE_2D, 0);


  /* Attention le CULL FACE est sur les faces avant */
  glCullFace(GL_FRONT);
  /* sphere */
  glUseProgram(_pId[1]);
  gl4duLoadIdentityf();
  gl4duScalef(25.0f, 25.0f, 25.0f);
  gl4duSendMatrices();
  glUniform1i(glGetUniformLocation(_pId[1], "tex_inv"), 0);
  glUniform1f(glGetUniformLocation(_pId[1], "tex_zoom"), 1.6f);
  useNoiseTextures(_pId[1], 0);
  gl4dgDraw(_sphereId);
  unuseNoiseTextures(0);

  glUseProgram(0);
  
  a += 0.4f * M_PI * get_dt();
}

void sortie(void) {
  if(_fbo) {
    glDeleteTextures(1, &_smTex);
    glDeleteFramebuffers(1, &_fbo);
    _fbo = 0;
  }
  if(_texId[0]) {
    glDeleteTextures(sizeof _texId / sizeof *_texId, _texId);
    _texId[0] = 0;
  }
  freeNoiseTextures();
  gl4duClean(GL4DU_ALL);
}
