/*!\file window.c
 *
 * \brief Walking on finite plane with skydome textured with a
 * triangle-edge midpoint-displacement algorithm.
 *
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date February 19 2018
 */
#include <GL4D/gl4dg.h>
#include <GL4D/gl4duw_SDL2.h>

static void quit(void);
static void initGL(void);
static void initData(void);
static void resize(int w, int h);
static void idle(void);
static void keydown(int keycode);
static void keyup(int keycode);
static void pmotion(int x, int y);
static void draw(void);

/*!\brief opened window width and height */
static int _wW = 800, _wH = 600;
/*!\brief mouse position (modified by pmotion function) */
static int _xm = 400, _ym = 300;
/*!\brief Quad geometry Id  */
static GLuint _plane = 0;
/*!\brief Sphere geometry Id  */
static GLuint _sphere = 0;
/*!\brief GLSL program Id */
static GLuint _pId = 0;
/*!\brief plane texture Id */
static GLuint _planeTexId = 0;
/*!\brief sky texture Id */
static GLuint _skyTexId = 0;
/*!\brief compass texture Id */
static GLuint _compassTexId = 0;
/*!\brief plane scale factor */
static GLfloat _planeScale = 100.0f;
/*!\brief boolean to toggle anisotropic filtering */
static GLboolean _anisotropic = GL_FALSE;
/*!\brief boolean to toggle mipmapping */
static GLboolean _mipmap = GL_FALSE;
/*!\brief boolean to toggle scene fog */
static GLboolean _fog = GL_FALSE;

/*!\brief enum that index keyboard mapping for direction commands */
enum kyes_t {
  KLEFT = 0,
  KRIGHT,
  KUP,
  KDOWN
};

/*!\brief virtual keyboard for direction commands */
static GLuint _keys[] = {0, 0, 0, 0};

typedef struct cam_t cam_t;
/*!\brief a data structure for storing camera position and
 * orientation */
struct cam_t {
  GLfloat x, z;
  GLfloat theta;
};

/*!\brief the used camera */
static cam_t _cam = {0, 0, 0};


/*!\brief creates the window, initializes OpenGL parameters,
 * initializes data and maps callback functions */
int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "GL4Dummies", 10, 10,
                         _wW, _wH, GL4DW_RESIZABLE | GL4DW_SHOWN))
    return 1;
  initGL();
  initData();
  atexit(quit);
  gl4duwResizeFunc(resize);
  gl4duwKeyUpFunc(keyup);
  gl4duwKeyDownFunc(keydown);
  gl4duwPassiveMotionFunc(pmotion);
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
  glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
  _pId  = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelMatrix");
  gl4duGenMatrix(GL_FLOAT, "viewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  resize(_wW, _wH);
}

/*!\brief initializes data : 
 *
 * creates 3D objects (plane and sphere) and 2D textures.
 */
static void initData(void) {
  /* the checkboard texture */
  GLuint check[] = {-1, 255 << 24, 128 << 24, -1};
  /* a red-white texture used to draw a compass */
  GLuint northsouth[] = {(255 << 24) + 255, -1};
  /* a fractal texture generated usind a midpoint displacement algorithm */
  GLfloat * hm = gl4dmTriangleEdge(257, 257, 0.3);
  /* generates a quad using GL4Dummies */
  _plane = gl4dgGenQuadf();
  /* generates a sphere using GL4Dummies */
  _sphere = gl4dgGenSpheref(10, 10);

  /* creation and parametrization of the plane texture */
  glGenTextures(1, &_planeTexId);
  glBindTexture(GL_TEXTURE_2D, _planeTexId);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, check);

  /* creation and parametrization of the sky texture */
  glGenTextures(1, &_skyTexId);
  glBindTexture(GL_TEXTURE_2D, _skyTexId);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 257, 257, 0, GL_RED, GL_FLOAT, hm);
  free(hm);

  /* creation and parametrization of the compass texture */
  glGenTextures(1, &_compassTexId);
  glBindTexture(GL_TEXTURE_2D, _compassTexId);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, northsouth);

  glBindTexture(GL_TEXTURE_2D, 0);
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
  gl4duFrustumf(-0.5, 0.5, -0.5 * _wH / _wW, 0.5 * _wH / _wW, 1.0, _planeScale + 1.0);
}

