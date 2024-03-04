#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4dg.h>
#include <stdio.h>

static void init(void);
/* TODO : gérer le retaillage de la fenêtre */
/* static void resize(int width, int height); */
static void draw(void);
static void sortie(void);

static GLuint _wW = 640, _wH = 480;
static GLuint _cubeId = 0;
static GLuint _pId = 0;

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
  glClearColor(0.5f, 0.0f, 0.0f, 1.0f);
  SDL_GL_SetSwapInterval(1);
  _cubeId = gl4dgGenCubef();
  _pId = gl4duCreateProgram("<vs>shaders/hello.vs", "<fs>shaders/hello.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelView");

  gl4duGenMatrix(GL_FLOAT, "proj");
  gl4duBindMatrix("proj");
  gl4duLoadIdentityf();
  gl4duFrustumf(-1.0f, 1.0f, (-1.0f * _wH)  / _wW, (1.0f * _wH)  / _wW, 1.0f, 1000.0f);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glLineWidth(7.0f);
  glEnable(GL_LINE_SMOOTH);
}

static double get_dt(void) {
  static double t0 = 0.0f;
  double t = gl4dGetElapsedTime(), dt = (t - t0) / 1000.0;
  t0 = t;
  return dt;
}

void draw(void) {
  static GLfloat a = 0.0f;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(_pId);
  gl4duBindMatrix("modelView");
  gl4duLoadIdentityf();
  gl4duTranslatef(sin(a), 0.0f, -2.5f);
  gl4duRotatef(180.0f * a / M_PI, 0.0f, 1.0f, 0.0f);
  gl4duSendMatrices();
  gl4dgDraw(_cubeId);
  glUseProgram(0);
  a += 2.0f * M_PI * get_dt();
}

void sortie(void) {
  gl4duClean(GL4DU_ALL);
}
