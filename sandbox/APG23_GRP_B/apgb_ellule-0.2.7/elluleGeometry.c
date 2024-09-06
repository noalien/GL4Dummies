#include "ellule.h"
#include "elluleRaster.h"

static inline surface_t * _new_surface(int);

surface_t * elQuad(void) {  
  static triangle_t t[2] = {
    { /* Triangle 0 */
      {
	{ /* vertex 0 */
	  {-1.0f, -1.0f, 0.0f, 1.0f}, /* x, y, z, w */
	  {1.0f, 0.0f, 0.0f, 1.0f}, /* r, g, b, a */
	  {0.0f, 0.0f, 1.0f}, /* nx, ny, nz */
	  {0.0f, 0.0f} /* s, t */
	},
	{ /* vertex 1 */
	  {1.0f, -1.0f, 0.0f, 1.0f}, /* x, y, z, w */
	  {0.0f, 1.0f, 0.0f, 1.0f}, /* r, g, b, a */
	  {0.0f, 0.0f, 1.0f}, /* nx, ny, nz */
	  {1.0f, 0.0f} /* s, t */
	},
	{ /* vertex 2 */
	  {-1.0f, 1.0f, 0.0f, 1.0f}, /* x, y, z, w */
	  {0.0f, 0.0f, 1.0f, 1.0f}, /* r, g, b, a */
	  {0.0f, 0.0f, 1.0f}, /* nx, ny, nz */
	  {0.0f, 1.0f} /* s, t */
	}
      }
    },
    {
      /* Triangle 1 */
      {
	{ /* vertex 2 */
	  {-1.0f, 1.0f, 0.0f, 1.0f}, /* x, y, z, w */
	  {0.0f, 0.0f, 1.0f, 1.0f}, /* r, g, b, a */
	  {0.0f, 0.0f, 1.0f}, /* nx, ny, nz */
	  {0.0f, 1.0f} /* s, t */
	},
	{ /* vertex 1 */
	  {1.0f, -1.0f, 0.0f, 1.0f}, /* x, y, z, w */
	  {0.0f, 1.0f, 0.0f, 1.0f}, /* r, g, b, a */
	  {0.0f, 0.0f, 1.0f}, /* nx, ny, nz */
	  {1.0f, 0.0f} /* s, t */
	},
	{ /* vertex 3 */
	  {1.0f, 1.0f, 0.0f, 1.0f}, /* x, y, z, w */
	  {1.0f, 0.0f, 0.0f, 1.0f}, /* r, g, b, a */
	  {0.0f, 0.0f, 1.0f}, /* nx, ny, nz */
	  {1.0f, 1.0f} /* s, t */
	}
      }
    }
  };
  surface_t * s = _new_surface(2);
  memcpy(s->triangles, t, sizeof t);
  return s;
}

surface_t * elCube(void) {
  const float
    data[] = {
	      /* front */
	      -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	      1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
	      -1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
	      1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
	      /* back */
	      1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
	      -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
	      1.0f,  1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
	      -1.0f,  1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
	      /* right */
	      1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
	      1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	      1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	      1.0f,  1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
	      /* left */
	      -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
	      -1.0f, -1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	      -1.0f,  1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	      -1.0f,  1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
	      /* top */
	      -1.0f, 1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
	      1.0f, 1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
	      -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	      1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
	      /* bottom */
	      -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
	      1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	      -1.0f, -1.0f,  1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
	      1.0f, -1.0f,  1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f
  };
  const int order[] = { 0, 1, 2, 2, 1, 3 };
  surface_t * s = _new_surface(12);
  int i, j, k, o;
  vec4 blanc = { 1.0f, 1.0f, 1.0f, 1.0f };
  for(i = 0, o = 0; i < 12; ++i)
    for(j = 0; j < 3; ++j, ++o) {
      k = 8 * (order[o % 6] + 4 * (i / 2));
      memcpy(s->triangles[i].v[j].position, &(data[k]), 3 * sizeof *data);
      s->triangles[i].v[j].position[3] = 1.0f;
      memcpy(s->triangles[i].v[j].normal,   &(data[k + 3]), 3 * sizeof *data);
      memcpy(s->triangles[i].v[j].texCoord, &(data[k + 6]), 2 * sizeof *data);
      memcpy(s->triangles[i].v[j].color, blanc, sizeof blanc);
    }
  return s;  
}

