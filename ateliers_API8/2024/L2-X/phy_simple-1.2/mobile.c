#include "mobile.h"

static mobile_t * _mobiles = NULL;
static int _nb_mobiles = 0;
static const float _gy = 9.8f / 2.0f;
static vec3_t _g = {0.0f, -_gy, 0.0f};

void mobile_update_g(GLfloat a) {
  _g.x = -_gy * sin(a);
  _g.y = _gy * cos(a);
  for(int i = 0; i < _nb_mobiles; ++i) {
    _mobiles[i].a = _g;
  }
}

void mobile_init(int n) {
  int i;
  _nb_mobiles = n;
  _mobiles = malloc(_nb_mobiles * sizeof *_mobiles);
  assert(_mobiles);
  for(i = 0; i < _nb_mobiles; ++i) {
    _mobiles[i].p.x = gl4dmSURand();
    _mobiles[i].p.y = /* gl4dmSURand(); // */0.0f;
    _mobiles[i].p.z = gl4dmSURand();

    _mobiles[i].v.x = 0.2f * gl4dmSURand();
    _mobiles[i].v.z = 0.2f * gl4dmSURand();
    _mobiles[i].v.y = 0.5f + 0.25f * gl4dmURand();

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
    int col = 0;
    if(_mobiles[i].p.y >= 1.0f - _mobiles[i].r && _mobiles[i].v.y > 0.0f) {
      _mobiles[i].v.y = -_mobiles[i].v.y;
      col = 1;
    }
    if(_mobiles[i].p.y <= -1.0f + _mobiles[i].r && _mobiles[i].v.y < 0.0f) {
      _mobiles[i].v.y = -_mobiles[i].v.y;
      col = 1;
    } /* else */ {
      _mobiles[i].v.x += _mobiles[i].a.x * dt;
      _mobiles[i].v.y += _mobiles[i].a.y * dt;
      _mobiles[i].v.z += _mobiles[i].a.z * dt;
    }
    
    if( (_mobiles[i].p.x <= -1.0f + _mobiles[i].r && _mobiles[i].v.x < 0.0f) ||
	(_mobiles[i].p.x >= 1.0f - _mobiles[i].r && _mobiles[i].v.x > 0.0f) ) {
      _mobiles[i].v.x = -_mobiles[i].v.x;
      col = 1;
    }
    
    if( (_mobiles[i].p.z <= -1.0f + _mobiles[i].r && _mobiles[i].v.z < 0.0f) ||
	(_mobiles[i].p.z >= 1.0f - _mobiles[i].r && _mobiles[i].v.z > 0.0f) ) {
      _mobiles[i].v.z = -_mobiles[i].v.z;
      col = 1;
    }
    
    _mobiles[i].p.x += _mobiles[i].v.x * dt;
    _mobiles[i].p.y += _mobiles[i].v.y * dt;
    _mobiles[i].p.z += _mobiles[i].v.z * dt;
    /* absorption d'une partie de l'energie cinétique */
    if(col) {
      const float pc = 0.9f;
      _mobiles[i].v.x *= pc;
      _mobiles[i].v.y *= pc;
      _mobiles[i].v.z *= pc;
    }
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
