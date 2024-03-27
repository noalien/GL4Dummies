/*!\file window.c
 *
 * \brief using GL4Dummies and Assimp Library to load 3D models or scenes.
 *
 * \author Farès Belhadj amsi@up8.edu
 * \date February 14 2017, modified on March 24, 2024
 */

#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4df.h>
#include <SDL_image.h>
#include "assimp.h"

/*!\brief opened window width */
static int _windowWidth = 1024;
/*!\brief opened window height */
static int _windowHeight = 768;
/*!\brief GLSL program Id */
static GLuint _pId = 0;
/*!\brief sphere Id */
static GLuint _sphereId = 0;
/*!\brief Id de la texture du ciel*/
static GLuint _texId = 0;

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
static cam_t _cam = {0.0f, 0.0f, 1.0f, 0.0f};

/*!\brief toggle y-axis rotation pause */
static GLboolean _pause = GL_TRUE;

/*!\brief toggle view focused on the scene center */
static GLboolean _center_view = GL_FALSE;

/*!\brief identifiant de la scene générée par assimpGenScene */
static GLuint _id_scene = 0;

/*!\brief rotation angles according to axis (0 = x, 1 = y, 2 = z) 
 * \todo améliorer l'interface et ajouter rotations/zoom à la souris */
static GLfloat rot[3] = {0, 0, 0};

static void initGL(void);
static void quit(void);
static void resize(int w, int h);
static void idle(void);
static void draw(void);
static void keydown(int keycode);
static void keyup(int keycode);

/*!\brief the main function.
 */
int main(int argc, char ** argv) {
  if(argc != 2) {
    fprintf(stderr, "usage: %s <3d_file>\n", argv[0]);
    return 1;
  }
  if(!gl4duwCreateWindow(argc, argv, "Exemple de loader de modèles 3D", GL4DW_POS_UNDEFINED, GL4DW_POS_UNDEFINED,
                         _windowWidth, _windowHeight, GL4DW_RESIZABLE | GL4DW_SHOWN))
    return 1;
  _id_scene = assimpGenScene(argv[1]);
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

/*!\brief function that initialize OpenGL / GL4D params and objects.
 */
static void initGL(void) {
  _sphereId = gl4dgGenSpheref(20, 20);
  glEnable(GL_DEPTH_TEST);
  _pId = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  resize(_windowWidth, _windowHeight);

  glGenTextures(1, &_texId);
  glBindTexture(GL_TEXTURE_2D, _texId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
  srand(42);
  GLfloat * fractale = gl4dmTriangleEdge (513, 513, 0.3f);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 513, 513, 0, GL_RED, GL_FLOAT, fractale);
  free(fractale);
  glBindTexture(GL_TEXTURE_2D, 0);
  
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
  glViewport(0.0f, 0.0f, _windowWidth, _windowHeight);
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  /* gl4duFrustumf(-0.005f, 0.005f, -0.005f * _windowHeight / _windowWidth, 0.005f * _windowHeight / _windowWidth, 0.01f, 1000.0f); */
  /* même résultat en utilisant la fonction perspective */
  gl4duPerspectivef(60.0f, (GLfloat)_windowWidth/(GLfloat)_windowHeight, 0.01f, 1000.0f);
  gl4duBindMatrix("modelViewMatrix");
}

/*!\brief function called by GL4Dummies' loop at idle.
 * 
 * uses the virtual keyboard states to move the camera according to
 * direction, orientation and time (dt = delta-time)
 */
static void idle(void) {
  static float t0 = 0.0f;
  float t, dt, dtheta = M_PI, step = 1.0f;
  dt = ((t = (float)gl4dGetElapsedTime()) - t0) / 1000.0f;
  t0 = t;
  if(_keys[KLEFT])
    _cam.theta += dt * dtheta;
  if(_keys[KRIGHT])
    _cam.theta -= dt * dtheta;
  if(_keys[KPAGEUP]) {
    _cam.y += dt * 0.5f * step;
  }
  if(_keys[KPAGEDOWN]) {
    _cam.y -= dt * 0.5f * step;
  }
  if(_keys[KUP]) {
    _cam.x += -dt * step * sin(_cam.theta);
    _cam.z += -dt * step * cos(_cam.theta);
  }
  if(_keys[KDOWN]) {
    _cam.x += dt * step * sin(_cam.theta);
    _cam.z += dt * step * cos(_cam.theta);
  }
  if(!_pause)
    rot[1] += 90.0f * dt;
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
  case GL4DK_LEFT:
    _keys[KLEFT] = 1;
    break;
  case GL4DK_RIGHT:
    _keys[KRIGHT] = 1;
    break;
  case GL4DK_UP:
    _keys[KUP] = 1;
    break;
  case GL4DK_DOWN:
    _keys[KDOWN] = 1;
    break;
  case GL4DK_d:
    _keys[KPAGEDOWN] = 1;
    break;
  case GL4DK_u:
    _keys[KPAGEUP] = 1;
    break;
  case GL4DK_ESCAPE:
  case 'q':
    exit(0);
    /* when 'w' pressed, toggle between line and filled mode */
  case 'w':
    glGetIntegerv(GL_POLYGON_MODE, v);
    if(v[0] == GL_FILL) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      glLineWidth(3.0f);
    } else {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      glLineWidth(1.0f);
    }
    break;
  case GL4DK_SPACE:
    _pause = !_pause;
    break;
  case GL4DK_c:
    _center_view = !_center_view;
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
  case GL4DK_LEFT:
    _keys[KLEFT] = 0;
    break;
  case GL4DK_RIGHT:
    _keys[KRIGHT] = 0;
    break;
  case GL4DK_UP:
    _keys[KUP] = 0;
    break;
  case GL4DK_DOWN:
    _keys[KDOWN] = 0;
    break;
  case GL4DK_d:
    _keys[KPAGEDOWN] = 0;
    break;
  case GL4DK_u:
    _keys[KPAGEUP] = 0;
    break;
  default:
    break;
  }
}

