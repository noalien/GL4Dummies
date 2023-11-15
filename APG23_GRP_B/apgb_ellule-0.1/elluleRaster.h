#ifndef ELLULE_RASTER_H
#define ELLULE_RASTER_H

#include "ellule.h"

#ifdef __cplusplus
extern "C" {
#endif
  
  typedef struct vertex_t vertex_t;
  typedef struct triangle_t triangle_t;

  /*!\brief le sommet et l'ensemble de ses attributs */
  struct vertex_t {
    int x, y; /* coordonnée dans l’espace écran */
    float r, g, b, a;
    float s, t;
  };
  
  /*!\brief le triangle */
  struct triangle_t {
    vertex_t v[3];
  };

  extern void elFillTriangle(triangle_t * t);
  extern void elUseTexture(const char * filename);

#ifdef __cplusplus
}
#endif
  
#endif
