#include <GL4D/gl4du.h>
#include <GL4D/gl4dp.h>

typedef struct mobile_t mobile_t;

struct mobile_t {
  GLfloat x, y;
  GLfloat vx, vy;
  GLfloat r;
  GLuint color;
};


extern void mobileInit(void);
extern void mobileSimu(void);
extern void mobileDraw(void);
