/*!\file gl4dg.c
 *
 * \brief The GL4Dummies Geometry
 *
 * \author Fares BELHADJ amsi@ai.univ-paris8.fr
 * \date February 22, 2016
 */

#if defined(_MSC_VER)
#  define _USE_MATH_DEFINES
#endif
#include <math.h>
#include "linked_list.h"
#include "gl4dg.h"
#include "gl4dm.h"
#include <stdlib.h>
#include <assert.h>

/*!\brief permet de sélectionner une topologie à utiliser selon le
 * niveau d'optimisation géométrique choisie. */
#define SELECT_GEOMETRY_OPTIMIZATION(index, geom, w, h) do {		\
    switch(_geometry_optimization_level) {				\
    case 0:								\
      (index) = mkRegularGridTriangleIndices((w), (h));			\
      (geom)->index_mode = GL_TRIANGLES;				\
      (geom)->index_row_count = 6 * ((w) - 1) * ((h) - 1);		\
      (geom)->index_nb_rows = 1;					\
      break;								\
    case 1:								\
      (index) = mkRegularGridStripsIndices((w), (h));			\
      (geom)->index_mode = GL_TRIANGLE_STRIP;				\
      (geom)->index_row_count = 2 * (w);				\
      (geom)->index_nb_rows = ((h) - 1);				\
      break;								\
    default:								\
      index = mkRegularGridStripIndices((w), (h));			\
      (geom)->index_mode = GL_TRIANGLE_STRIP;				\
      (geom)->index_row_count = 2 * (w) * ((h) - 1);			\
      (geom)->index_nb_rows = 1;					\
      break;								\
    }									\
  } while(0)

/*!\brief dessine un element-array selon la topologie de l'objet. */
#define DRAW_WITH_GEOMETRY_OPTIMIZATION(geom) do {			\
    int i, d;								\
    for(i = 0, d = 0; i <  (geom)->index_nb_rows; i++) {		\
      glDrawElements((geom)->index_mode, (geom)->index_row_count, GL4D_VAO_INDEX, (const GLvoid *)(intptr_t)d); \
      d += (geom)->index_row_count * sizeof(GL4Dvaoindex);		\
    }									\
  } while(0)

typedef struct geom_t geom_t;
typedef struct gsphere_t gsphere_t;
typedef struct gstatic_t gstatic_t;
typedef struct gcone_t gcone_t;
typedef struct gcylinder_t gcylinder_t;
typedef struct gdisk_t gdisk_t;
typedef struct gtorus_t gtorus_t;
typedef struct ggrid2d_t ggrid2d_t;
typedef enum   geom_e geom_e;

enum geom_e {
  GE_NONE = 0,
  GE_SPHERE,
  GE_QUAD,
  GE_CUBE,
  GE_CONE,
  GE_FAN_CONE,
  GE_CYLINDER,
  GE_DISK,
  GE_TORUS,
  GE_GRID2D,
  GE_TEAPOT
};

struct geom_t {
  GLuint id, vao;
  geom_e type;
  void * geom;
};

struct gsphere_t {
  GLuint buffers[2];
  GLuint slices, stacks;
  GLenum index_mode;
  GLsizei index_row_count;
  GLsizei index_nb_rows;
};

struct gstatic_t {
  GLuint buffer;
};

struct gcone_t {
  GLuint buffer;
  GLuint slices;
  GLboolean base;
};

struct gcylinder_t {
  GLuint buffer;
  GLuint slices;
  GLboolean base;
};

struct gdisk_t {
  GLuint buffer;
  GLuint slices;
};

struct gtorus_t {
  GLuint buffers[2];
  GLuint slices, stacks;
  GLdouble radius;
  GLenum index_mode;
  GLsizei index_row_count;
  GLsizei index_nb_rows;
};

struct ggrid2d_t {
  GLuint buffers[2];
  GLuint width, height;
  GLenum index_mode;
  GLsizei index_row_count;
  GLsizei index_nb_rows;
};

static geom_t * _garray = NULL;
static GLint _garray_size = 256;
static linked_list_t * _glist = NULL;
static int _hasInit = 0;
static GLuint _geometry_optimization_level = 1;

