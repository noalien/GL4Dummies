#include "claude.h"
#include "primitives.h"

/* GROS TODO, ajouter un clipping, ne serait-ce que non-optimal */

static inline void _ftransform(const mat4 M, const mat4 V, const mat4 P, const float * p, float * pp, float * zmod);

void claude_apply_transforms(const mat4 M, const mat4 V, const mat4 P, const surface_t * s, surface_t * sp) {
  /* s' doit pouvoir contenir le même nombre de triangles que s */
  assert(s->n == sp->n);
  /* d'abord on copie de l'un à l'autre */
  /* les triangles */
  memcpy(sp->triangles, s->triangles, s->n * sizeof *s->triangles);
  for(int i = 0; i < s->n; ++i) {
    for(int j = 0; j < 3; ++j) {
      _ftransform(M, V, P, &(s->triangles[i].v[j].x), &(sp->triangles[i].v[j].x), &(sp->triangles[i].v[j].zmod));
    }
  }
}

void claude_draw(surface_t * sp, const int * viewport) {
  for(int i = 0; i < sp->n; ++i) {
    for(int j = 0; j < 3; ++j) {
      sp->triangles[i].v[j].xe = viewport[0] + (int)(((sp->triangles[i].v[j].x + 1.0f) / 2.0f) * (viewport[2] - CL_EPSILON));
      sp->triangles[i].v[j].ye = viewport[1] + (int)(((sp->triangles[i].v[j].y + 1.0f) / 2.0f) * (viewport[3] - CL_EPSILON));
      sp->triangles[i].v[j].d  = pow((-sp->triangles[i].v[j].z + 1.0f) * 0.5f, 0.5f); /* la depth est une inversion du z, rendu non linéaire */
    }
  }
  for(int i = 0; i < sp->n; ++i)
    fill_triangle(&(sp->triangles[i]));
}

void _ftransform(const mat4 M, const mat4 V, const mat4 P, const float * p, float * pp, float * zmod) {
  mat4 mv;
  vec4 vmod;
  mat4mult(mv, V, M);
  mat4vec4mult(vmod,   mv, p);
  *zmod = 1.0f; /* TODO 1 au lieu de vmod[2]; pour montrer le problème avant de corriger */
  mat4vec4mult(pp,   P, vmod);
  pp[0] /= pp[3];
  pp[1] /= pp[3];
  pp[2] /= pp[3];
  pp[3] = 1.0f;
}

