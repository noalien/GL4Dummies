#include <GL4D/gl4duw_SDL2.h>
#include <stdio.h>

static void init(void);
static void resize(int width, int height);
static void draw(void);
static void sortie(void);

static GLuint _wW = 640, _wH = 480;
static GLuint _quadId = 0;
static GLuint _pId = 0;

int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "Eclairage", GL4DW_POS_CENTERED, GL4DW_POS_CENTERED,
			 _wW, _wH, GL4DW_OPENGL | GL4DW_RESIZABLE | GL4DW_SHOWN)) {
    fprintf(stderr, "Erreur lors de la création de la fenêtre\n");
    return 1;
  }
  init();
  atexit(sortie);
  gl4duwResizeFunc(resize);
  gl4duwDisplayFunc(draw);
  gl4duwMainLoop();
  return 0;
}

void init(void) {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  _quadId = gl4dgGenQuadf();
  _pId = gl4duCreateProgram("<vs>shaders/lights.vs", "<fs>shaders/lights.fs", NULL);
  glEnable(GL_DEPTH_TEST);
  gl4duGenMatrix(GL_FLOAT, "proj");
  gl4duGenMatrix(GL_FLOAT, "mod");
  gl4duGenMatrix(GL_FLOAT, "view");
  resize(_wW, _wH);
}

static void resize(int width, int height) {
  GLfloat ratio;
  _wW = width;
  _wH = height;
  glViewport(0, 0, _wW, _wH);
  ratio = _wW / ((GLfloat)_wH);
  gl4duBindMatrix("proj");
  gl4duLoadIdentityf();
  gl4duFrustumf(-1 * ratio, 1 * ratio, -1 /* / ratio */, 1 /* / ratio */, 2, 100);
}

void scene(void) {
  GLfloat rouge[] = {1.0f, 0, 0, 1.0f}, blanc[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  gl4duBindMatrix("mod");
  gl4duLoadIdentityf();
  glUseProgram(_pId);
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_pId, "scolor"), 1, rouge);
  glUniform4fv(glGetUniformLocation(_pId, "lcolor"), 1, blanc);
  gl4dgDraw(_quadId);
  glUseProgram(0);
}

void draw(void) {
  static double t0 = 0.0;
  double t = gl4dGetElapsedTime() / 1000.0, dt = t - t0;
  t0 = t;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  /* vue 1 */
  gl4duBindMatrix("view");
  gl4duLoadIdentityf();
  gl4duLookAtf(0, 0, 4.0f, 0, 0, 0, 0.0f, 1.0f, 0);
  scene();
}

void sortie(void) {
  gl4duClean(GL4DU_ALL);
}