static void            freeGeom(void * data);
static GLuint          genId(void);
static GLuint          mkStaticf(geom_e type);
static GLfloat       * mkSphereVerticesf(GLuint slices, GLuint stacks);
static GL4Dvaoindex  * mkRegularGridTriangleIndices(GLuint width, GLuint height);
static GL4Dvaoindex  * mkRegularGridStripsIndices(GLuint width, GLuint height);
static GL4Dvaoindex  * mkRegularGridStripIndices(GLuint width, GLuint height);
static GLfloat       * mkConeVerticesf(GLuint slices, GLboolean base);
static GLfloat       * mkFanConeVerticesf(GLuint slices, GLboolean base);
static GLfloat       * mkCylinderVerticesf(GLuint slices, GLboolean base);
static GLfloat       * mkDiskVerticesf(GLuint slices);
static GLfloat       * mkTorusVerticesf(GLuint slices, GLuint stacks, GLfloat radius);
static GLfloat       * mkGrid2dVerticesf(GLuint width, GLuint height, GLfloat * heightmap);
static void            mkGrid2dNormalsf(GLuint width, GLuint height, GLfloat * data);
static inline void     triangleNormalf(GLfloat * out, GLfloat * p0, GLfloat * p1, GLfloat * p2);

void gl4dgInit(void) {
  int i;
  if(_hasInit) return;
  _glist = llNew();
  _garray = calloc(_garray_size, sizeof *_garray);
  assert(_garray);
  for(i = _garray_size - 1 ; i >= 0; i--) {
    _garray[i].id = i;
    llPush(_glist, &_garray[i]);
  }
  _hasInit = 1;
}

void gl4dgClean(void) {
  if(_glist) {
    llFree(_glist, NULL);
    _glist = NULL;
  }
  if(_garray) {
    int i;
    for(i = 0; i < _garray_size; i++)
      if(_garray[i].vao || _garray[i].geom)
	freeGeom(&_garray[i]);
    free(_garray);
    _garray = NULL;
  }
  _garray_size = 256;
  _hasInit = 0;
}

void gl4dgSetGeometryOptimizationLevel(GLuint level) {
  _geometry_optimization_level = level;
}

GLuint gl4dgGetVAO(GLuint id) {
  return _garray[--id].vao;
}

GLuint gl4dgGenSpheref(GLuint slices, GLuint stacks) {
  GLfloat * idata = NULL;
  GL4Dvaoindex * index = NULL;
  GLuint i = genId();
  gsphere_t * s = malloc(sizeof *s);
  assert(s);
  _garray[i].geom = s;
  _garray[i].type = GE_SPHERE;
  s->slices = slices; s->stacks = stacks;
  idata = mkSphereVerticesf(slices, stacks);
  SELECT_GEOMETRY_OPTIMIZATION(index, s, slices + 1, stacks + 1);
  glGenVertexArrays(1, &_garray[i].vao);
  glBindVertexArray(_garray[i].vao);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glGenBuffers(2, s->buffers);
  glBindBuffer(GL_ARRAY_BUFFER, s->buffers[0]);
  glBufferData(GL_ARRAY_BUFFER, 5 * (slices + 1) * (stacks + 1) * sizeof *idata, idata, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (5 * sizeof *idata), (const void *)0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (5 * sizeof *idata), (const void *)0);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, (5 * sizeof *idata), (const void *)(3 * sizeof *idata));
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s->buffers[1]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, s->index_row_count * s->index_nb_rows * sizeof *index, index, GL_STATIC_DRAW);
  free(idata);
  free(index);
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  return ++i;
}

GLuint gl4dgGenQuadf(void) {
  return mkStaticf(GE_QUAD);
}

GLuint gl4dgGenCubef(void) {
  return mkStaticf(GE_CUBE);
}

GLuint gl4dgGenConef(GLuint slices, GLboolean base) {
  GLfloat * data = NULL;
  GLuint i = genId();
  gcone_t * c = malloc(sizeof *c);
  assert(c);
  _garray[i].geom = c;
  _garray[i].type = GE_CONE;
  c->slices = slices; c->base = base;
  data = mkConeVerticesf(slices, base);
  glGenVertexArrays(1, &_garray[i].vao);
  glBindVertexArray(_garray[i].vao);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glGenBuffers(1, &(c->buffer));
  glBindBuffer(GL_ARRAY_BUFFER, c->buffer);
  glBufferData(GL_ARRAY_BUFFER, (16 * (slices + 1) + (base ? 8 : 0) * (slices + 2)) * sizeof *data, data, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (8 * sizeof *data), (const void *)0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (8 * sizeof *data), (const void *)(3 * sizeof *data));
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, (8 * sizeof *data), (const void *)(6 * sizeof *data));
  free(data);
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  return ++i;
}