/*!\brief function called on each GL4Dummies' display event. It draws
 * the scene with its given params.
 */
static void draw(void) {
  GLfloat lum[4] = {0.0f, 0.0f, 5.0f, 1.0f};
  glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(_pId);

  glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lum);

  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();

  gl4duLookAtf(_cam.x, _cam.y, _cam.z,
	       _cam.x - sin(_cam.theta),  _center_view ? 0.0f : _cam.y,  _cam.z - cos(_cam.theta),
	       0.0f, 1.0f, 0.0f);


  /* Skydome */

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _texId);
  glUniform4f(glGetUniformLocation(_pId, "diffuse_color"), 1.0f, 1.0f, 1.0f, 1.0f);
  glUniform4f(glGetUniformLocation(_pId, "ambient_color"), 1.0f, 1.0f, 1.0f, 1.0f);
  glUniform1i(glGetUniformLocation(_pId, "myTexture"), 0);
  glUniform1i(glGetUniformLocation(_pId, "hasTexture"), 1);
  glUniform1i(glGetUniformLocation(_pId, "sky"), 1);
  gl4duPushMatrix();
  gl4duTranslatef(_cam.x, _cam.y, _cam.z);
  gl4duScalef(10.0f, 10.0f, 10.0);
  gl4duSendMatrices();
  gl4duPopMatrix();
  glCullFace(GL_FRONT);
  gl4dgDraw(_sphereId);
  glCullFace(GL_BACK);

  glUniform1i(glGetUniformLocation(_pId, "sky"), 0);
  glBindTexture(GL_TEXTURE_2D, 0);
  /* pour du flou gaussien */
  gl4dfBlur (0, 0, 5, 1, 0 /* weight */, GL_FALSE);
  /* pour le contour du cell-shading */
  gl4dfSobelSetMixMode(GL4DF_SOBEL_MIX_MULT);
  gl4dfSobel (0, 0, GL_FALSE);

  /* l'objet chargé avec assimp */  
  gl4duPushMatrix();
  gl4duRotatef(rot[1], 0.0f, 1.0f, 0.0f);
  assimpDrawScene(_id_scene);
  gl4duPopMatrix();
}

/*!\brief function called at exit, it cleans all created GL4D objects.
 */
static void quit(void) {
  if(_texId) {
    glDeleteTextures(1, &_texId);
    _texId = 0;
  }
  gl4duClean(GL4DU_ALL);
}