/*!\brief function called by GL4Dummies' loop at idle.
 * 
 * uses the virtual keyboard states to move the camera according to
 * direction, orientation and time (dt = delta-time)
 */
static void idle(void) {
  double dt, dtheta = M_PI, step = 5.0;
  static double t0 = 0, t;
  dt = ((t = gl4dGetElapsedTime()) - t0) / 1000.0;
  t0 = t;
  if(_keys[KLEFT])
    _cam.theta += dt * dtheta;
  if(_keys[KRIGHT])
    _cam.theta -= dt * dtheta;
  if(_keys[KUP]) {
    _cam.x += -dt * step * sin(_cam.theta);
    _cam.z += -dt * step * cos(_cam.theta);
  }
  if(_keys[KDOWN]) {
    _cam.x += dt * step * sin(_cam.theta);
    _cam.z += dt * step * cos(_cam.theta);
  }
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
  case GL4DK_ESCAPE:
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
  case 'f':
    _fog = !_fog;
    break;
    /* when 'm' pressed, toggle between mipmapping or nearest for the plane texture */
  case 'm': {
    _mipmap = !_mipmap;
    glBindTexture(GL_TEXTURE_2D, _planeTexId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    break;
  }
    /* when 'a' pressed, toggle on/off the anisotropic mode */
  case 'a': {
    _anisotropic = !_anisotropic;
    /* l'Anisotropic sous GL ne fonctionne que si la version de la
       bibliothèque le supporte ; supprimer le bloc ci-après si
       problème à la compilation. */
#ifdef GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
    GLfloat max;
    glBindTexture(GL_TEXTURE_2D, _planeTexId);
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, _anisotropic ? max : 1.0f);
    glBindTexture(GL_TEXTURE_2D, 0);
#endif
    break;
  }
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
  default:
    break;
  }
}

/*!\brief function called by GL4Dummies' loop at the passive mouse motion event.*/
static void pmotion(int x, int y) {
  _xm = x; 
  _ym = y;
}