GLuint gl4dgGenFanConef(GLuint slices, GLboolean base) {
  GLfloat * data = NULL;
  GLuint i = genId();
  gcone_t * c = malloc(sizeof *c);
  assert(c);
  _garray[i].geom = c;
  _garray[i].type = GE_FAN_CONE;
  c->slices = slices; c->base = base;
  data = mkFanConeVerticesf(slices, base);
  glGenVertexArrays(1, &_garray[i].vao);
  glBindVertexArray(_garray[i].vao);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glGenBuffers(1, &(c->buffer));
  glBindBuffer(GL_ARRAY_BUFFER, c->buffer);
  glBufferData(GL_ARRAY_BUFFER, (base ? 16 : 8) * (slices + 1 + /* le sommet ou le centre de la base */ 1) * sizeof *data, data, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (8 * sizeof *data), (const void *)0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (8 * sizeof *data), (const void *)(3 * sizeof *data));
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, (8 * sizeof *data), (const void *)(6 * sizeof *data));
  free(data);
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  return ++i;
}

GLuint gl4dgGenCylinderf(GLuint slices, GLboolean base) {
  GLfloat * data = NULL;
  GLuint i = genId();
  gcylinder_t * c = malloc(sizeof *c);
  assert(c);
  _garray[i].geom = c;
  _garray[i].type = GE_CYLINDER;
  c->slices = slices; c->base = base;
  data = mkCylinderVerticesf(slices, base);
  glGenVertexArrays(1, &_garray[i].vao);
  glBindVertexArray(_garray[i].vao);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glGenBuffers(1, &(c->buffer));
  glBindBuffer(GL_ARRAY_BUFFER, c->buffer);
  glBufferData(GL_ARRAY_BUFFER, (16 * (slices + 1) + (base ? 16 : 0) * (slices + 2)) * sizeof *data, data, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (8 * sizeof *data), (const void *)0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (8 * sizeof *data), (const void *)(3 * sizeof *data));
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, (8 * sizeof *data), (const void *)(6 * sizeof *data));
  free(data);
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  return ++i;
}

GLuint gl4dgGenDiskf(GLuint slices) {
  GLfloat * data = NULL;
  GLuint i = genId();
  gdisk_t * c = malloc(sizeof *c);
  assert(c);
  _garray[i].geom = c;
  _garray[i].type = GE_DISK;
  c->slices = slices;
  data = mkDiskVerticesf(slices);
  glGenVertexArrays(1, &_garray[i].vao);
  glBindVertexArray(_garray[i].vao);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glGenBuffers(1, &(c->buffer));
  glBindBuffer(GL_ARRAY_BUFFER, c->buffer);
  glBufferData(GL_ARRAY_BUFFER, 8 * (slices + 1 + /* le centre du disk */ 1) * sizeof *data, data, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (8 * sizeof *data), (const void *)0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (8 * sizeof *data), (const void *)(3 * sizeof *data));
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, (8 * sizeof *data), (const void *)(6 * sizeof *data));
  free(data);
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  return ++i;
}

GLuint gl4dgGenTorusf(GLuint slices, GLuint stacks, GLfloat radius) {
  GLfloat * idata = NULL;
  GL4Dvaoindex * index = NULL;
  GLuint i = genId();
  gtorus_t * s = malloc(sizeof *s);
  assert(s);
  _garray[i].geom = s;
  _garray[i].type = GE_TORUS;
  s->slices = slices; s->stacks = stacks;
  s->radius = radius;
  idata = mkTorusVerticesf(slices, stacks, radius);
  SELECT_GEOMETRY_OPTIMIZATION(index, s, slices + 1, stacks + 1);
  glGenVertexArrays(1, &_garray[i].vao);
  glBindVertexArray(_garray[i].vao);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glGenBuffers(2, s->buffers);
  glBindBuffer(GL_ARRAY_BUFFER, s->buffers[0]);
  glBufferData(GL_ARRAY_BUFFER, 8 * (slices + 1) * (stacks + 1) * sizeof *idata, idata, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (8 * sizeof *idata), (const void *)0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (8 * sizeof *idata), (const void *)(3 * sizeof *idata));
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, (8 * sizeof *idata), (const void *)(6 * sizeof *idata));
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s->buffers[1]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, s->index_row_count * s->index_nb_rows * sizeof *index, index, GL_STATIC_DRAW);
  free(idata);
  free(index);
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  return ++i;
}

GLuint gl4dgGenGrid2df(GLuint width, GLuint height) {
  return gl4dgGenGrid2dFromHeightMapf(width, height, NULL);
}