surface_t * elSphere(int longitudes, int latitudes) {
  surface_t * s;
  float r0, r1, pas_phi = 2.0f * M_PI / (longitudes - 1), pas_theta = M_PI / (latitudes - 1);
  vertex_t v[4];
  assert(longitudes > 2 && latitudes > 2);
  /* tous les w à 1.0 */
  v[0].position[3] = v[1].position[3] = v[2].position[3] = v[3].position[3] = 1.0f;
  /* les couleurs, tout est blanc */
  v[0].color[0] = v[0].color[1] = v[0].color[2] = v[0].color[3] = 1.0f;
  v[1].color[0] = v[1].color[1] = v[1].color[2] = v[1].color[3] = 1.0f;
  v[2].color[0] = v[2].color[1] = v[2].color[2] = v[2].color[3] = 1.0f;
  v[3].color[0] = v[3].color[1] = v[3].color[2] = v[3].color[3] = 1.0f;
  /* construire les paires de triangles en calculant 4 vertices pour
   * chaque couple (i, j) -- on utilise les "next", à droite, en haut
   * et en haut à droite */
  s = _new_surface(2 * (longitudes - 1) * (latitudes - 1));
  for(int i = 0, k = 0; i < latitudes - 1; ++i) {
    r0 = cos(i * pas_theta - M_PI / 2.0f);
    r1 = cos((i + 1) * pas_theta - M_PI / 2.0f);
    v[1].position[1] = v[0].position[1] = sin(i * pas_theta - M_PI / 2.0f);
    v[1].texCoord[1] = v[0].texCoord[1] = i / (latitudes - 1.0f);
    v[3].position[1] = v[2].position[1] = sin((i + 1) * pas_theta - M_PI / 2.0f);
    v[3].texCoord[1] = v[2].texCoord[1] = (i + 1) / (latitudes - 1.0f);
    for(int j = 0; j < longitudes - 1; ++j) {
      v[0].position[0] =  r0 * cos(j * pas_phi);
      v[0].position[2] = -r0 * sin(j * pas_phi);
      v[1].position[0] =  r0 * cos((j + 1) * pas_phi);
      v[1].position[2] = -r0 * sin((j + 1) * pas_phi);
      v[2].position[0] =  r1 * cos((j + 1) * pas_phi);
      v[2].position[2] = -r1 * sin((j + 1) * pas_phi);
      v[3].position[0] =  r1 * cos(j * pas_phi);
      v[3].position[2] = -r1 * sin(j * pas_phi);
      v[3].texCoord[0] = v[0].texCoord[0] = j / (longitudes - 1.0f);
      v[2].texCoord[0] = v[1].texCoord[0] = (j + 1) / (longitudes - 1.0f);

      /* les normales sont les coordonnées du sommet */
      v[0].normal[0] = v[0].position[0]; v[0].normal[1] = v[0].position[1]; v[0].normal[2] = v[0].position[2]; 
      v[1].normal[0] = v[1].position[0]; v[1].normal[1] = v[1].position[1]; v[1].normal[2] = v[1].position[2]; 
      v[2].normal[0] = v[2].position[0]; v[2].normal[1] = v[2].position[1]; v[2].normal[2] = v[2].position[2]; 
      v[3].normal[0] = v[3].position[0]; v[3].normal[1] = v[3].position[1]; v[3].normal[2] = v[3].position[2];
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

void elFreeSurface(surface_t * s) {
  free(s->triangles);
  free(s);
}

void elSetTexture(surface_t * s, texture_t * t) {
  s->tex = t;
}

void elSetColor(surface_t * s, const vec4 color) {
  for(int i = 0; i < s->n; ++i)
    for(int j = 0; j < 3; ++j)
      memcpy(s->triangles[i].v[j].color, color, sizeof s->triangles[i].v[j].color);
}


surface_t * _new_surface(int nb_triangles) {
  surface_t * s = malloc(sizeof *s);
  assert(s);
  s->n = nb_triangles;
  s->triangles = malloc(s->n * sizeof *(s->triangles));
  s->tex = NULL;
  assert(s->triangles);
  return s;
}
