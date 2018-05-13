/*!\file window.c
 *
 * \brief utilisation de GL4Dummies et Lib Assimp pour chargement de
 * modèles 3D.
 *
 * Ici on charge un modèle d'avion et on le fait tourner sur un cercle
 * en le suivant du regard.
 *
 * \author Farès Belhadj amsi@ai.univ-paris8.fr
 * \date May 13 2018
 */

#include <GL4D/gl4duw_SDL2.h>
#include <SDL_image.h>

/*!\brief opened window width */
static int _wW = 800;
/*!\brief opened window height */
static int _wH = 600;
/*!\brief paramètres du cercle suivi par l'avion */
static GLfloat _radius = 10, _x0 = 10, _z0 = -10, _y0 = 2.5;
/*!\brief paramètres de l'avion */
static GLfloat _x = 0, _y = 0, _z = 0, _alpha = 0;
/*!\brief GLSL program Id */
static GLuint _pId = 0;

extern void assimpInit(const char * filename);
extern void assimpDrawScene(void);
extern void assimpQuit(void);

static void init(void);
static void quit(void);
static void resize(int w, int h);
static void idle(void);
static void draw(void);

int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "GL4Dummies", 0, 0,
                         _wW, _wH, GL4DW_RESIZABLE | GL4DW_SHOWN))
    return 1;
  /* charge l'avion */
  assimpInit("models/piper_pa18/piper_pa18.obj");
  init();
  atexit(quit);
  gl4duwResizeFunc(resize);
  gl4duwDisplayFunc(draw);
  gl4duwIdleFunc(idle);
  gl4duwMainLoop();
  return 0;
}

static void init(void) {
  glEnable(GL_DEPTH_TEST);
  _pId = gl4duCreateProgram("<vs>shaders/model.vs", "<fs>shaders/model.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  resize(_wW, _wH);
}

/*!\brief function called by GL4Dummies' loop at resize. Sets the
 *  projection matrix and the viewport according to the given width
 *  and height.
 * \param w window width
 * \param h window height
 */
static void resize(int w, int h) {
  _wW = w; 
  _wH = h;
  glViewport(0, 0, _wW, _wH);
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.005f, 0.005f, -0.005f * _wH / _wW, 0.005f * _wH / _wW, 0.01f, 1000.0f);
  gl4duBindMatrix("modelViewMatrix");
}

/*!\brief function called by GL4Dummies' loop at idle.
 * 
 * uses the virtual keyboard states to move the camera according to
 * direction, orientation and time (dt = delta-time)
 */
static void idle(void) {
  double dt;
  static Uint32 t0 = 0, t;
  dt = ((t = SDL_GetTicks()) - t0) / 1000.0;
  t0 = t;
  _alpha -= 0.5f * dt;
  _x = _x0 + _radius * cos(_alpha);
  _z = _z0 - _radius * sin(_alpha);
  _y = _y0;
}

static void draw(void) {
  GLfloat lum[4] = {0.0, 0.0, 5.0, 1.0};
  glClearColor(0.0f, 0.7f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(_pId);

  glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lum);

  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();
  /* je regarde l'avion depuis 1, 0, -1 */
  gl4duLookAtf(1, 0, -1, _x, _y, _z, 0.0, 1.0, 0.0);
  /* je place l'avions */
  gl4duTranslatef(_x, _y, _z);
  /* je le tourne selon sa trajectoire */
  gl4duRotatef(_alpha * 180.0f / M_PI, 0, 1, 0);
  /* je dessine l'avion */
  assimpDrawScene();
}

static void quit(void) {
  assimpQuit();
  gl4duClean(GL4DU_ALL);
}