GLuint gl4dgGenGrid2dFromHeightMapf(GLuint width, GLuint height, GLfloat * heightmap) {
  GLfloat * idata = NULL;
  GL4Dvaoindex * index = NULL;
  GLuint i = genId();
  ggrid2d_t * s = malloc(sizeof *s);
  assert(s);
  _garray[i].geom = s;
  _garray[i].type = GE_GRID2D;
  s->width = width; s->height = height;
  idata = mkGrid2dVerticesf(width, height, heightmap);
  SELECT_GEOMETRY_OPTIMIZATION(index, s, width, height);
  glGenVertexArrays(1, &_garray[i].vao);
  glBindVertexArray(_garray[i].vao);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glGenBuffers(2, s->buffers);
  glBindBuffer(GL_ARRAY_BUFFER, s->buffers[0]);
  glBufferData(GL_ARRAY_BUFFER, 8 * width * height * sizeof *idata, idata, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (8 * sizeof *idata), (const void *)0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (8 * sizeof *idata), (const void *)(3 * sizeof *idata));
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, (8 * sizeof *idata), (const void *)(6 * sizeof *idata));
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s->buffers[1]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, s->index_row_count * s->index_nb_rows * sizeof *index, index, GL_STATIC_DRAW);
  free(idata);
  free(index);
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  return ++i;
}

void gl4dgDraw(GLuint id) {
  switch(_garray[--id].type) {
  case GE_SPHERE:
    glBindVertexArray(_garray[id].vao);
    DRAW_WITH_GEOMETRY_OPTIMIZATION((gsphere_t *)(_garray[id].geom));
    glBindVertexArray(0);
    break;
  case GE_TORUS:
    glBindVertexArray(_garray[id].vao);
    DRAW_WITH_GEOMETRY_OPTIMIZATION((gtorus_t *)(_garray[id].geom));
    glBindVertexArray(0);
    break;
  case GE_GRID2D:
    glBindVertexArray(_garray[id].vao);
    DRAW_WITH_GEOMETRY_OPTIMIZATION((ggrid2d_t *)(_garray[id].geom));
    glBindVertexArray(0);
    break;
  case GE_QUAD:
    glBindVertexArray(_garray[id].vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    break;
  case GE_CUBE:
    glBindVertexArray(_garray[id].vao);
    glDrawArrays(GL_TRIANGLE_STRIP,  0, 4);
    glDrawArrays(GL_TRIANGLE_STRIP,  4, 4);
    glDrawArrays(GL_TRIANGLE_STRIP,  8, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);
    glBindVertexArray(0);
    break;
  case GE_CONE:
  case GE_FAN_CONE:
    glBindVertexArray(_garray[id].vao);
    if(_garray[id].type == GE_CONE)
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 2 * (((gcone_t *)(_garray[id].geom))->slices + 1));
    else
      glDrawArrays(GL_TRIANGLE_FAN, 0, ((gcone_t *)(_garray[id].geom))->slices + 2);
    if(((gcone_t *)(_garray[id].geom))->base) {
      if(_garray[id].type == GE_CONE)
	glDrawArrays(GL_TRIANGLE_FAN, 2 * (((gcone_t *)(_garray[id].geom))->slices + 1), ((gcone_t *)(_garray[id].geom))->slices + 2);
      else
	glDrawArrays(GL_TRIANGLE_FAN, ((gcone_t *)(_garray[id].geom))->slices + 2, ((gcone_t *)(_garray[id].geom))->slices + 2);
    }
    glBindVertexArray(0);
    break;
  case GE_CYLINDER:
    glBindVertexArray(_garray[id].vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 2 * (((gcylinder_t *)(_garray[id].geom))->slices + 1));
    if(((gcylinder_t *)(_garray[id].geom))->base) {
      glDrawArrays(GL_TRIANGLE_FAN, 2 * (((gcylinder_t *)(_garray[id].geom))->slices + 1), ((gcylinder_t *)(_garray[id].geom))->slices + 2);
      glDrawArrays(GL_TRIANGLE_FAN, 2 * (((gcylinder_t *)(_garray[id].geom))->slices + 1) + ((gcylinder_t *)(_garray[id].geom))->slices + 2,
		   ((gcylinder_t *)(_garray[id].geom))->slices + 2);
    }
    glBindVertexArray(0);
    break;
  case GE_DISK:
    glBindVertexArray(_garray[id].vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, ((gdisk_t *)(_garray[id].geom))->slices + 2);
    glBindVertexArray(0);
    break;
  default:
    break;
  }
}

