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
#include <GL4D/gl4df.h>
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
static GLuint _pId = 0, _pId2 = 0;
/* une sphere pour la bounding sphere */
static GLuint _sphere = 0;
/* une texture pour les nuages */
static GLuint _tId = 0;

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

static void loadTexture(GLuint id, const char * filename) {
  SDL_Surface * t;
  glBindTexture(GL_TEXTURE_2D, id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  if( (t = IMG_Load(filename)) != NULL ) {
#ifdef __APPLE__
    int mode = t->format->BytesPerPixel == 4 ? GL_BGRA : GL_BGR;
#else
    int mode = t->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;
#endif       
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t->w, t->h, 0, mode, GL_UNSIGNED_BYTE, t->pixels);
    SDL_FreeSurface(t);
  } else {
    fprintf(stderr, "can't open file %s : %s\n", filename, SDL_GetError());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  }
}

static void init(void) {
  glEnable(GL_DEPTH_TEST);
  _pId = gl4duCreateProgram("<vs>shaders/model.vs", "<fs>shaders/model.fs", NULL);
  _pId2 = gl4duCreateProgram("<vs>shaders/model.vs", "<fs>shaders/clouds.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  _sphere = gl4dgGenSpheref(30, 30);
  glGenTextures(1, &_tId);
  loadTexture(_tId, "images/nuages.jpg");
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
  glUseProgram(_pId2);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _tId);
  glUniform1i(glGetUniformLocation(_pId, "tex"), 0);

  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();
  /* je regarde l'avion depuis 1, 0, -1 */
  gl4duLookAtf(1, 0, -1, _x, _y, _z, 0.0, 1.0, 0.0);
  gl4duPushMatrix(); {
    gl4duScalef(400, 400, 400);
    gl4duSendMatrices();
  } gl4duPopMatrix();
  glDisable(GL_CULL_FACE);
  /* je dessine la bounding sphere avec _pId2 */
  gl4dgDraw(_sphere);

  glUseProgram(_pId);
  glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lum);

  glEnable(GL_CULL_FACE);
  /* je place l'avions */
  gl4duTranslatef(_x, _y, _z);
  /* je le tourne selon sa trajectoire */
  gl4duRotatef(_alpha * 180.0f / M_PI, 0, 1, 0);
  gl4duSendMatrices();
  /* je dessine l'avion */
  assimpDrawScene();
  gl4dfBlur(0, 0, 5, 1, 0, GL_FALSE);
  gl4dfSobelSetMixMode(GL4DF_SOBEL_MIX_MULT);
  gl4dfSobel(0, 0, GL_FALSE);
}

static void quit(void) {
  assimpQuit();
  gl4duClean(GL4DU_ALL);
}

