/*!\file moteur.h 
 *
 * \brief structures de données et protos de fonctions externes
 * (primitives.c transformations.c et scene.c (pas encore créé)) pour
 * réaliser un moteur de rendu par rastérisation.
 *
 * \author Farès BELHADJ, amsi@up8.edu
 * \date November 25, 2020. 
*/

#ifndef MOTEUR_H_SEEN
#  define MOTEUR_H_SEEN

#  include <GL4D/gl4dp.h>
#  include <GL4D/gl4dm.h>

#include <float.h>
#define EPSILON ((double)FLT_EPSILON)

#  ifdef __cplusplus
extern "C" {
#  endif
  
  typedef enum pstate_t pstate_t;
  typedef enum soptions_t soptions_t;
  typedef struct vec4 vec4;
  typedef struct vec3 vec3;
  typedef struct vec2 vec2;
  typedef struct vertex_t vertex_t;
  typedef struct triangle_t triangle_t;
  typedef struct surface_t surface_t;

  /*!\brief états pour les sommets ou les triangles */
  enum pstate_t {
		 PS_NONE = 0,
		 PS_TOTALLY_OUT = 1,
		 PS_PARTIALLY_OUT = 2,
		 PS_CULL = 4, /* si en BACKFACE et que
				 SO_CULL_BACKFACES est actif */
		 PS_TOO_FAR = 8,
		 PS_OUT_LEFT = 16,
		 PS_OUT_RIGHT = 32,
		 PS_OUT_BOTTOM = 64,
		 PS_OUT_TOP = 128,
		 PS_OUT_NEAR = 256,
		 PS_OUT_FAR = 512
  };

  /*!\brief options pour les surfaces */
  enum soptions_t {
		   SO_NONE = 0, /* la surface n'a pas de rendu
				   "couleur" */
		   SO_USE_TEXTURE = 1, /* utiliser la texture pour
					  colorer (multiplication si
					  SO_USE_COLOR est actif) */
		   SO_USE_COLOR = 2, /* utiliser la couleur de la
					surface ou des sommets pour
					colorer (multiplication si
					SO_USE_TEXTURE est actif) */
		   SO_COLOR_MATERIAL = 4, /* utiliser la couleur aux
					     sommets si actif
					     (nécessite aussi
					     l'activation de
					     SO_USE_COLOR) */
		   SO_CULL_BACKFACES = 8, /* active le fait de cacher
					     les faces arrières */
		   SO_USE_LIGHTING = 16, /* active le calcul d'ombre
					    propre (Gouraud sur
					    diffus) */
		   SO_DEFAULT = SO_CULL_BACKFACES | SO_USE_COLOR /* comportement
								    par
								    défaut */
  };

  struct vec4 {
    float x /* r */, y/* g */, z /* b */, w /* a */;
  };

  struct vec2 {
    float x /* s */, y /* t */;
  };

  struct vec3 {
    float x /* r */, y/* g */, z/* b */;
  };

  /*!\brief le sommet et l'ensemble de ses attributs */
  struct vertex_t {
    vec4 position;
    vec4 color0;
    /* début des données à partir desquelles on peut interpoler en masse */
    vec2 texCoord; /* coordonnée de texture */
    vec4 icolor;   /* couleur à interpoler */
    float li;      /* intensité de lumière (lambertien) */
    float zmod;    /* z après modelview, sert à corriger
		      l'interpolation par rapport à une projection en
		      perspective */
    float z;       /* ce z représente la depth */
    /* fin des données à partir desquelles on peut interpoler */
    vec3 normal; /* interpolez les normales si vous implémentez Phong */
    int x, y;
    enum pstate_t state;
  };

  /*!\brief le triangle */
  struct triangle_t {
    vertex_t v[3];
    vec3 normal;  
    enum pstate_t state;
  };

  /*!\brief la surface englobe plusieurs triangles et des options
   * telles que le type de rendu, la couleur diffuse ou la texture.
   */
  struct surface_t {
    int n;
    triangle_t * t;
    GLuint texId;
    vec4 dcolor; /* couleur diffuse, ajoutez une couleur ambiante et
		    spéculaire si vous souhaitez compléter le
		    modèle */
    soptions_t options; /* paramétrage du rendu de la surface */
    void (*interpolatefunc)(vertex_t *, vertex_t *, vertex_t *, float, float);
    void (*shadingfunc)(surface_t *, GLuint *, vertex_t *);
  };
  
  /* dans primitives.c */
  extern void transform_n_raster(surface_t * s, float * mvMat, float * projMat);
  extern void clearDepth(void);
  extern void setTexture(GLuint screen);
  extern void updatesfuncs(surface_t * s);
  extern void drawLine(int x0, int y0, int x1, int y1, GLuint color);

  /* dans tranformations.c */
  extern vertex_t vtransform(surface_t * s, vertex_t v, float * mvMat, float * timvMat, float * projMat, float * viewport);
  extern void     stransform(surface_t * s, float * mvMat, float * projMat, float * viewport);
  extern void     multMatrix(float * res, float * m);
  extern void     translate(float * m, float tx, float ty, float tz);
  extern void     rotate(float * m, float angle, float x, float y, float z);
  extern void     scale(float * m, float sx, float sy, float sz);
  extern void     lookAt(float * m, float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ);
  
  /* dans scene.c */
  extern void        tnormal(triangle_t * t);
  extern void        snormals(surface_t * s);
  extern void        tnormals2vertices(surface_t * s);
  extern void        setTexId(surface_t * s, GLuint texId);
  extern void        setDiffuseColor(surface_t * s, vec4 dcolor);
  extern void        enableSurfaceOption(surface_t * s, soptions_t option);
  extern void        disableSurfaceOption(surface_t * s, soptions_t option);
  extern surface_t * newSurface(triangle_t * t, int n, int duplicateTriangles, int hasNormals);
  extern void        freeSurface(surface_t * s);
  extern GLuint      getTexFromBMP(const char * filename);

  /* dans geometry.c */
  extern surface_t * mkQuad(void);  
  extern surface_t * mkCube(void);
  extern surface_t * mkSphere(int longitudes, int latitudes);
#  ifdef __cplusplus
}
#  endif


#endif
