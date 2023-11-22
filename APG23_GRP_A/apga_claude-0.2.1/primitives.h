#ifndef PRIMITIVES_H
#define PRIMITIVES_H


#ifdef __cplusplus
extern "C" {
#endif

  typedef struct vertex_t vertex_t;
  typedef struct triangle_t triangle_t;
  typedef struct surface_t surface_t;
  /*!\brief le sommet et l'ensemble de ses attributs */
  struct vertex_t {
    float x, y, z, w; /* coordonnée dans l’espace objet (3D/4D) */
    float r, g, b, a; /* couleur du sommet */
    float s, t; /* coordonnée de texture du sommet */
    float nx, ny, nz; /* vecteur normal */
    float depth;
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

  extern void fill_triangle(const triangle_t * t);
  extern void apply_texture(const char * file);
  
#ifdef __cplusplus
}
#endif

  
#endif /* du #ifndef PRIMITIVES_H */
