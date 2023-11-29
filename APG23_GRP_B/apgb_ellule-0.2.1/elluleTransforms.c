#include "ellule.h"
#include "elluleRaster.h"
#include "elluleMaths.h"

int _viewport[4] = { 0, 0, 1, 1 };

void elSetViewport(int * vp) {
  memcpy(_viewport, vp, sizeof _viewport);
}

void elTransformations(surface_t * s, mat4 M, mat4 V, mat4 P) {
  mat4 mv, mvp;
  _mat4mult(mv, V, M);
  _mat4mult(mvp, P, mv);

  for(int i = 0; i < s->n; ++i) {
    for(int j = 0; j < 3; ++j) {
      _mat4vec4mult(s->triangles[i].v[j].mvppos,   mvp, s->triangles[i].v[j].position);
      _mat4vec4mult(s->triangles[i].v[j].mvpos,     mv, s->triangles[i].v[j].position);
      s->triangles[i].v[j].mvppos[0] /= s->triangles[i].v[j].mvppos[3];
      s->triangles[i].v[j].mvppos[1] /= s->triangles[i].v[j].mvppos[3];
      s->triangles[i].v[j].mvppos[2] /= s->triangles[i].v[j].mvppos[3];
      s->triangles[i].v[j].mvppos[3] = 1.0f;
    }
  }
}

void elDraw(surface_t * s) {
  for(int i = 0; i < s->n; ++i) {
    for(int j = 0; j < 3; ++j) {
      s->triangles[i].v[j].iCoord[0] = _viewport[0] + (int)(((s->triangles[i].v[j].mvppos[0] + 1.0f) / 2.0f) * (_viewport[2] - EL_EPSILON));
      s->triangles[i].v[j].iCoord[1] = _viewport[1] + (int)(((s->triangles[i].v[j].mvppos[1] + 1.0f) / 2.0f) * (_viewport[3] - EL_EPSILON));
    }
  }
  for(int i = 0; i < s->n; ++i)
    elFillTriangle(&(s->triangles[i]));
}
