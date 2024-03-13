#include "mobile.h"

static mobile_t * _mobiles = NULL;
static int _nb_mobiles = 0;

static const vec3_t _g = {0.0f, -9.8f / 3.0f, 0.0f};

void mobile_init(int n) {
  int i;
  _nb_mobiles = n;
  _mobiles = malloc(_nb_mobiles * sizeof *_mobiles);
  assert(_mobiles);
  for(i = 0; i < _nb_mobiles; ++i) {
    _mobiles[i].p.x = /* gl4dmSURand(); // */0.0f;
    _mobiles[i].p.y = /* gl4dmSURand(); // */1.0f;
    _mobiles[i].p.z = /* gl4dmSURand(); // */0.0f;

    _mobiles[i].v.x = 0.2f * gl4dmSURand();
    _mobiles[i].v.z = 0.2f * gl4dmSURand();
    _mobiles[i].v.y = 0.5f + gl4dmURand();

    _mobiles[i].a = _g;
    
    _mobiles[i].couleur.x = gl4dmURand();
    _mobiles[i].couleur.y = gl4dmURand();
    _mobiles[i].couleur.z = gl4dmURand();

    _mobiles[i].r = 0.05f + 0.05f * gl4dmURand();    
  }
}

void mobile_simu(void) {
  int i;
  static double t0 = 0;
  double t = gl4dGetElapsedTime(), dt = (t - t0) / 1000.0;
  t0 = t;
  for(i = 0; i < _nb_mobiles; ++i) {

    if(_mobiles[i].p.y <= _mobiles[i].r && _mobiles[i].v.y < 0.0f)
      _mobiles[i].v.y = -_mobiles[i].v.y;
    else {
      _mobiles[i].v.x += _mobiles[i].a.x * dt;
      _mobiles[i].v.y += _mobiles[i].a.y * dt;
      _mobiles[i].v.z += _mobiles[i].a.z * dt;
    }
    
    if( (_mobiles[i].p.x <= -1.0f + _mobiles[i].r && _mobiles[i].v.x < 0.0f) ||
	(_mobiles[i].p.x >= 1.0f - _mobiles[i].r && _mobiles[i].v.x > 0.0f) )
      _mobiles[i].v.x = -_mobiles[i].v.x;
    
    if( (_mobiles[i].p.z <= -1.0f + _mobiles[i].r && _mobiles[i].v.z < 0.0f) ||
	(_mobiles[i].p.z >= 1.0f - _mobiles[i].r && _mobiles[i].v.z > 0.0f) )
      _mobiles[i].v.z = -_mobiles[i].v.z;
    
    _mobiles[i].p.x += _mobiles[i].v.x * dt;
    _mobiles[i].p.y += _mobiles[i].v.y * dt;
    _mobiles[i].p.z += _mobiles[i].v.z * dt;
  }
}

void mobile_draw(GLuint pId, GLuint oId) {
  int i;
  for(i = 0; i < _nb_mobiles; ++i) {
    gl4duPushMatrix();
    gl4duTranslatef(_mobiles[i].p.x, _mobiles[i].p.y, _mobiles[i].p.z);
    gl4duScalef(_mobiles[i].r, _mobiles[i].r, _mobiles[i].r);
    gl4duRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    /* envoyer les matrice GL4D au programme GPU OpenGL (en cours) */
    gl4duSendMatrices();
    gl4duPopMatrix();
    /* set la variable "uniform" couleur pour mettre du rouge */
    glUniform4f(glGetUniformLocation(pId, "couleur"), _mobiles[i].couleur.x, _mobiles[i].couleur.y, _mobiles[i].couleur.z, 1.0f);
    gl4dgDraw(oId);
  }
}

void mobile_quit(void) {
  if(_mobiles) {
    free(_mobiles);
    _mobiles = NULL;
  }
}
