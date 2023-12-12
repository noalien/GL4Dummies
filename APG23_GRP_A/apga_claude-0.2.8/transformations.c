#include "claude.h"
#include "primitives.h"

/* GROS TODO, ajouter un clipping, ne serait-ce que non-optimal */

static inline void _ftransform(const mat4 M, const mat4 V, const mat4 P, const float * p, float * pp, float * zmod, float * normal, float * il);

void claude_apply_transforms(const mat4 M, const mat4 V, const mat4 P, const surface_t * s, surface_t * sp) {
  /* s' doit pouvoir contenir le même nombre de triangles que s */
  assert(s->n == sp->n);
  /* d'abord on copie de l'un à l'autre */
  /* les triangles */
  memcpy(sp->triangles, s->triangles, s->n * sizeof *s->triangles);
  for(int i = 0; i < s->n; ++i) {
    for(int j = 0; j < 3; ++j) {
      _ftransform(M, V, P, &(s->triangles[i].v[j].x), &(sp->triangles[i].v[j].x), &(sp->triangles[i].v[j].zmod), &(sp->triangles[i].v[j].nx), &(sp->triangles[i].v[j].il));
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
    /* TODO : un peu radical, mieux calculer le clipping et ne pas
       rejeter l'ensemble du triangle poiur un seul sommet en dehors
       du cube unitaire */
    if((sp->triangles[i].v[0].d < 0.0f || sp->triangles[i].v[1].d < 0.0f || sp->triangles[i].v[2].d < 0.0f) ||
       (sp->triangles[i].v[0].d > 1.0f || sp->triangles[i].v[1].d > 1.0f || sp->triangles[i].v[2].d > 1.0f) )
      continue;
    fill_triangle(&(sp->triangles[i]));
  }
}

void _ftransform(const mat4 M, const mat4 V, const mat4 P, const float * p, float * pp, float * zmod, float * normal, float * il) {
  /* TODO : calculs faits pour chaque sommet ... dommage ... on peut mieux faire */
  mat4 mv, timv;
  vec4 vmod;
  mat4mult(mv, V, M);

  memcpy(timv, mv, sizeof timv);
  mat4inverse(timv);
  mat4transpose(timv);
  
  mat4vec4mult(vmod,   mv, p);
  *zmod = vmod[2];
  mat4vec4mult(pp,   P, vmod);
  pp[0] /= pp[3];
  pp[1] /= pp[3];
  pp[2] /= pp[3];
  pp[3] = 1.0f;


  /* la lumière est positionnelle et fixe dans la scène.
     TODO : la rendre modifiable, voire aussi pouvoir la placer par
     rapport aux objets (elle subirait la matrice modèle). */
  const vec4 lp = { 0.0f, 0.0f, 1.0f, 1.0f };
  vec3 ld = {lp[0] - vmod[0], lp[1] - vmod[1], lp[2] - vmod[2]};
  vec4 res;
  normal[3] = 0.0f;
  mat4vec4mult(res, timv, normal);
  vec3normalize(res);
  normal[0] = res[0]; normal[1] = res[1]; normal[2] = res[2]; 
  vec3normalize(ld);
  *il = vec3dot(res, ld);
  *il = _min(_max(0.0f, *il), 1.0f) * 0.8f + 0.2f; /* 80% de diffus et 20% d'ambient */
  
}

