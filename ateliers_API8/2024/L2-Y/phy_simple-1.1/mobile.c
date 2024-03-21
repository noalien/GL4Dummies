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
    _mobiles[i].p.x = gl4dmSURand(); //0.0f;
    _mobiles[i].p.y = gl4dmURand(); //1.0f;
    _mobiles[i].p.z = gl4dmSURand(); //0.0f;

    _mobiles[i].v.x = 0.2f * gl4dmSURand();
    _mobiles[i].v.z = 0.2f * gl4dmSURand();
    _mobiles[i].v.y = 2.0f * (0.5f + gl4dmURand());

    _mobiles[i].a = _g;
    
    _mobiles[i].couleur.x = gl4dmURand();
    _mobiles[i].couleur.y = gl4dmURand();
    _mobiles[i].couleur.z = gl4dmURand();

    _mobiles[i].r = 0.05f;    
  }
}

void _collision_inter_mobiles(void) {
  int i, j;
  for(i = 0; i < _nb_mobiles; ++i) {
    for(j = i + 1; j < _nb_mobiles; ++j) {
      float sr2 = _mobiles[i].r + _mobiles[j].r;
      sr2 = sr2 * sr2; /* carre de la somme des rayons */
      float dx = _mobiles[i].p.x - _mobiles[j].p.x;
      float dy = _mobiles[i].p.y - _mobiles[j].p.y;
      float dz = _mobiles[i].p.z - _mobiles[j].p.z;
      float d2 = dx * dx + dy * dy + dz * dz; /* carre de la distance */
      if(d2 <= sr2) {
	vec3_t temp = _mobiles[i].v;
	_mobiles[i].v = _mobiles[j].v;
	_mobiles[j].v = temp;
      }
    }
  }
}

void mobile_simu(void) {
  int i;
  static double t0 = 0;
  double t = gl4dGetElapsedTime(), dt = (t - t0) / 1000.0;
  t0 = t;
  for(i = 0; i < _nb_mobiles; ++i) {
    int collision_mur = 0;
    if(_mobiles[i].p.y <= _mobiles[i].r && _mobiles[i].v.y < 0.0f) {
      _mobiles[i].v.y = -_mobiles[i].v.y;
      collision_mur = 1;
    } else {
      _mobiles[i].v.x += _mobiles[i].a.x * dt;
      _mobiles[i].v.y += _mobiles[i].a.y * dt;
      _mobiles[i].v.z += _mobiles[i].a.z * dt;
    }
    
    if( (_mobiles[i].p.x <= -1.0f + _mobiles[i].r && _mobiles[i].v.x < 0.0f) ||
	(_mobiles[i].p.x >= 1.0f - _mobiles[i].r && _mobiles[i].v.x > 0.0f) ) {
      _mobiles[i].v.x = -_mobiles[i].v.x;
      collision_mur = 1;
    }
    if( (_mobiles[i].p.z <= -1.0f + _mobiles[i].r && _mobiles[i].v.z < 0.0f) ||
	(_mobiles[i].p.z >= 1.0f - _mobiles[i].r && _mobiles[i].v.z > 0.0f) ) {
      _mobiles[i].v.z = -_mobiles[i].v.z;
      collision_mur = 1;
    }
    if(collision_mur) {
      /* version longue */
      float n = sqrt(_mobiles[i].v.x * _mobiles[i].v.x +
		     _mobiles[i].v.y * _mobiles[i].v.y +
		     _mobiles[i].v.z * _mobiles[i].v.z);
      vec3_t vp = _mobiles[i].v;
      vp.x /= n;
      vp.y /= n;
      vp.z /= n;
      _mobiles[i].v.x = vp.x * (0.95 * n);
      _mobiles[i].v.y = vp.y * (0.95 * n);
      _mobiles[i].v.z = vp.z * (0.95 * n);
    }
    _mobiles[i].p.x += _mobiles[i].v.x * dt;
    _mobiles[i].p.y += _mobiles[i].v.y * dt;
    _mobiles[i].p.z += _mobiles[i].v.z * dt;
  }
  //_collision_inter_mobiles();
}

void mobile_draw(GLuint pId, GLuint oId) {
  int i;
  gl4duPushMatrix();
  for(i = 0; i < _nb_mobiles; ++i) {
    gl4duLoadIdentityf();
    gl4duTranslatef(_mobiles[i].p.x, _mobiles[i].p.y, _mobiles[i].p.z);
    gl4duScalef(_mobiles[i].r, _mobiles[i].r, _mobiles[i].r);
    /* envoyer les matrice GL4D au programme GPU OpenGL (en cours) */
    gl4duSendMatrices();
    /* set la variable "uniform" couleur pour mettre du rouge */
    glUniform4f(glGetUniformLocation(pId, "couleur"), _mobiles[i].couleur.x, _mobiles[i].couleur.y, _mobiles[i].couleur.z, 1.0f);
    gl4dgDraw(oId);
  }
  gl4duPopMatrix();
}

void mobile_quit(void) {
  if(_mobiles) {
    free(_mobiles);
    _mobiles = NULL;
  }
}
