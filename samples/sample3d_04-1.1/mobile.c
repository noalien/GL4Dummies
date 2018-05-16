/*!\file mobile.c
 *
 * \brief Bibliothèque de gestion de mobiles
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr 
 * \date March 10 2017
 */
#include <stdlib.h>
#include <assert.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4dg.h>

/*!\typedef structure pour mobile */
typedef struct mobile_t mobile_t;
struct mobile_t {
  GLuint id;
  GLfloat x, y, z, r;
  GLfloat vx, vy, vz;
  GLfloat color[4];
  GLboolean freeze;
};

static mobile_t * _mobile = NULL;
static int _nb_mobiles = 0;
static GLfloat _width = 1, _depth = 1;
static GLfloat _gravity[3] = {0, -9.8 * 3.0, 0};

static void quit(void);
static void frottements(int i, GLfloat kx, GLfloat ky, GLfloat kz);
static double get_dt(void);

void mobileInit(int n, GLfloat width, GLfloat depth) {
  int i;
  _width = width; _depth = depth;
  _nb_mobiles = n;
  if(_mobile) {
    free(_mobile);
    _mobile = NULL;
  } else
    atexit(quit);
  _mobile = malloc(_nb_mobiles * sizeof *_mobile);
  assert(_mobile);
  for(i = 0; i < _nb_mobiles; i++) {
    _mobile[i].r = 0.05f + 0.5f * gl4dmURand();
    _mobile[i].x = gl4dmSURand() * _width - _mobile[i].r;
    _mobile[i].z = gl4dmSURand() * _depth - _mobile[i].r;
    _mobile[i].y = _depth;
    _mobile[i].vx = 3.0f * gl4dmSURand(); 
    _mobile[i].vy = 3.0f * gl4dmURand(); 
    _mobile[i].vz = 3.0f * gl4dmSURand();
    _mobile[i].color[0] = gl4dmURand();
    _mobile[i].color[1] = gl4dmURand();
    _mobile[i].color[2] = gl4dmURand();
    _mobile[i].color[3] = 1.0f;
    _mobile[i].freeze = GL_FALSE;
  }
}

#define EPSILON 0.00001f

void mobileSetFreeze(GLuint id, GLboolean freeze) {
  _mobile[id].freeze = freeze;
}

void mobileGetCoords(GLuint id, GLfloat * coords) {
  coords[0] = _mobile[id].x;
  coords[1] = _mobile[id].y;
  coords[2] = _mobile[id].z;
}

void mobileSetCoords(GLuint id, GLfloat * coords) {
  _mobile[id].x = coords[0];
  _mobile[id].y = coords[1];
  _mobile[id].z = coords[2];
}

void mobileMove(void) {
  int i;
  GLfloat dt = get_dt(), d;
  for(i = 0; i < _nb_mobiles; i++) {
    if(_mobile[i].freeze) continue;
    _mobile[i].x += _mobile[i].vx * dt;
    _mobile[i].y += _mobile[i].vy * dt;
    _mobile[i].z += _mobile[i].vz * dt;
    if( (d = _mobile[i].x - _mobile[i].r + _width) <= EPSILON ||
	(d = _mobile[i].x + _mobile[i].r - _width) >= -EPSILON ) {
      if(d * _mobile[i].vx > 0)
    	_mobile[i].vx = -_mobile[i].vx;
      _mobile[i].x -= d - EPSILON;
      frottements(i, 0.1f, 0.0f, 0.1f);
    }
    if( (d = _mobile[i].y - _mobile[i].r) <= EPSILON ) {
      if(_mobile[i].vy < 0)
	_mobile[i].vy = -_mobile[i].vy;
      _mobile[i].y -= d - EPSILON;
      frottements(i, MIN(1, dt), 0.1f, MIN(1, dt));
    }
    if( (d = _mobile[i].z - _mobile[i].r + _depth) <= EPSILON ||
	(d = _mobile[i].z + _mobile[i].r - _depth) >= -EPSILON ) {
      if(d * _mobile[i].vz > 0)
    	_mobile[i].vz = -_mobile[i].vz;
      _mobile[i].z -= d - EPSILON;
      frottements(i, 0.1f, 0.0f, 0.1f);
    }
    _mobile[i].vx += _gravity[0] * dt;
    _mobile[i].vy += _gravity[1] * dt;
    _mobile[i].vz += _gravity[2] * dt;
  }
}

void mobileDraw(GLuint obj) {
  int i;
  GLint pId;
  glGetIntegerv(GL_CURRENT_PROGRAM, &pId);
  for(i = 0; i < _nb_mobiles; i++) {
    gl4duPushMatrix();
    gl4duTranslatef(_mobile[i].x, _mobile[i].y, _mobile[i].z);
    gl4duScalef(_mobile[i].r, _mobile[i].r, _mobile[i].r);
    gl4duSendMatrices();
    gl4duPopMatrix();
    glUniform1i(glGetUniformLocation(pId, "id"), i + 2);
    glUniform4fv(glGetUniformLocation(pId, "couleur"), 1, _mobile[i].color);
    gl4dgDraw(obj);
  }
}

static void frottements(int i, GLfloat kx, GLfloat ky, GLfloat kz) {
  GLfloat vx = fabs(_mobile[i].vx), vy = fabs(_mobile[i].vy), vz = fabs(_mobile[i].vz);
  if(vx < EPSILON)
    _mobile[i].vx = 0;
  else
    _mobile[i].vx = (vx - kx * vx) * SIGN(_mobile[i].vx);
  if(vy < EPSILON)
    _mobile[i].vy = 0;
  else
    _mobile[i].vy = (vy - ky * vy) * SIGN(_mobile[i].vy);
  if(vz < EPSILON)
    _mobile[i].vz = 0;
  else
    _mobile[i].vz = (vz - kz * vz) * SIGN(_mobile[i].vz);
}

static void quit(void) {
  _nb_mobiles = 0;
  if(_mobile) {
    free(_mobile);
    _mobile = NULL;
  }
}

static double get_dt(void) {
  static double t0 = 0, t, dt;
  t = gl4dGetElapsedTime();
  dt = (t - t0) / 1000.0;
  t0 = t;
  return dt;
}

