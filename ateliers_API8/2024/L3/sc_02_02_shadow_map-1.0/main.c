#include <GL4D/gl4duw_SDL2.h>
#include <stdio.h>

static void init(void);
static void resize(int width, int height);
static void draw(void);
static void sortie(void);

static const GLuint SHADOW_MAP_SIDE = 1024;
static GLuint _wW = 1200, _wH = 960;
static GLuint _quadId = 0;
static GLuint _torusId = 0, _terrainId = 0;
static GLuint _smTex = 0;
static GLuint _pId = 0, _smpId = 0, _fbo = 0;

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
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  _quadId = gl4dgGenQuadf();
  _torusId = gl4dgGenTorusf(20, 10, 0.3);
  GLfloat * hm = gl4dmTriangleEdge(513, 513, 0.4f);
  _terrainId = gl4dgGenConef(20, GL_TRUE); //gl4dgGenGrid2dFromHeightMapf(513, 513, hm);
  free(hm);
  _pId = gl4duCreateProgram("<vs>shaders/lights.vs", "<fs>shaders/lights.fs", NULL);
  _smpId  = gl4duCreateProgram("<vs>shaders/shadowMap.vs", "<fs>shaders/shadowMap.fs", NULL);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  gl4duGenMatrix(GL_FLOAT, "proj");
  gl4duGenMatrix(GL_FLOAT, "mod");
  gl4duGenMatrix(GL_FLOAT, "view");
  
  gl4duGenMatrix(GL_FLOAT, "lightView");
  gl4duGenMatrix(GL_FLOAT, "lightProj");
  
  /* Création et paramétrage de la Texture de shadow map */
  glGenTextures(1, &_smTex);
  glBindTexture(GL_TEXTURE_2D, _smTex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_MAP_SIDE, SHADOW_MAP_SIDE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  /* Création du Framebuffer Object */
  glGenFramebuffers(1, &_fbo);
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
  gl4duFrustumf(-ratio, ratio, -1.0f, 1.0f, 2.0f, 100.0f);
  gl4duBindMatrix("lightProj");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.1f, 0.1f, -0.1f, 0.1f, 0.1f, 50.0f);
}

void scene(GLboolean sm, GLfloat dt) {
  static GLfloat x = 0.0f;
  GLfloat rouge[] = {1.0f, 0.5, 0.5, 1.0f}, jaune[] = {1.0f, 1.0f, 0, 1.0f}, blanc[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  GLfloat lumpos0[] = { 0.0f, 3.5f, 1.5f, 1.0f }, lumpos[4], * mat;
  lumpos0[0] = 4.0f * sin(x);

  gl4duBindMatrix("view");

  /* gl4duLoadIdentityf(); */
  /* gl4duLookAtf(lumpos0[0], lumpos0[1], lumpos0[2], 0, 0, -5.0f, 0, 1, 0); */
  
  mat = (GLfloat *)gl4duGetMatrixData();
  MMAT4XVEC4(lumpos, mat, lumpos0);
  
  if(sm) {
    glCullFace(GL_FRONT);
    glUseProgram(_smpId);
    gl4duBindMatrix("lightView");
    gl4duLoadIdentityf();
    gl4duLookAtf(lumpos0[0], lumpos0[1], lumpos0[2], 0, 0, -5.0f, 0, 1, 0);
  } else {
    glCullFace(GL_BACK);
    glUseProgram(_pId);
    x += dt;
  }
  glEnable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _smTex);
  glUniform1i(glGetUniformLocation(_pId, "smTex"), 0);


  gl4duBindMatrix("mod");

  gl4duLoadIdentityf();
  gl4duTranslatef(0.0f, 0.0f, -4.0f);
  gl4duScalef(7.0f, 1.0f, 7.0f);
  gl4duRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_pId, "scolor"), 1, rouge);
  glUniform4fv(glGetUniformLocation(_pId, "lcolor"), 1, blanc);
  glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lumpos);
  gl4dgDraw(_quadId);

  glUniform4fv(glGetUniformLocation(_pId, "scolor"), 1, jaune);

  gl4duLoadIdentityf();
  gl4duTranslatef(-2.5f, 1.0f, -2.5f);
  gl4duScalef(0.25f, 1.0f, 0.25f);
  gl4duRotatef(90.0f, 1.0f, 0.0f, 0.0f);
  gl4duSendMatrices();
  gl4dgDraw(_torusId);

  gl4duLoadIdentityf();
  gl4duTranslatef(-1.0f, 0.5f, -4.5f);
  gl4duScalef(3.0f, 3.0f, 3.0f);
  gl4duRotatef(10.0f, 1.0f, 0.0f, 0.0f);
  gl4duSendMatrices();
  gl4dgDraw(_terrainId);
  
  glUseProgram(0);
}

void draw(void) {
  GLenum rendering = GL_COLOR_ATTACHMENT0;
  static double t0 = 0.0;
  double t = gl4dGetElapsedTime() / 1000.0, dt = t - t0;
  t0 = t;

  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
  /* désactiver le rendu de couleur et ne laisser que le depth, dans _smTex */
  glDrawBuffer(GL_NONE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,    GL_TEXTURE_2D, 0, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _smTex, 0);
  /* viewport de la shadow map et dessin de la scène du point de vue de la lumière */
  glViewport(0, 0, SHADOW_MAP_SIDE, SHADOW_MAP_SIDE);
  glClear(GL_DEPTH_BUFFER_BIT);
  scene(GL_TRUE, dt);

  /* remettre GL_COLOR_ATTACHMENT0, viewport et fbo écran */
  glDrawBuffers(1, &rendering);
  glViewport(0, 0, _wW, _wH);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  /* vue 1 */
  gl4duBindMatrix("view");
  gl4duLoadIdentityf();
  gl4duLookAtf(0, 1.8f, 6.0f, 0, 0, 0, 0.0f, 1.0f, 0);
  scene(GL_FALSE, dt);
}

void sortie(void) {
  if(_fbo) {
    glDeleteTextures(1, &_smTex);
    glDeleteFramebuffers(1, &_fbo);
    _fbo = 0;
  }
  gl4duClean(GL4DU_ALL);
}
