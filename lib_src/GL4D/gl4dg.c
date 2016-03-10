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
#include "gl4dm.h"
#include <stdlib.h>
#include <assert.h>

typedef struct geom_t geom_t;
typedef struct gsphere_t gsphere_t;
typedef struct gquad_t gquad_t;
typedef struct gcone_t gcone_t;
typedef enum geom_e geom_e;

enum geom_e {
  GE_NONE = 0,
  GE_SPHERE,
  GE_QUAD,
  GE_CONE,
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

struct gquad_t {
  GLuint buffer;
};

struct gcone_t {
  GLuint buffers[2];
  GLuint slices;
  GLboolean base;
};

static geom_t * _garray = NULL;
static GLint _garray_size = 256;
static linked_list_t * _glist = NULL;
static int _hasInit = 0;

static void      freeGeom(void * data);
static GLuint    genId(void);
static GLfloat * mkSphereVerticesf(GLuint longitudes, GLuint latitudes);
static GLuint  * mkSphereIndex(GLuint longitudes, GLuint latitudes);
static GLfloat * mkConeVerticesf(GLuint longitudes, GLboolean base);

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
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  return ++i;
}

GLuint gl4dgGenQuadf(void) {
  GLfloat data[] = {
    /* 4 coordonnées de sommets */
    -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f, 1.0f,  1.0f, 0.0f,
    /* 4 normales */
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    /* 2 coordonnées de texture par sommet */
    0.0f, 0.0f, 1.0f, 0.0f, 
    0.0f, 1.0f, 1.0f, 1.0f
  };
  GLuint i = genId();
  gquad_t * q = malloc(sizeof *q);
  assert(q);
  _garray[i].geom = q;
  _garray[i].type = GE_QUAD;
  glGenVertexArrays(1, &_garray[i].vao);
  glBindVertexArray(_garray[i].vao);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glGenBuffers(1, &(q->buffer));
  glBindBuffer(GL_ARRAY_BUFFER, q->buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof data, data, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);  
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (const void *)((4 * 3) * sizeof *data));  
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (const void *)((8 * 3) * sizeof *data));  
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  return ++i;
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
  glGenBuffers(2, c->buffers);
  glBindBuffer(GL_ARRAY_BUFFER, c->buffers[0]);
  glBufferData(GL_ARRAY_BUFFER, 8 * (slices + 1 + /* le sommet ou le centre de la base */ 1) * sizeof *data, data, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (8 * sizeof *data), (const void *)0);  
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (8 * sizeof *data), (const void *)(3 * sizeof *data));  
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, (8 * sizeof *data), (const void *)(6 * sizeof *data));  
  free(data);
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
  case GE_QUAD:
    glBindVertexArray(_garray[id].vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    break;
  case GE_CONE:
    glBindVertexArray(_garray[id].vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, ((gcone_t *)(_garray[id].geom))->slices + 2);
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
  case GE_QUAD:
    glDeleteVertexArrays(1, &(geom->vao));
    glDeleteBuffers(1, &(((gquad_t *)(geom->geom))->buffer));
    break;
  case GE_CONE:
    glDeleteVertexArrays(1, &(geom->vao));
    glDeleteBuffers(2, ((gcone_t *)(geom->geom))->buffers);
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


static GLfloat * mkConeVerticesf(GLuint longitudes, GLboolean base) {
  int j, k = 0;
  GLdouble phi, y = -1.0;
  GLfloat * data;
  GLdouble c2MPI_Long = 2.0 * M_PI / longitudes;
  const GLdouble _1pi_4 = M_PI / 4.0, _3pi_4 = 3.0 * M_PI / 4.0, _5pi_4 = 5.0 * M_PI / 4.0, _7pi_4 = 7.0 * M_PI / 4.0;
  data = malloc(8 * (longitudes + 2) * sizeof *data);
  assert(data);
  data[k++] = 0; data[k++] = 1; data[k++] = 0;
  data[k++] = 0; data[k++] = 1; data[k++] = 0;
  data[k++] = 0.5; data[k++] = 0.5; 
  for(j = 0; j <= longitudes; j++) {
    phi = j * c2MPI_Long;
    data[k++] = cos(phi); 
    data[k++] = y; 
    data[k++] = -sin(phi);
    data[k] = 2.0f * data[k - 3] / sqrt(5.0); k++;
    data[k++] = 1.0f / sqrt(5.0); 
    data[k] = 2.0f * data[k - 3] / sqrt(5.0); k++;
    if(phi < _1pi_4 || phi > _7pi_4) {
      data[k++] = 1.0;
      data[k++] = 0.5 + tan(phi) / 2.0;
    } else if(phi < _3pi_4) {
      data[k++] = 0.5 - tan(phi - GL4DM_PI_2) / 2.0;
      data[k++] = 1.0;
    } else if(phi < _5pi_4) {
      data[k++] = 0.0;
      data[k++] = 0.5 - tan(phi) / 2.0;
    } else {
      data[k++] = 0.5 + tan(phi - GL4DM_PI_2) / 2.0;
      data[k++] = 0.0;
    }
  }
  return data;
}