void gl4dgDelete(GLuint id) {
  --id;
  freeGeom(&_garray[id]);
  _garray[id].vao  = 0;
  _garray[id].type = 0;
  llPush(_glist, &_garray[id]);
}

static void freeGeom(void * data) {
  geom_t * geom = (geom_t *)data;
  switch(geom->type) {
  case GE_SPHERE:
    glDeleteVertexArrays(1, &(geom->vao));
    glDeleteBuffers(2, ((gsphere_t *)(geom->geom))->buffers);
    break;
  case GE_TORUS:
    glDeleteVertexArrays(1, &(geom->vao));
    glDeleteBuffers(2, ((gtorus_t *)(geom->geom))->buffers);
    break;
  case GE_GRID2D:
    glDeleteVertexArrays(1, &(geom->vao));
    glDeleteBuffers(2, ((ggrid2d_t *)(geom->geom))->buffers);
    break;
  case GE_QUAD:
  case GE_CUBE:
    glDeleteVertexArrays(1, &(geom->vao));
    glDeleteBuffers(1, &(((gstatic_t *)(geom->geom))->buffer));
    break;
  case GE_CONE:
  case GE_FAN_CONE:
    glDeleteVertexArrays(1, &(geom->vao));
    glDeleteBuffers(1, &(((gcone_t *)(geom->geom))->buffer));
    break;
  case GE_CYLINDER:
    glDeleteVertexArrays(1, &(geom->vao));
    glDeleteBuffers(1, &(((gcylinder_t *)(geom->geom))->buffer));
    break;
  case GE_DISK:
    glDeleteVertexArrays(1, &(geom->vao));
    glDeleteBuffers(1, &(((gdisk_t *)(geom->geom))->buffer));
    break;
  default:
    break;
  }
  if(geom->geom)
    free(geom->geom);
  geom->geom = NULL;
}

static GLuint genId(void) {
  int i;
  if(llEmpty(_glist)) {
    int s = _garray_size;
    _garray = realloc(_garray, (_garray_size *= 2) * sizeof *_garray);
    assert(_garray);
    for(i = _garray_size - 1 ; i >= s; i--) {
      _garray[i].id   = i;
      _garray[i].vao  = 0;
      _garray[i].type = 0;
      _garray[i].geom = NULL;
      llPush(_glist, &_garray[i]);
    }
  }
  return ((geom_t *)llPop(_glist))->id;
}

static GLuint mkStaticf(geom_e type) {
  static GLfloat quad_data[] = {
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
     1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
     1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f
  };
  static GLfloat cube_data[] = {
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
  GLuint i = genId();
  gstatic_t * q = malloc(sizeof *q);
  assert(q);
  _garray[i].geom = q;
  _garray[i].type = type;
  glGenVertexArrays(1, &_garray[i].vao);
  glBindVertexArray(_garray[i].vao);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glGenBuffers(1, &(q->buffer));
  glBindBuffer(GL_ARRAY_BUFFER, q->buffer);
  switch(type) {
  case GE_QUAD:
    glBufferData(GL_ARRAY_BUFFER, sizeof quad_data, quad_data, GL_STATIC_DRAW);
    break;
  case GE_CUBE:
    glBufferData(GL_ARRAY_BUFFER, sizeof cube_data, cube_data, GL_STATIC_DRAW);
    break;
  default:
    assert(0);
    break;
  }
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (8 * sizeof *quad_data), (const void *)0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (8 * sizeof *quad_data), (const void *)(3 * sizeof *quad_data));
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, (8 * sizeof *quad_data), (const void *)(6 * sizeof *quad_data));
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  return ++i;
}

static GLfloat * mkSphereVerticesf(GLuint slices, GLuint stacks) {
  int i, j, k;
  GLdouble phi, theta, r, y;
  GLfloat * data;
  GLdouble c2MPI_Long = 2.0 * M_PI / slices;
  GLdouble cMPI_Lat = M_PI / stacks;
  data = malloc(5 * (slices + 1) * (stacks + 1) * sizeof *data);
  assert(data);
  for(i = 0, k = 0; i <= (int)stacks; i++) {
    theta  = -M_PI_2 + i * cMPI_Lat;
    y = sin(theta);
    r = cos(theta);
    for(j = 0; j <= (int)slices; j++) {
      phi = j * c2MPI_Long;
      data[k++] = -r * cos(phi);
      data[k++] = y;
      data[k++] = r * sin(phi);
      data[k++] = phi / (2.0 * M_PI);
      data[k++] = (theta + M_PI_2) / M_PI;
    }
  }
  return data;
}

