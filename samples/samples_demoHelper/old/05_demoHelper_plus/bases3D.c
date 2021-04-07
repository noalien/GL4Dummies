#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4dp.h>
#include <stdio.h>

void bases3D_init(void);
void bases3D_draw(void);
void bases3D_sortie(void);

static GLuint _wW = 1280, _wH = 960;
static GLuint _sphereId = 0;
static GLuint _torusId = 0;
static GLuint _pId = 0;
static GLuint _texId = 0;

void bases3D_init(void) {
  GLuint pixels[] = {
    RGB(255, 100, 100), RGB(100, 255, 100), RGB(100, 100, 255), RGB(255, 255, 100),
    RGB(100, 100, 255), RGB(255, 255, 100), RGB(255, 255, 255), RGB(255, 100, 100), 
    RGB(255, 255, 100), RGB(255, 100, 100), RGB(100, 255, 100), RGB(100, 100, 255)
  };
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  _sphereId = gl4dgGenSpheref(10, 10);
  _torusId = gl4dgGenTorusf(10, 10, 0.3f);
  _pId = gl4duCreateProgram("<vs>shaders/bases3D.vs", "<fs>shaders/bases3D.fs", NULL);
  glEnable(GL_DEPTH_TEST);
  /* glEnable(GL_CULL_FACE); */
  /* glCullFace(GL_FRONT); */
  /* glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); */
  gl4duGenMatrix(GL_FLOAT, "proj");
  gl4duGenMatrix(GL_FLOAT, "mod");
  gl4duGenMatrix(GL_FLOAT, "view");
  glGenTextures(1, &_texId);
  glBindTexture(GL_TEXTURE_2D, _texId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 4, 3, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
  glBindTexture(GL_TEXTURE_2D, 0);
}

static void resize(void) {
  GLfloat ratio;
  GLint data[4];
  glGetIntegerv(GL_VIEWPORT, data);
  _wW = data[2];
  _wH = data[3];
  ratio = _wW / ((GLfloat)_wH);
  gl4duBindMatrix("proj");
  gl4duLoadIdentityf();
  /* gl4duOrthof(-1 * ratio, 1 * ratio, -1 /\* / ratio *\/, 1 /\* / ratio *\/, -1, 1); */
  gl4duFrustumf(-1 * ratio, 1 * ratio, -1 /* / ratio */, 1 /* / ratio */, 2, 100);
}

static void scene(GLfloat a) {
  GLfloat rouge[] = {1.0f, 0.7f, 0.7f, 1.0f}, bleu[] = {0.7f, 0.7f, 1.0f, 1.0f}, blanc[] = {1.0f, 1.0f, 1.0f, 1.0f};
  gl4duBindMatrix("mod");
  gl4duLoadIdentityf();
  gl4duRotatef(a, 0, 1, 0);
  glUseProgram(_pId);
  gl4duSendMatrices();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _texId);
  glUniform1i(glGetUniformLocation(_pId, "tex"), 0);
  glUniform4fv(glGetUniformLocation(_pId, "scolor"), 1, rouge);
  glUniform4fv(glGetUniformLocation(_pId, "lcolor"), 1, blanc);
  gl4dgDraw(_sphereId);
  gl4duTranslatef(3.0f, 0, 0);
  gl4duRotatef(90, 0, 1, 0);
  gl4duRotatef(90, 1, 0, 0);
  gl4duSendMatrices();
  glUniform1i(glGetUniformLocation(_pId, "tex"), 0);
  glUniform4fv(glGetUniformLocation(_pId, "scolor"), 1, bleu);
  glUniform4fv(glGetUniformLocation(_pId, "lcolor"), 1, blanc);
  gl4dgDraw(_torusId);
  glUseProgram(0);
  glBindTexture(GL_TEXTURE_2D, 0);

}

void bases3D_draw(void) {
  static double t0 = 0.0;
  double t = gl4dGetElapsedTime() / 1000.0, dt = t - t0;
  t0 = t;
  static GLfloat a = 0.0f, y = 0.0f;
  resize();
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  /* vue 1 */
  glViewport(0, 0, _wW / 2, _wH / 2);
  gl4duBindMatrix("view");
  gl4duLoadIdentityf();
  gl4duLookAtf(0, y, 8.0f, 0, 0, 0, 0, 1.0f, 0);
  scene(a);
  /* vue 2 */
  glViewport(_wW / 2, 0, _wW / 2, _wH / 2);
  gl4duBindMatrix("view");
  gl4duLoadIdentityf();
  gl4duLookAtf(8.0f, y, 0.0f, 0, 0, 0, 0, 1.0f, 0);
  scene(a);

  a += 180.0f * dt;
  //y += 0.5f * dt;
  glViewport(0, 0, _wW, _wH);
}

void bases3D_sortie(void) {
  if(_texId) {
    glDeleteTextures(1, &_texId);
    _texId = 0;
  }
}
