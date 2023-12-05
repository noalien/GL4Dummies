#include "claude.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

static inline surface_t * _new_surface(int nb_triangles);

surface_t * gen_quad(void) {
  static triangle_t t[2] = {
    { /* Triangle 1 */
      {
	{ /* vertex 1 */
	  -1.0f, -1.0f, 0.0f, 1.0f, /* x, y, z, w */
	  1.0f, 1.0f, 1.0f, 1.0f, /* r, g, b, a */
	  0.0f, 0.0f, /* s, t */
	  0.0f, 0.0f, 1.0f /* nx, ny */
	},
	{ /* vertex 2 */
	  1.0f, -1.0f, 0.0f, 1.0f, /* x, y, z, w */
	  1.0f, 1.0f, 1.0f, 1.0f, /* r, g, b, a */
	  1.0f, 0.0f, /* s, t */
	  0.0f, 0.0f, 1.0f /* nx, ny */
	},
	{ /* vertex 3 */
	  -1.0f, 1.0f, 0.0f, 1.0f, /* x, y, z, w */
	  1.0f, 1.0f, 1.0f, 1.0f, /* r, g, b, a */
	  0.0f, 1.0f, /* s, t */
	  0.0f, 0.0f, 1.0f /* nx, ny */
	}
      }
    },
    { /* Triangle 2 */
      {
	{ /* vertex 1 */
	  -1.0f, 1.0f, 0.0f, 1.0f, /* x, y, z, w */
	  1.0f, 1.0f, 1.0f, 1.0f, /* r, g, b, a */
	  0.0f, 1.0f, /* s, t */
	  0.0f, 0.0f, 1.0f /* nx, ny */
	},
	{ /* vertex 2 */
	  1.0f, -1.0f, 0.0f, 1.0f, /* x, y, z, w */
	  1.0f, 1.0f, 1.0f, 1.0f, /* r, g, b, a */
	  1.0f, 0.0f, /* s, t */
	  0.0f, 0.0f, 1.0f /* nx, ny */
	},
	{ /* vertex 3 */
	  1.0f, 1.0f, 0.0f, 1.0f, /* x, y, z, w */
	  1.0f, 1.0f, 1.0f, 1.0f, /* r, g, b, a */
	  1.0f, 1.0f, /* s, t */
	  0.0f, 0.0f, 1.0f /* nx, ny */
	}
      }
    }
  };
  surface_t * s = _new_surface(2);
  memcpy(s->triangles, &t, sizeof t);
  return s;
}


/* je vais essayer d'utiliser le quad + transformations pour écrire le cube */
surface_t * gen_cube(void) {
  surface_t * q = gen_quad(), * qp = duplicate_surface(q);
  surface_t * s = _new_surface(12); /* 6 faces -> 12 triangles */
  mat4 id, mv;
  mat4identite(id);
  
  /* copie de la face avant */
  mat4identite(mv);
  translate(mv, 0.0f, 0.0f, 1.0f);
  claude_apply_transforms(mv, id, id, q, qp);
  memcpy(&(s->triangles[0]), qp->triangles, 2 * sizeof *(qp->triangles));
  /* copie de la face droite */
  mat4identite(mv);
  rotate(mv, 90, 0.0f, 1.0f, 0.0f);
  translate(mv, 0.0f, 0.0f, 1.0f);
  claude_apply_transforms(mv, id, id, q, qp);
  memcpy(&(s->triangles[2]), qp->triangles, 2 * sizeof *(qp->triangles));
  /* copie de la face arrière */
  mat4identite(mv);
  rotate(mv, 180, 0.0f, 1.0f, 0.0f);
  translate(mv, 0.0f, 0.0f, 1.0f);
  claude_apply_transforms(mv, id, id, q, qp);
  memcpy(&(s->triangles[4]), qp->triangles, 2 * sizeof *(qp->triangles));
  /* copie de la face gauche */
  mat4identite(mv);
  rotate(mv, 270 /* ou -90 */, 0.0f, 1.0f, 0.0f);
  translate(mv, 0.0f, 0.0f, 1.0f);
  claude_apply_transforms(mv, id, id, q, qp);
  memcpy(&(s->triangles[6]), qp->triangles, 2 * sizeof *(qp->triangles));
  /* copie de la face du haut */
  mat4identite(mv);
  rotate(mv, -90, 1.0f, 0.0f, 0.0f);
  translate(mv, 0.0f, 0.0f, 1.0f);
  claude_apply_transforms(mv, id, id, q, qp);
  memcpy(&(s->triangles[8]), qp->triangles, 2 * sizeof *(qp->triangles));
  /* copie de la face du bas */
  mat4identite(mv);
  rotate(mv, 90, 1.0f, 0.0f, 0.0f);
  translate(mv, 0.0f, 0.0f, 1.0f);
  claude_apply_transforms(mv, id, id, q, qp);
  memcpy(&(s->triangles[10]), qp->triangles, 2 * sizeof *(qp->triangles));

  free_surface(q);
  free_surface(qp);
  return s;  
}