static GL4Dvaoindex * mkRegularGridTriangleIndices(GLuint width, GLuint height) {
  int y, ny, x, nx, k, yw, nyw, wm1 = width - 1, hm1 = height - 1;
  GLuint * index;
  assert(height > 1 && width > 1);
  index = malloc(6 * wm1 * hm1 * sizeof *index);
  assert(index);
  for(y = 0, k = 0; y < hm1; y++) {
    ny = y + 1;
    yw = y * width;
    nyw = ny * width;
    for(x = 0; x < wm1; x++) {
      nx = x + 1;
      index[k++] = yw  + x;
      index[k++] = yw  + nx;
      index[k++] = nyw + x;

      index[k++] = nyw + x;
      index[k++] = yw + nx;
      index[k++] = nyw + nx;
    }
  }
  return index;
}

static GL4Dvaoindex * mkRegularGridStripsIndices(GLuint width, GLuint height) {
  int y, x, k, yw, nyw, hm1 = height - 1;
  GLuint * index;
  assert(height > 1 && width > 1);
  index = malloc(2 * width * hm1 * sizeof *index);
  assert(index);
  for(y = 0, k = 0; y < hm1; y++) {
    yw = y * width;
    nyw = yw + width;
    for(x = 0; x < (int)width; x++) {
      index[k++] = nyw + x;
      index[k++] = yw  + x;
    }
  }
  return index;
}

static GL4Dvaoindex * mkRegularGridStripIndices(GLuint width, GLuint height) {
  int y, x, k, yw, nyw, nnyw, wm1 = width - 1, hm1 = height - 1, hi = hm1 - ((height&1) ? 0 : 1);
  GLuint * index;
  assert(height > 1 && width > 1);
  index = malloc(2 * width * hm1 * sizeof *index);
  assert(index);
  for(y = 0, k = 0; y < hi; y += 2) {
    yw = y * width;
    nyw = yw + width;
    nnyw = nyw + width;
    for(x = 0; x < (int)width; x++) {
      index[k++] = nyw + x;
      index[k++] = yw  + x;
    }
    for(x = wm1; x >= 0; x--) {
      index[k++] = nyw  + x;
      index[k++] = nnyw + x;
    }
  }
  if(!(height&1)) {
    yw = y * width;
    nyw = yw + width;
    for(x = 0; x < (int)width; x++) {
      index[k++] = nyw + x;
      index[k++] = yw + x;
    }
  }
  return index;
}

static inline void fcvNormals(GLfloat * p, GLfloat y, int i) {
  (void)y; /* warning silenced */
  p[i] = 2.0f * p[i - 3] / sqrt(5.0);
  p[i + 1] = 1.0f / sqrt(5.0);
  p[i + 2] = 2.0f * p[i - 1] / sqrt(5.0);
}

static inline void fcvbNormals(GLfloat * p, GLfloat y, int i) {
  p[i] = 0;
  p[i + 1] = y;
  p[i + 2] = 0;
}

/*!\brief Macro servant à remplir des FANs, soit en disk soit en cone.
 * d est data, i est indice à partir duquel remplir, ym est le y du
 * point central, ye est le y du point extreme, slices est le nombre
 * de longitudes et normals est la fonction calculant les normales.
*/
#define DISK_FAN(d, i, ym, ye, slices, normals) do {			\
    int j;								\
    const GLdouble _1pi_4 = M_PI / 4, _3pi_4 = 3 * M_PI / 4;		\
    const GLdouble _5pi_4 = 5 * M_PI / 4, _7pi_4 = 7 * M_PI / 4;	\
    GLdouble c2MPI_Long = 2 * M_PI / slices, sens = SIGN(ym), phi;	\
    (d)[(i)++] = 0; (d)[(i)++] = ym; (d)[(i)++] = 0;			\
    (d)[(i)++] = 0; (d)[(i)++] = sens; (d)[(i)++] = 0;			\
    (d)[(i)++] = 0.5; (d)[(i)++] = 0.5;					\
    for(j = 0; j <= (slices); j++) {					\
      phi = j * c2MPI_Long;						\
      (d)[(i)++] = -cos(sens * phi);					\
      (d)[(i)++] = (ye);						\
      (d)[(i)++] = sin(sens * phi);					\
      (normals)((d), (ye), (i)); (i) += 3;				\
      if(phi < _1pi_4 || phi > _7pi_4) {				\
	(d)[(i)++] = 1.0;						\
	(d)[(i)++] = 0.5 + tan(phi) / 2.0;				\
      } else if(phi < _3pi_4) {						\
	(d)[(i)++] = 0.5 - tan(phi - GL4DM_PI_2) / 2.0;			\
	(d)[(i)++] = 1.0;						\
      } else if(phi < _5pi_4) {						\
	(d)[(i)++] = 0.0;						\
	(d)[(i)++] = 0.5 - tan(phi) / 2.0;				\
      } else {								\
	(d)[(i)++] = 0.5 + tan(phi - GL4DM_PI_2) / 2.0;			\
	(d)[(i)++] = 0.0;						\
      }									\
    }									\
  } while(0)


