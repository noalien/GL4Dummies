/*!\file gl4dg.c
 *
 * \brief The GL4Dummies Geometry
 *
 * \author Fares BELHADJ amsi@ai.univ-paris8.fr
 * \date February 22, 2016
 * \todo commenter
 * \todo gérer GLES 2, SHORT à la place de INT
 */

#include "linked_list.h"
#include "gl4dg.h"
#include <stdlib.h>
#include <assert.h>

typedef struct geom_t geom_t;
typedef struct gsphere_t gsphere_t;
typedef enum geom_e geom_e;

enum geom_e {
  GE_NONE = 0,
  GE_SPHERE,
  GE_CUBE,
  GE_CYLINDER,
  GE_TORUS,
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
};

static geom_t * _garray = NULL;
static GLint _garray_size = 256;
static linked_list_t * _glist = NULL;
static int _hasInit = 0;

static void      freeGeom(void * data);
static GLuint    genId(void);
static GLfloat * mkSphereVerticesf(GLuint longitudes, GLuint latitudes);
static GLuint  * mkSphereIndex(GLuint longitudes, GLuint latitudes);

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
    llFree(_glist, freeGeom);
    _glist = NULL;
  }
  if(_garray) {
    free(_garray);
    _garray = NULL;
  }
  _garray_size = 256;
  _hasInit = 0;
}

GLuint gl4dgGenSpheref(GLuint slices, GLuint stacks) {
  GLfloat * idata = NULL;
  GLuint * index = NULL;
  GLuint i = genId();
  gsphere_t * s = malloc(sizeof *s);
  assert(s);
  _garray[i].geom = s;
  _garray[i].type = GE_SPHERE;
  s->slices = slices; s->stacks = stacks;
  idata = mkSphereVerticesf(slices, stacks);
  index = mkSphereIndex(slices, stacks);
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
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * slices * stacks * sizeof *index, index, GL_STATIC_DRAW);
  free(idata);
  free(index);
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  return ++i;
}

void gl4dgDraw(GLuint id) {
  switch(_garray[--id].type) {
  case GE_SPHERE:
    glBindVertexArray(_garray[id].vao);
    glDrawElements(GL_TRIANGLES, 6 * ((gsphere_t *)(_garray[id].geom))->slices * ((gsphere_t *)(_garray[id].geom))->stacks, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    break;
  default:
    break;
  }
}

void gl4dgDelete(GLuint id) {
  --id;
  _garray[id].id   = id;
  _garray[id].vao  = 0;
  _garray[id].type = 0;
  freeGeom(&_garray[id]);
  llPush(_glist, &_garray[id]);
}

static void freeGeom(void * data) {
  geom_t * geom = (geom_t *)data;
  switch(geom->type) {
  case GE_SPHERE:
    glDeleteVertexArrays(1, &(geom->vao));
    glDeleteBuffers(2, ((gsphere_t *)(geom->geom))->buffers);
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

static GLfloat * mkSphereVerticesf(GLuint longitudes, GLuint latitudes) {
  int i, j, k;
  GLdouble phi, theta, r, y;
  GLfloat * data;
  GLdouble c2MPI_Long = 2.0 * M_PI / longitudes;
  GLdouble cMPI_Lat = M_PI / latitudes;
  data = malloc(5 * (longitudes + 1) * (latitudes + 1) * sizeof *data);
  assert(data);
  for(i = 0, k = 0; i <= latitudes; i++) {
    theta  = -M_PI_2 + i * cMPI_Lat;
    y = sin(theta);
    r = cos(theta);
    for(j = 0; j <= longitudes; j++) {
      phi = j * c2MPI_Long;
      data[k++] = r * cos(phi); 
      data[k++] = y; 
      data[k++] = r * sin(phi);
      data[k++] = 1.0 - phi / (2.0 * M_PI); 
      data[k++] = 1.0 - (theta + M_PI_2) / M_PI;
    }
  }
  return data;
}

static GLuint * mkSphereIndex(GLuint longitudes, GLuint latitudes) {
  int i, ni, j, nj, k;
  GLuint * index;
  index = malloc(6 * longitudes * latitudes * sizeof *index);
  assert(index);
  for(i = 0, k = 0; i < latitudes; i++) {
    ni = i + 1;
    for(j = 0; j < longitudes; j++) {
      nj = j + 1;
      index[k++] = i * (longitudes + 1) + j;
      index[k++] = ni * (longitudes + 1) + nj;
      index[k++] = i * (longitudes + 1) + nj;

      index[k++] = i * (longitudes + 1) + j;
      index[k++] = ni * (longitudes + 1) + j;
      index[k++] = ni * (longitudes + 1) + nj;
    }
  }
  return index;
}
