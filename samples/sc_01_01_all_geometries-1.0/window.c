/*!\file window.c
 *
 * \brief All GL4D Geometries.
 *
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date November 19 2022
 */
#include <time.h>
#include <GL4D/gl4dg.h>
#include <GL4D/gl4df.h>
#include <GL4D/gl4duw_SDL2.h>

static void quit(void);
static void initGL(void);
static void resize(int w, int h);
static void idle(void);
static void keydown(int keycode);
static void keyup(int keycode);
static void draw(void);

/*!\brief geometries Id  */
static GLuint _geom[8] = {}, _gId = 7;
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
                         800, 800, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN))
    return 1;
  initGL();
  _geom[0] = gl4dgGenQuadf();
  _geom[1] = gl4dgGenCubef();
  _geom[2] = gl4dgGenSpheref(20, 20);
  _geom[3] = gl4dgGenConef(20, GL_FALSE);
  _geom[4] = gl4dgGenCylinderf(20, GL_FALSE);
  _geom[5] = gl4dgGenDiskf(20);
  _geom[6] = gl4dgGenTorusf(20, 20, 0.4f);
  /* generates a fractal height-map */
  srand(time(NULL));
  GLfloat * hm = gl4dmTriangleEdge(255, 255, 0.6);
  /* generates a grid using GL4Dummies */
  _geom[7] = gl4dgGenGrid2dFromHeightMapf(255, 255, hm);
  free(hm);

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
  glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
  glEnable(GL_DEPTH_TEST);
  _pId  = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  resize(800, 800);
}

/*!\brief function called by GL4Dummies' loop at resize. Sets the
 *  projection matrix and the viewport according to the given width
 *  and height.
 * \param w window width
 * \param h window height
 */
static void resize(int w, int h) {
  glViewport(0, 0, w, h);
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.5, 0.5, -0.5 * h / w, 0.5 * h / w, 1.0, 10.0);
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
  case 'n':
    _gId = (_gId + 1) % (sizeof _geom / sizeof *_geom);
    break;
  case 'b':
    _gId = (_gId - 1 + (sizeof _geom / sizeof *_geom)) % (sizeof _geom / sizeof *_geom);
    break;
  case 'q':
    exit(0);
    /* when 'w' pressed, toggle between line and filled mode */
  case 'w':
    glGetIntegerv(GL_POLYGON_MODE, v);
    if(v[0] == GL_FILL) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      glLineWidth(2.0);
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
  /* clears the OpenGL color buffer and depth buffer */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  /* sets the current program shader to _pId */
  glUseProgram(_pId);
  /* loads the identity matrix in the current GL4Dummies matrix ("modelViewMatrix") */
  gl4duLoadIdentityf();

  /* rotation around x-axis, old y-axis and tranlation */
  gl4duPushMatrix(); {
    gl4duTranslatef(0, 0, -5);
    gl4duRotatef(_ry, 0, cos(_rx * M_PI / 180.0), sin(_rx * M_PI / 180.0));
    gl4duRotatef(_rx, 1, 0, 0);
    gl4duSendMatrices();
  } gl4duPopMatrix();
  gl4dgDraw(_geom[_gId]);
  gl4dfSobelSetMixMode(GL4DF_SOBEL_MIX_MULT);
  gl4dfSobel(0, 0, GL_FALSE);
}

/*!\brief function called at exit, it cleans-up SDL2_mixer and GL4Dummies.*/
static void quit(void) {
  gl4duClean(GL4DU_ALL);
}
