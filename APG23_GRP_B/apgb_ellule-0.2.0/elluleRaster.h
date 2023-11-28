#ifndef ELLULE_RASTER_H
#define ELLULE_RASTER_H

#include "ellule.h"
#include "elluleMaths.h"

#ifdef __cplusplus
extern "C" {
#endif
  
  typedef struct vertex_t vertex_t;
  typedef struct triangle_t triangle_t;
  typedef struct surface_t surface_t;

  /*!\brief le sommet et l'ensemble de ses attributs */
  struct vertex_t {
    vec4 position;
    vec4 color;
    vec3 normal;
    vec2 texCoord;
    ivec2 iCoord; /* coordonnée dans l’espace écran */
    vec4 mvppos;
  };
  
  /*!\brief le triangle */
  struct triangle_t {
    vertex_t v[3];
  };

  /*!\brief la surface composée de n triangles et ayant des propriétés de matériau */
  struct surface_t {
    int n;
    triangle_t * triangles;
  };

  extern void elFillTriangle(triangle_t * t);
  extern void elUseTexture(const char * filename);

  /* elluleTransforms.c */
  extern void elSetViewport(int * vp);
  extern void elTransformations(surface_t * s, mat4 M, mat4 V, mat4 P);
  extern void elDraw(surface_t * s);
  

#ifdef __cplusplus
}
#endif
  
#endif
