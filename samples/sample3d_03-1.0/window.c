/*!\file window.c
 *
 * \brief Walking on finite plane with skydome textured with a
 * triangle-edge midpoint-displacement algorithm.
 *
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date February 9 2017
 */
#include <math.h>
#include <GL4D/gl4dg.h>
#include <GL4D/gl4duw_SDL2.h>

static void quit(void);
static void initGL(void);
static void resize(int w, int h);
static void idle(void);
static void keydown(int keycode);
static void keyup(int keycode);
static void draw(void);

/*!\brief opened window width */
static int _windowWidth = 800;
/*!\brief opened window height */
static int _windowHeight = 600;
/*!\brief Grid geometry Id  */
static GLuint _grid = 0;
/*!\brief grid width */
static int _gridWidth = 129;
/*!\brief grid height */
static int _gridHeight = 129;
/*!\brief GLSL program Id */
static GLuint _pId = 0;
/*!\brief angle, in degrees, for x-axis rotation */
static GLfloat _rx = 90.0f;
/*!\brief angle, in degrees, for y-axis rotation */
static GLfloat _ry = 0.0f;

/*!\brief enum that index keyboard mapping for direction commands */
enum kyes_t {
  KLEFT = 0, KRIGHT, KUP, KDOWN, KSHIFT
};

/*!\brief virtual keyboard for direction commands */
static GLboolean _keys[] = {GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE};

/*!\brief creates the window, initializes OpenGL parameters,
 * initializes data and maps callback functions */
int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "GL4Dummies", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                         _windowWidth, _windowHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN))
    return 1;
  initGL();
  /* generates a grid using GL4Dummies */
  _grid = gl4dgGenGrid2df(_gridWidth, _gridHeight);
  atexit(quit);
  gl4duwResizeFunc(resize);
  gl4duwKeyUpFunc(keyup);
  gl4duwKeyDownFunc(keydown);
  gl4duwDisplayFunc(draw);
  gl4duwIdleFunc(idle);
  gl4duwMainLoop();
  return 0;
}

/*!\brief initializes OpenGL parameters :
 *
 * the clear color, enables face culling, enables blending and sets
 * its blending function, enables the depth test and 2D textures,
 * creates the program shader, the model-view matrix and the
 * projection matrix and finally calls resize that sets the projection
 * matrix and the viewport.
 */
static void initGL(void) {
  glClearColor(0.0f, 0.4f, 0.9f, 0.0f);
  glEnable(GL_DEPTH_TEST);
  _pId  = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
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
  gl4duFrustumf(-0.5, 0.5, -0.5 * _windowHeight / _windowWidth, 0.5 * _windowHeight / _windowWidth, 1.0, 10.0);
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
  if(_keys[KLEFT])
    _ry += 180 * dt;
  if(_keys[KRIGHT])
    _ry -= 180 * dt;
  if(_keys[KUP])
    _rx -= 180 * dt;
  if(_keys[KDOWN])
    _rx += 180 * dt;
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
    _keys[KLEFT] = GL_TRUE;
    break;
  case SDLK_RIGHT:
    _keys[KRIGHT] = GL_TRUE;
    break;
  case SDLK_UP:
    _keys[KUP] = GL_TRUE;
    break;
  case SDLK_DOWN:
    _keys[KDOWN] = GL_TRUE;
    break;
  case SDLK_ESCAPE:
  case 'q':
    exit(0);
    /* when 'w' pressed, toggle between line and filled mode */
  case 'w':
    glGetIntegerv(GL_POLYGON_MODE, v);
    if(v[0] == GL_FILL) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      glLineWidth(1.0);
    } else {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      glLineWidth(1.0);
    }
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
    _keys[KLEFT] = GL_FALSE;
    break;
  case SDLK_RIGHT:
    _keys[KRIGHT] = GL_FALSE;
    break;
  case SDLK_UP:
    _keys[KUP] = GL_FALSE;
    break;
  case SDLK_DOWN:
    _keys[KDOWN] = GL_FALSE;
    break;
  default:
    break;
  }
}

/*!\brief function called by GL4Dummies' loop at draw.*/
static void draw(void) {
  /* step = x and y max distance between two adjacent vertices */
  GLfloat step[2] = { 2.0 / _gridWidth, 2.0 / _gridHeight};
  /* clears the OpenGL color buffer and depth buffer */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  /* sets the current program shader to _pId */
  glUseProgram(_pId);
  /* loads the identity matrix in the current GL4Dummies matrix ("modelViewMatrix") */
  gl4duLoadIdentityf();

  /* rotation around x-axis, old y-axis and tranlation */
  gl4duPushMatrix(); {
    gl4duTranslatef(0, 0, -4);
    gl4duRotatef(_ry, 0, cos(_rx * M_PI / 180.0), sin(_rx * M_PI / 180.0));
    gl4duRotatef(_rx, 1, 0, 0);
    gl4duSendMatrices();
  } gl4duPopMatrix();
  /* draws the grid */
  glUniform1ui(glGetUniformLocation(_pId, "frame"), SDL_GetTicks());
  glUniform2fv(glGetUniformLocation(_pId, "step"), 1, step);
  gl4dgDraw(_grid);
}

/*!\brief function called at exit, it cleans-up GL4Dummies.*/
static void quit(void) {
  gl4duClean(GL4DU_ALL);
}
