#include "mobile.h"
#include <assert.h>

static mobile_t * _mobiles = NULL;
static int _nmobiles = 0;

void mobile_init(int n) {
  int i;
  _nmobiles = n;
  if(_mobiles) {
    free(_mobiles);
    _mobiles = NULL;
  }
  _mobiles = malloc(_nmobiles * sizeof *_mobiles);
  assert(_mobiles);
  for(i = 0; i < _nmobiles; i++) {
    GLubyte r, g, b;
    _mobiles[i].r = (gl4dpGetWidth() / 20.0f) * (0.2f + 0.8f * rand() / (RAND_MAX + 1.0));
    _mobiles[i].x = _mobiles[i].r + (gl4dpGetWidth()  - 2.0f * _mobiles[i].r) * (rand() / (RAND_MAX + 1.0));
    _mobiles[i].y = _mobiles[i].r + (gl4dpGetHeight() - 2.0f * _mobiles[i].r) * (rand() / (RAND_MAX + 1.0));
    _mobiles[i].vx = gl4dpGetWidth()  * ((rand() / (RAND_MAX + 1.0)) - 0.5);
    _mobiles[i].vy = gl4dpGetHeight() * ((rand() / (RAND_MAX + 1.0)) - 0.5);
    r = rand()&0xFF; g = rand()&0xFF; b = rand()&0xFF;
    _mobiles[i].c = RGB(r, g, b);
  }
}

static void frottements(int i, float kx, float ky) {
  const float dt = 1.0 / 60.0; /* pour que le frottement ne dépende pas de la puissance de la machine */
  double x = fabs(_mobiles[i].vx), y = fabs(_mobiles[i].vy);
  _mobiles[i].vx = MAX(x - kx * x * x * dt * dt - 2.0 * kx, 0.0) * SIGN(_mobiles[i].vx);
  _mobiles[i].vy = MAX(y - ky * y * y * dt * dt - 2.0 * ky, 0.0) * SIGN(_mobiles[i].vy);
}

static void inter_mobile(double dt) {
  int i, j;
  float dx, dy, d, vx, vy, de, ndx, ndy, nde;
  for(i = 0; i < _nmobiles; i++) {
    for(j = i + 1; j < _nmobiles; j++) {
      dx = _mobiles[i].x - _mobiles[j].x;
      dy = _mobiles[i].y - _mobiles[j].y;
      d = _mobiles[i].r + _mobiles[j].r;
      if((de = dx * dx + dy * dy) < d * d) {
	ndx = _mobiles[i].x + _mobiles[j].vx * dt - (_mobiles[j].x + _mobiles[i].vx * dt);
	ndy = _mobiles[i].y + _mobiles[j].vy * dt - (_mobiles[j].y + _mobiles[i].vy * dt);
	nde = ndx * ndx + ndy * ndy;
	if(nde < de) continue;
	vx = _mobiles[i].vx;
	vy = _mobiles[i].vy;
	_mobiles[i].vx = _mobiles[j].vx;
	_mobiles[i].vy = _mobiles[j].vy;
	_mobiles[j].vx = vx;
	_mobiles[j].vy = vy;
	frottements(i, 1.0, 1.0);
      }
    }
  }
}

void mobile_move(void) {
  const double G = -980;
  static Uint32 t0 = 0;
  Uint32 t = SDL_GetTicks();
  double dt = (t - t0) / 1000.0, d;
  t0 = t;
  int i, collision_x, collision_bas;
  /* décommenter pour obtenir la collision entre mobiles */
  inter_mobile(dt);
  for(i = 0; i < _nmobiles; i++) {
    collision_x = collision_bas = 0;
    _mobiles[i].x += _mobiles[i].vx * dt;
    _mobiles[i].y += _mobiles[i].vy * dt;
    if( (d = _mobiles[i].x - _mobiles[i].r) <= 0 ) {
      if(_mobiles[i].vx < 0)
        _mobiles[i].vx = -_mobiles[i].vx;
      _mobiles[i].vx -= d;
      frottements(i, 1.0, 0.2);
      collision_x = 1;
    }
    if( (d = _mobiles[i].x + _mobiles[i].r - (gl4dpGetWidth() - 1)) >= 0 ) {
      if(_mobiles[i].vx > 0)
        _mobiles[i].vx = -_mobiles[i].vx;
      _mobiles[i].vx -= d;
      frottements(i, 1.0, 0.2);
      collision_x = 1;
    }
    if( (d = _mobiles[i].y - (_mobiles[i].r + 1)) <= 0 ) {
      if(_mobiles[i].vy < 0)
        _mobiles[i].vy = -_mobiles[i].vy;
      _mobiles[i].vy -= d;
      frottements(i, 0.2, 1.0);
      collision_bas = 1;
    }
    if(!collision_bas)
      _mobiles[i].vy += G * dt;
  }
}

void mobile_draw(void) {
  int i;
  for(i = 0; i < _nmobiles; i++) {
    gl4dpSetColor(_mobiles[i].c);
    gl4dpFilledCircle(_mobiles[i].x, _mobiles[i].y, _mobiles[i].r);
  }
}

void mobile_delete(void) {
  if(_mobiles) {
    free(_mobiles);
    _mobiles = NULL;
  }
}
