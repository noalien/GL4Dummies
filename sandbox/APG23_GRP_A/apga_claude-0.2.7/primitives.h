#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <stdint.h>
#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4du.h>

#include "claude_math.h"

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct vertex_t vertex_t;
  typedef struct triangle_t triangle_t;
  typedef struct surface_t surface_t;
  typedef struct texture_t texture_t;

  /*!\brief le sommet et l'ensemble de ses attributs */
  struct vertex_t {
    float x, y, z, w; /* coordonnée dans l’espace objet (3D/4D) */
    float r, g, b, a; /* couleur du sommet */
    float s, t; /* coordonnée de texture du sommet */
    float nx, ny, nz, nw; /* vecteur normal */
    float d; /* depth (le z dans l'espace projeté) */
    float il;      /* intensité de lumière (lambertien) */
    float zmod;    /* z après modelview, sert à corriger
		      l'interpolation par rapport à une projection en
		      perspective */
    int xe, ye; /* coordonnée dans l’espace écran */
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
  /*!\brief stocke une texture et ses infos */
  struct texture_t {
    int w, h;
    uint32_t * rgba;
  };

  extern void        fill_triangle(const triangle_t * t);
  extern texture_t * gen_texture(const char * file);
  extern void        apply_texture(texture_t * t);
  extern void        free_texture(texture_t * t);

  /* dans geometry.c */
  extern surface_t * gen_quad(void);
  extern surface_t * gen_cube(void);
  extern surface_t * gen_sphere(int longitudes, int latitudes);
  extern void        free_surface(surface_t * s);
  extern surface_t * duplicate_surface(surface_t * s);
  extern void        colorize_surface(surface_t * s, vec4 color);
  
#ifdef __cplusplus
}
#endif

  
#endif /* du #ifndef PRIMITIVES_H */
