#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4dg.h>
#include <GL4D/gl4dh.h>
#include <stdio.h>
#include <SDL_image.h>

static void init(void);
/* TODO : gérer le retaillage de la fenêtre */
/* static void resize(int width, int height); */
static void draw(void);
static void sortie(void);

static GLuint _wW = 1024, _wH = 768;
static GLuint _coneId = 0;
static GLuint _quadId = 0;
static GLuint _pId = 0;
static GLuint _texId[3] = { 0 };

void nmap(int state) {
  /* INITIALISEZ VOS VARIABLES */
  /* ... */
  switch(state) {
  case GL4DH_INIT:
    /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    init();
    return;
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    sortie();
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
    return;
  default: /* GL4DH_DRAW */
    /* JOUER L'ANIMATION */
    draw();
    return;
  }
}



void init(void) {
  SDL_Surface * s = NULL;
  _coneId = gl4dgGenConef(3, GL_TRUE);
  _quadId = gl4dgGenQuadf();
  _pId = gl4duCreateProgram("<vs>shaders/nmap.vs", "<fs>shaders/nmap.fs", NULL);

  gl4duGenMatrix(GL_FLOAT, "model");
  gl4duGenMatrix(GL_FLOAT, "view");

  gl4duGenMatrix(GL_FLOAT, "proj");
  gl4duBindMatrix("proj");
  gl4duLoadIdentityf();
  gl4duFrustumf(-1.0f, 1.0f, (-1.0f * _wH)  / _wW, (1.0f * _wH)  / _wW, 1.0f, 1000.0f);
  /* glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); */
  /* glLineWidth(7.0f); */
  /* glEnable(GL_LINE_SMOOTH); */

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

  glBindTexture(GL_TEXTURE_2D, 0);
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
  GLfloat lumpos[] = {-4.0f, 4.0f, 0.0f, 1.0f};
  GLboolean dt = glIsEnabled(GL_DEPTH_TEST);
  lumpos[1] = 2.0f + 1.9f * sin(a);

  glEnable(GL_DEPTH_TEST);
  
  glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(_pId);
  glUniform4fv(glGetUniformLocation(_pId, "lumpos") , 1, lumpos);

  gl4duBindMatrix("view");
  gl4duLoadIdentityf();
  gl4duLookAtf(0.0f, 2.0f, 6.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

  gl4duBindMatrix("model");
  gl4duLoadIdentityf();
  gl4duTranslatef(0.0f, 1.5f, 0.0f);
  gl4duRotatef(180.0f * a / M_PI, 0.0f, 1.0f, 0.0f);
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_pId, "couleur") , 1, bleu);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _texId[0]);
  glUniform1i(glGetUniformLocation(_pId, "use_tex"), 1);
  glUniform1i(glGetUniformLocation(_pId, "use_nm"), 0);
  gl4dgDraw(_coneId);

  gl4duLoadIdentityf();
  gl4duRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
  gl4duScalef(5.0f, 5.0f, 5.0f);
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_pId, "couleur") , 1, rouge);

  glActiveTexture(GL_TEXTURE0 + 1);
  glBindTexture(GL_TEXTURE_2D, _texId[2]);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _texId[1]);
  glUniform1i(glGetUniformLocation(_pId, "tex"), 0);
  glUniform1i(glGetUniformLocation(_pId, "nm"), 1);
  glUniform1i(glGetUniformLocation(_pId, "use_tex"), 1);
  glUniform1i(glGetUniformLocation(_pId, "use_nm"), 1);

  gl4dgDraw(_quadId);

  glBindTexture(GL_TEXTURE_2D, 0);

  glUseProgram(0);
  a += 0.4f * M_PI * get_dt();

  if(!dt)
    glDisable(GL_DEPTH_TEST);
}

void sortie(void) {
  if(_texId[0]) {
    glDeleteTextures(sizeof _texId / sizeof *_texId, _texId);
    _texId[0] = 0;
  }
}
