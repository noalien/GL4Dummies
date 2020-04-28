#ifndef _MOBILE_H
#define _MOBILE_H

#include <GL4D/gl4dp.h>

typedef struct mobile_t mobile_t;
struct mobile_t {
  float x, y, vx, vy, r;
  GLuint c;
};

extern void mobile_init(int n);
extern void mobile_move(void);
extern void mobile_draw(void);
extern void mobile_delete(void);


#endif