/*!\brief function called by GL4Dummies' loop at draw.*/
static void draw(void) {
  /* clears the OpenGL color buffer and depth buffer */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  /* sets the current program shader to _pId */
  glUseProgram(_pId);
  gl4duBindMatrix("viewMatrix");
  /* loads the identity matrix in the current GL4Dummies matrix ("viewMatrix") */
  gl4duLoadIdentityf();
  /* modifies the current matrix to simulate camera position and orientation in the scene */
  /* see gl4duLookAtf documentation or gluLookAt documentation */
  gl4duLookAtf(_cam.x, 1.0, _cam.z, 
	       _cam.x - sin(_cam.theta), 1.0 - (_ym - (_wH >> 1)) / (GLfloat)_wH, _cam.z - cos(_cam.theta), 
	       0.0, 1.0,0.0);
  gl4duBindMatrix("modelMatrix");
  /* loads the identity matrix in the current GL4Dummies matrix ("modelMatrix") */
  gl4duLoadIdentityf();
  /* sets the current texture stage to 0 */
  glActiveTexture(GL_TEXTURE0);
  /* tells the pId program that "tex" is set to stage 0 */
  glUniform1i(glGetUniformLocation(_pId, "tex"), 0);
  /* tells the pId program what is the value of fog */
  glUniform1i(glGetUniformLocation(_pId, "fog"), _fog);

  /* pushs (saves) the current matrix (modelMatrix), scales, rotates,
   * sends matrices to pId and then pops (restore) the matrix */
  gl4duPushMatrix(); {
    gl4duRotatef(-90, 1, 0, 0);
    gl4duScalef(_planeScale, _planeScale, 1);
    gl4duSendMatrices();
  } gl4duPopMatrix();
  /* culls the back faces */
  glCullFace(GL_BACK);
  /* uses the checkboard texture */
  glBindTexture(GL_TEXTURE_2D, _planeTexId);
  /* sets in pId the uniform variable texRepeat to the plane scale */
  glUniform1f(glGetUniformLocation(_pId, "texRepeat"), _planeScale);
  /* tells pId that the sky is false */
  glUniform1i(glGetUniformLocation(_pId, "sky"), 0);
  /* draws the plane */
  gl4dgDraw(_plane);

  /* pushs (saves) the current matrix (modelMatrix), scales,
   * translates, sends matrices to pId and then pops (restore) the
   * matrix */
  /* this part means that the skydome always follow the camera position */
  gl4duPushMatrix(); {
    gl4duTranslatef(_cam.x, 1.0, _cam.z);
    gl4duScalef(_planeScale, _planeScale, _planeScale);
    gl4duSendMatrices();
  } gl4duPopMatrix();
  /* culls the front faces (because we are in the skydome) */
  glCullFace(GL_FRONT);
  /* uses the sky texture (fractal cloud) */
  glBindTexture(GL_TEXTURE_2D, _skyTexId);
  /* texture repeat only once */
  glUniform1f(glGetUniformLocation(_pId, "texRepeat"), 1);
  /* tells pId that the sky is true (we are drawing the sky dome) */
  glUniform1i(glGetUniformLocation(_pId, "sky"), 1);
  /* draws the sky dome */
  gl4dgDraw(_sphere);
  
  /* the compass should be drawn in an orthographic projection, thus
   * we should bind the projection matrix; save it; load identity;
   * bind the model-view matrix; modify it to place the compass at the
   * top left of the screen and rotate the compass according to the
   * camera orientation (theta); send matrices; restore the model-view
   * matrix; bind the projection matrix and restore it; and then
   * re-bind the model-view matrix for after.*/
  gl4duBindMatrix("projectionMatrix");
  gl4duPushMatrix(); {
    gl4duLoadIdentityf();
    gl4duBindMatrix("modelMatrix");
    gl4duPushMatrix(); {
      gl4duLoadIdentityf();
      gl4duTranslatef(-0.75, 0.7, 0.0);
      gl4duRotatef(_cam.theta * 180.0 / M_PI, 0, 0, 1);
      gl4duScalef(0.03 / 5.0, 1.0 / 5.0, 1.0 / 5.0);
      gl4duBindMatrix("viewMatrix");
      gl4duPushMatrix(); {
	gl4duLoadIdentityf();
	gl4duSendMatrices();
      } gl4duPopMatrix();
      gl4duBindMatrix("modelMatrix");
    } gl4duPopMatrix();
    gl4duBindMatrix("projectionMatrix");
  } gl4duPopMatrix();
  gl4duBindMatrix("modelMatrix");
  /* disables cull facing and depth testing */
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  /* uses the compass texture */
  glBindTexture(GL_TEXTURE_2D, _compassTexId);
  /* texture repeat only once */
  glUniform1f(glGetUniformLocation(_pId, "texRepeat"), 1);
  /* tells pId that the sky is false */
  glUniform1i(glGetUniformLocation(_pId, "sky"), 0);
  /* draws the compass */
  gl4dgDraw(_plane);
  /* enables cull facing and depth testing */
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
}

/*!\brief function called at exit. Frees used textures and clean-up
 * GL4Dummies.*/
static void quit(void) {
  if(_planeTexId)
    glDeleteTextures(1, &_planeTexId);
  if(_skyTexId)
    glDeleteTextures(1, &_skyTexId);
  if(_compassTexId)
    glDeleteTextures(1, &_compassTexId);
  gl4duClean(GL4DU_ALL);
}
