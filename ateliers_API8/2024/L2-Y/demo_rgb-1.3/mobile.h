#ifndef _MOBILE_H
#define _MOBILE_H

#include <GL4D/gl4du.h>

#ifdef _cplusplus
extern "C" {
#endif

  typedef struct mobile_t mobile_t;
  typedef struct vec3_t vec3_t;

  struct vec3_t {
    GLfloat x, y, z;
  };
  
  struct mobile_t {
    vec3_t p, v, a, couleur;
    GLfloat r;
  };

  extern void mobile_init(int n);
  extern void mobile_pulse(float intensite_son);
  extern void mobile_simu(void);
  extern void mobile_draw(GLuint pId, GLuint oId);
  extern void mobile_quit(void);


  
#ifdef _cplusplus
}
#endif

  
#endif
