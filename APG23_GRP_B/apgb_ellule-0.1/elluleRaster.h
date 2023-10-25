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
  };
  
  /*!\brief le triangle */
  struct triangle_t {
    vertex_t v[3];
  };

  extern void elFillTriangle(triangle_t * t);

#ifdef __cplusplus
}
#endif
  
#endif
