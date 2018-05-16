/*!\file window.c
 *
 * \brief utilisation de GL4Dummies et Lib Assimp pour chargement de
 * scènes.
 *
 * \author Farès Belhadj amsi@ai.univ-paris8.fr
 * \date February 14 2017
 */

#include <GL4D/gl4duw_SDL2.h>
#include <SDL_image.h>

/*!\brief opened window width */
static int _windowWidth = 600;
/*!\brief opened window height */
static int _windowHeight = 800;
/*!\brief GLSL program Id */
static GLuint _pId = 0;

/*!\brief enum that index keyboard mapping for direction commands */
enum kyes_t {
  KLEFT = 0,
  KRIGHT,
  KUP,
  KDOWN,
  KPAGEUP,
  KPAGEDOWN
};

/*!\brief virtual keyboard for direction commands */
static GLuint _keys[] = {0, 0, 0, 0, 0, 0};

typedef struct cam_t cam_t;
/*!\brief a data structure for storing camera position and
 * orientation */
struct cam_t {
  GLfloat x, y, z;
  GLfloat theta;
};

/*!\brief the used camera */
static cam_t _cam = {0, 0, 1.0, 0};

/*!\brief toggle y-axis rotation pause */
static GLboolean _pause = GL_TRUE;

/*!\brief rotation angles according to axis (0 = x, 1 = y, 2 = z) 
 * \todo améliorer l'interface et ajouter rotations/zoom à la souris */
static GLfloat rot[3] = {0, 0, 0};

extern void assimpInit(const char * filename);
extern void assimpDrawScene(void);
extern void assimpQuit(void);

static void initGL(void);
static void quit(void);
static void resize(int w, int h);
static void idle(void);
static void draw(void);
static void keydown(int keycode);
static void keyup(int keycode);

int main(int argc, char ** argv) {
  if(argc != 2) {
    fprintf(stderr, "usage: %s <3d_file>\n", argv[0]);
    return 1;
  }
  if(!gl4duwCreateWindow(argc, argv, "GL4Dummies", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                         _windowWidth, _windowHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN))
    return 1;
  assimpInit(argv[1]);
  initGL();
  atexit(quit);
  gl4duwResizeFunc(resize);
  gl4duwKeyUpFunc(keyup);
  gl4duwKeyDownFunc(keydown);
  gl4duwDisplayFunc(draw);
  gl4duwIdleFunc(idle);
  gl4duwMainLoop();
  return 0;
}

static void initGL(void) {
  glEnable(GL_DEPTH_TEST);
  _pId = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  resize(_windowWidth, _windowHeight);
}

/*!\brief function called by GL4Dummies' loop at resize. Sets the
 *  projection matrix and the viewport according to the given width
 *  and height.
 * \param w window width
 * \param h window height
 */
static void resize(int w, int h) {
  _windowWidth = w; 
  _windowHeight = h;
  glViewport(0, 0, _windowWidth, _windowHeight);
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.005, 0.005, -0.005 * _windowHeight / _windowWidth, 0.005 * _windowHeight / _windowWidth, 0.01, 1000.0);
  //gl4duPerspectivef(60.0, (GLfloat)_windowWidth/(GLfloat)_windowHeight, 0.01, 1000.0);
  gl4duBindMatrix("modelViewMatrix");
}

/*!\brief function called by GL4Dummies' loop at idle.
 * 
 * uses the virtual keyboard states to move the camera according to
 * direction, orientation and time (dt = delta-time)
 */
static void idle(void) {
  double dt, dtheta = M_PI, step = 5.0;
  static Uint32 t0 = 0, t;
  dt = ((t = SDL_GetTicks()) - t0) / 1000.0;
  t0 = t;
  if(_keys[KLEFT])
    _cam.theta += dt * dtheta;
  if(_keys[KRIGHT])
    _cam.theta -= dt * dtheta;
  if(_keys[KUP]) {
    _cam.y += dt * 0.1f * step;
  }
  if(_keys[KDOWN]) {
    _cam.y -= dt * 0.1f * step;
  }
  if(_keys[KPAGEUP]) {
    _cam.x += -dt * step * sin(_cam.theta);
    _cam.z += -dt * step * cos(_cam.theta);
  }
  if(_keys[KPAGEDOWN]) {
    _cam.x += dt * step * sin(_cam.theta);
    _cam.z += dt * step * cos(_cam.theta);
  }
  if(!_pause)
    rot[1] += 90.0 * dt;
}

/*!\brief function called by GL4Dummies' loop at key-down (key
 * pressed) event.
 * 
 * stores the virtual keyboard states (1 = pressed) and toggles the
 * boolean parameters of the application.
 */
static void keydown(int keycode) {
  GLint v[2];
  switch(keycode) {
  case SDLK_LEFT:
    _keys[KLEFT] = 1;
    break;
  case SDLK_RIGHT:
    _keys[KRIGHT] = 1;
    break;
  case SDLK_UP:
    _keys[KUP] = 1;
    break;
  case SDLK_DOWN:
    _keys[KDOWN] = 1;
    break;
  case SDLK_PAGEDOWN:
    _keys[KPAGEDOWN] = 1;
    break;
  case SDLK_PAGEUP:
    _keys[KPAGEUP] = 1;
    break;
  case SDLK_ESCAPE:
  case 'q':
    exit(0);
    /* when 'w' pressed, toggle between line and filled mode */
  case 'w':
    glGetIntegerv(GL_POLYGON_MODE, v);
    if(v[0] == GL_FILL) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      glLineWidth(3.0);
    } else {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      glLineWidth(1.0);
    }
    break;
  case ' ':
    _pause = !_pause;
    break;
  default:
    break;
  }
}

/*!\brief function called by GL4Dummies' loop at key-up (key
 * released) event.
 * 
 * stores the virtual keyboard states (0 = released).
 */
static void keyup(int keycode) {
  switch(keycode) {
  case SDLK_LEFT:
    _keys[KLEFT] = 0;
    break;
  case SDLK_RIGHT:
    _keys[KRIGHT] = 0;
    break;
  case SDLK_UP:
    _keys[KUP] = 0;
    break;
  case SDLK_DOWN:
    _keys[KDOWN] = 0;
    break;
  case SDLK_PAGEDOWN:
    _keys[KPAGEDOWN] = 0;
    break;
  case SDLK_PAGEUP:
    _keys[KPAGEUP] = 0;
    break;
  default:
    break;
  }
}

static void draw(void) {
  GLfloat lum[4] = {0.0, 0.0, 5.0, 1.0};
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(_pId);

  glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lum);

  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();

  gl4duLookAtf(_cam.x, _cam.y, _cam.z,
               _cam.x - sin(_cam.theta), _cam.y,  _cam.z - cos(_cam.theta),
               0.0, 1.0, 0.0);
  gl4duRotatef(rot[1], 0, 1, 0);
  assimpDrawScene();
}

static void quit(void) {
  assimpQuit();
  gl4duClean(GL4DU_ALL);
}

