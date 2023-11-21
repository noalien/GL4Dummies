#include "claude.h"
#include "primitives.h"

static inline void _ftransform(const mat4 M, const mat4 V, const mat4 P, const float * p, float * pp);

void claude_apply_transforms(const mat4 M, const mat4 V, const mat4 P, const surface_t * s, surface_t * sp) {
  /* s' doit pouvoir contenir le même nombre de triangles que s */
  assert(s->n == sp->n);
  /* d'abord on copie de l'un à l'autre */
  /* les triangles */
  memcpy(sp->triangles, s->triangles, s->n * sizeof *s->triangles);
  for(int i = 0; i < s->n; ++i) {
    for(int j = 0; j < 3; ++j)
      _ftransform(M, V, P, &(s->triangles[i].v[j].x), &(sp->triangles[i].v[j].x));
  }
}

void claude_draw(surface_t * sp, const int * viewport) {
  for(int i = 0; i < sp->n; ++i) {
    for(int j = 0; j < 3; ++j) {
      sp->triangles[i].v[j].xe = viewport[0] + (int)(((sp->triangles[i].v[j].x + 1.0f) / 2.0f) * (viewport[2] - CL_EPSILON));
      sp->triangles[i].v[j].ye = viewport[1] + (int)(((sp->triangles[i].v[j].y + 1.0f) / 2.0f) * (viewport[3] - CL_EPSILON));
    }
  }
  for(int i = 0; i < sp->n; ++i)
    fill_triangle(&(sp->triangles[i]));
}

void _ftransform(const mat4 M, const mat4 V, const mat4 P, const float * p, float * pp) {
  vec4 tmp;
  mat4vec4mult(pp,  M, p);
  mat4vec4mult(tmp, V, pp);
  mat4vec4mult(pp,  P, tmp);
}

