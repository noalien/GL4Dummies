#include "mobile.h"

static mobile_t _m;

static const GLfloat Gy = -980;

void mobileInit(void) {
  _m.x  = 400;
  _m.y  = 100;
  _m.vx = 100;
  _m.vy = 800;
  _m.r  = 30;
}

static void collision(void) {
  _m.vx = 0.95 * _m.vx;
  _m.vy = 0.95 * _m.vy;
}

void mobileSimu(void) {
  static Uint32 t0 = 0;
  int collision_bas = 0;
  Uint32 t = SDL_GetTicks();
  double dt = (t - t0) / 1000.0;
  t0 = t;
  _m.x += _m.vx * dt;
  _m.y += _m.vy * dt;
  if((_m.x + _m.r >= gl4dpGetWidth() && _m.vx > 0) || 
     (_m.x - _m.r < 0 && _m.vx < 0)) {
    _m.vx = -_m.vx;
    collision();
  }
  if((_m.y + _m.r >= gl4dpGetHeight() && _m.vy > 0) || 
     (_m.y - _m.r < 0 && _m.vy < 0 && (collision_bas = 1))) {
    _m.vy = -_m.vy;
    collision();
  }
  if(!collision_bas)
    _m.vy += Gy * dt;
}

void mobileDraw(void) {
  gl4dpFilledCircle(_m.x, _m.y, _m.r);
}