surface_t * gen_sphere(int longitudes, int latitudes) {
  surface_t * s;
  float r0, r1, pas_phi = 2.0f * M_PI / (longitudes - 1), pas_theta = M_PI / (latitudes - 1);
  vertex_t v[4];
  assert(longitudes > 2 && latitudes > 2);
  /* tous les w à 1.0 */
  v[0].w = v[1].w = v[2].w = v[3].w = 1.0f;
  /* les couleurs, tout est blanc */
  v[0].r = v[0].g = v[0].b = v[0].a = 1.0f;
  v[1].r = v[1].g = v[1].b = v[1].a = 1.0f;
  v[2].r = v[2].g = v[2].b = v[2].a = 1.0f;
  v[3].r = v[3].g = v[3].b = v[3].a = 1.0f;
  /* construire les paires de triangles en calculant 4 vertices pour
   * chaque couple (i, j) -- on utilise les "next", à droite, en haut
   * et en haut à droite */
  s = _new_surface(2 * (longitudes - 1) * (latitudes - 1));
  for(int i = 0, k = 0; i < latitudes - 1; ++i) {
    r0 = cos(i * pas_theta - M_PI / 2.0f);
    r1 = cos((i + 1) * pas_theta - M_PI / 2.0f);
    v[1].y = v[0].y = sin(i * pas_theta - M_PI / 2.0f);
    v[1].t = v[0].t = i / (latitudes - 1.0f);
    v[3].y = v[2].y = sin((i + 1) * pas_theta - M_PI / 2.0f);
    v[3].t = v[2].t = (i + 1) / (latitudes - 1.0f);
    for(int j = 0; j < longitudes - 1; ++j) {
      v[0].x = r0  * cos(j * pas_phi);
      v[0].z = -r0 * sin(j * pas_phi);
      v[1].x = r0  * cos((j + 1) * pas_phi);
      v[1].z = -r0 * sin((j + 1) * pas_phi);
      v[2].x = r1  * cos((j + 1) * pas_phi);
      v[2].z = -r1 * sin((j + 1) * pas_phi);
      v[3].x = r1  * cos(j * pas_phi);
      v[3].z = -r1 * sin(j * pas_phi);
      v[3].s = v[0].s = j / (longitudes - 1.0f);
      v[2].s = v[1].s = (j + 1) / (longitudes - 1.0f);

      /* les normales sont les coordonnées du sommet */
      v[0].nx = v[0].x; v[0].ny = v[0].y; v[0].nz = v[0].z; 
      v[1].nx = v[1].x; v[1].ny = v[1].y; v[1].nz = v[1].z; 
      v[2].nx = v[2].x; v[2].ny = v[2].y; v[2].nz = v[2].z; 
      v[3].nx = v[3].x; v[3].ny = v[3].y; v[3].nz = v[3].z;
      /* copie */
      s->triangles[k].v[0]   = v[0];
      s->triangles[k].v[1]   = v[1];
      s->triangles[k++].v[2] = v[2];
      s->triangles[k].v[0]   = v[0];
      s->triangles[k].v[1]   = v[2];
      s->triangles[k++].v[2] = v[3];
    }
  }
  return s;    
}

void free_surface(surface_t * s) {
  free(s->triangles);
  free(s);
}

surface_t * duplicate_surface(surface_t * s) {
  surface_t * d = malloc(sizeof *d);
  assert(d);
  memcpy(d, s, sizeof *d);
  d->triangles = malloc(s->n * sizeof *(d->triangles));
  assert(d->triangles);
  memcpy(d->triangles, s->triangles, sizeof *d);
  return d;
}

surface_t * _new_surface(int nb_triangles) {
  surface_t * s = malloc(sizeof *s);
  assert(s);
  s->triangles = malloc(nb_triangles * sizeof *(s->triangles));
  assert(s->triangles);
  s->n = nb_triangles;
  return s;
}