static GLfloat * mkConeVerticesf(GLuint slices, GLboolean base) {
  int j, k = 0;
  GLdouble phi;
  GLfloat * data;
  GLdouble c2MPI_Long = 2.0 * M_PI / slices, s;
  data = malloc((16 * (slices + 1) + (base ? 8 : 0) * (slices + 2)) * sizeof *data);
  assert(data);
  for(j = 0; j <= (int)slices; j++) {
    data[k++] = 0; data[k++] = 1; data[k++] = 0;
    data[k++] = 0; data[k++] = 1; data[k++] = 0;
    data[k++] = (s = j / (GLdouble)slices); data[k++] = 1;
    phi = j * c2MPI_Long;
    data[k++] = -cos(phi);
    data[k++] = -1;
    data[k++] = sin(phi);
    fcvNormals(data, 0, k); k += 3;
    data[k++] = s; data[k++] = 0;
  }
  if(base)
    DISK_FAN(data, k, -1, -1, (int)slices, fcvbNormals);
  return data;
}

static GLfloat * mkFanConeVerticesf(GLuint slices, GLboolean base) {
  int k = 0;
  GLfloat * data;
  data = malloc((base ? 16 : 8) * (slices + 2) * sizeof *data);
  assert(data);
  DISK_FAN(data, k, 1, -1, (int)slices, fcvNormals);
  if(base)
    DISK_FAN(data, k, -1, -1, (int)slices, fcvbNormals);
  return data;
}

static GLfloat * mkCylinderVerticesf(GLuint slices, GLboolean base) {
  int j, k = 0;
  GLdouble phi;
  GLfloat * data;
  GLdouble c2MPI_Long = 2.0 * M_PI / slices, s;
  data = malloc((16 * (slices + 1) + (base ? 16 : 0) * (slices + 2)) * sizeof *data);
  assert(data);
  for(j = 0; j <= (int)slices; j++) {
    phi = j * c2MPI_Long;
    data[k++] = -cos(phi);
    data[k++] = 1;
    data[k++] = sin(phi);
    fcvNormals(data, 0, k); k += 3;
    data[k++] = (s = j / (GLdouble)slices); data[k++] = 1;
    data[k] = data[k - 8]; k++;
    data[k++] = -1;
    data[k] = data[k - 8]; k++;
    fcvNormals(data, 0, k); k += 3;
    data[k++] = s; data[k++] = 0;
  }
  if(base) {
    DISK_FAN(data, k,  1,  1, (int)slices, fcvbNormals);
    DISK_FAN(data, k, -1, -1, (int)slices, fcvbNormals);
  }
  return data;
}

static GLfloat * mkDiskVerticesf(GLuint slices) {
  int k = 0;
  GLfloat * data;
  data = malloc(8 * (slices + 2) * sizeof *data);
  assert(data);
  DISK_FAN(data, k, 0, 0, (int)slices, fcvbNormals);
  return data;
}

static GLfloat * mkTorusVerticesf(GLuint slices, GLuint stacks, GLfloat radius) {
  int i, j, k;
  GLdouble phi, theta, r, x, y, z;
  GLfloat * data;
  GLdouble c2MPI_Long = 2.0 * M_PI / slices;
  GLdouble c2MPI_Lat  = 2.0 * M_PI / stacks;
  data = malloc(8 * (slices + 1) * (stacks + 1) * sizeof *data);
  assert(data);
  for(i = 0, k = 0; i <= (int)stacks; i++) {
    theta  = i * c2MPI_Lat;
    y = radius * sin(theta);
    r = radius * cos(theta);
    for(j = 0; j <= (int)slices; j++) {
      phi = j * c2MPI_Long;
      x = -cos(phi);
      z = sin(phi);
      data[k++] = (1 - radius + r) * x;
      data[k++] = y;
      data[k++] = (1 - radius + r) * z;
      data[k + 0] = data[k - 3] - (1 - radius) * x;
      data[k + 1] = 0;
      data[k + 2] = data[k - 1] - (1 - radius) * z;
      MVEC3NORMALIZE(&data[k]); k += 3;
      data[k++] = phi   / (2.0 * M_PI);
      data[k++] = theta / (2.0 * M_PI);
    }
  }
  return data;
}

