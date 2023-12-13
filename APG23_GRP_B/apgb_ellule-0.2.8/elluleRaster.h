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
  typedef struct texture_t texture_t;

  /*!\brief le sommet et l'ensemble de ses attributs */
  struct vertex_t {
    vec4 position;
    vec4 color;
    vec3 normal;
    vec2 texCoord;
    float il; /* incidence de lumière calculée par produit scalaire */
    ivec2 iCoord; /* coordonnée dans l’espace écran */
    vec4 mvppos; /* position après model, view puis projection */
    vec4 mvpos; /* position après model et view */
  };
  
  /*!\brief le triangle */
  struct triangle_t {
    vertex_t v[3];
  };

  /*!\brief la surface composée de n triangles et ayant des propriétés de matériau */
  struct surface_t {
    int n;
    triangle_t * triangles;
    texture_t * tex;
  };

  /*!\brief stocke une texture et ses infos */
  struct texture_t {
    int w, h;
    uint32_t * rgba;
  };

  enum rendering_opt_t {
    EL_ALPHA = 1,
    EL_BACKFACE_CULLING = 2,
    EL_COLOR = 4,
    EL_SHADING = 8,
    EL_TEXTURE = 16
  };

  extern void        elFillTriangle(surface_t * s, triangle_t * t);
  extern texture_t * elGenTexture(const char * filename);
  extern void        elFreeTexture(texture_t * t);
  extern void        elEnable(enum rendering_opt_t opt);
  extern void        elDisable(enum rendering_opt_t opt);
  extern int         elIsEnabled(enum rendering_opt_t opt);

  /* elluleTransforms.c */
  extern void elSetViewport(int * vp);
  extern void elTransformations(surface_t * s, mat4 M, mat4 V, mat4 P);
  extern void elDraw(surface_t * s);
  
  /* elluleGeometry.c */

  extern surface_t * elQuad(void);
  extern surface_t * elCube(void);
  extern surface_t * elSphere(int longitudes, int latitudes);
  extern void        elFreeSurface(surface_t * s);
  extern void        elSetTexture(surface_t * s, texture_t * t);
  extern void        elSetColor(surface_t * s, const vec4 color);
  
#ifdef __cplusplus
}
#endif
  
#endif