static GLfloat * mkGrid2dVerticesf(GLuint width, GLuint height, GLfloat * heightmap) {
  int i, j, k, iw;
  GLdouble x, z, tx, tz;
  GLfloat * data;
  data = malloc(8 * width * height * sizeof *data);
  assert(data);
  if(heightmap) {
    for(i = 0, k = 0; i < (int)height; i++) {
      z = 1.0 - 2.0 * (tz = i / (height - 1.0));
      iw = i * width;
      for(j = 0; j < (int)width; j++) {
	x = -1.0 + 2.0 * (tx = j / (width - 1.0));
	data[k++] = x; data[k++] = 2.0 * heightmap[iw + j] - 1.0; data[k++] = z;
	k += 3;
	data[k++] = tx; data[k++] = tz;
      }
    }
    mkGrid2dNormalsf(width, height, data);
  } else {
    for(i = 0, k = 0; i < (int)height; i++) {
      z = -1.0 + 2.0 * (tz = i / (height - 1.0));
      for(j = 0; j < (int)width; j++) {
	x = -1.0 + 2.0 * (tx = j / (width - 1.0));
	data[k++] = x;  data[k++] = 0; data[k++] = z;
	data[k++] = 0;  data[k++] = 1; data[k++] = 0;
	data[k++] = tx; data[k++] = tz;
      }
    }
  }
  return data;
}

static void mkGrid2dNormalsf(GLuint width, GLuint height, GLfloat * data) {
  int x, z, zw, i, wm1 = width - 1, hm1 = height - 1;
  GLfloat n[18];
  for(z = 1; z < hm1; z++) {
    zw = z * width;
    for(x = 1; x < wm1; x++) {
      triangleNormalf(&n[0],  &data[8 * (x + zw)], &data[8 * (x + 1 + zw)], &data[8 * (x + 1 + (z + 1) * width)]);
      triangleNormalf(&n[3],  &data[8 * (x + zw)], &data[8 * (x + 1 + (z + 1) * width)], &data[8 * (x + (z + 1) * width)]);
      triangleNormalf(&n[6],  &data[8 * (x + zw)], &data[8 * (x + (z + 1) * width)], &data[8 * (x - 1 + zw)]);
      triangleNormalf(&n[9],  &data[8 * (x + zw)], &data[8 * (x - 1 + zw)], &data[8 * (x - 1 + (z - 1) * width)]);
      triangleNormalf(&n[12], &data[8 * (x + zw)], &data[8 * (x - 1 + (z - 1) * width)], &data[8 * (x + (z - 1) * width)]);
      triangleNormalf(&n[15], &data[8 * (x + zw)], &data[8 * (x + (z - 1) * width)], &data[8 * (x + 1 + zw)]);
      data[8 * (x + zw) + 3] = 0;
      data[8 * (x + zw) + 4] = 0;
      data[8 * (x + zw) + 5] = 0;
      for(i = 0; i < 6; i++) {
        data[8 * (x + zw) + 3] += n[3 * i + 0];
        data[8 * (x + zw) + 4] += n[3 * i + 1];
        data[8 * (x + zw) + 5] += n[3 * i + 2];
      }
      data[8 * (x + zw) + 3] /= 6.0;
      data[8 * (x + zw) + 4] /= 6.0;
      data[8 * (x + zw) + 5] /= 6.0;
    }
  }
}

static inline void triangleNormalf(GLfloat * out, GLfloat * p0, GLfloat * p1, GLfloat * p2) {
  GLfloat v0[3], v1[3];
  v0[0] = p1[0] - p0[0];
  v0[1] = p1[1] - p0[1];
  v0[2] = p1[2] - p0[2];
  v1[0] = p2[0] - p1[0];
  v1[1] = p2[1] - p1[1];
  v1[2] = p2[2] - p1[2];
  MVEC3CROSS(out, v0, v1);
  MVEC3NORMALIZE(out);
}
