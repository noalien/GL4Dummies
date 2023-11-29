#include "ellule.h"
#include "elluleRaster.h"

static uint32_t _width = 0, _height = 0;
static uint32_t * _pixels = NULL;
static float    * _depths = NULL;

/* ** machinerie interne gérée par FB ** */
static GLuint _id = 0;
static GLuint _pId = 0;
static GLuint _quadId = 0;
/*****************************************/

static void _quit(void);

/* TODO : écrire et mettre en place le quit */
int elInit(int argc, char ** argv, const char * title, int ww, int wh, int width, int height) {
  const char * imvs =
    "<imvs>gl4dp_basic_with_transforms.vs</imvs>\
     #version 330\n							\
     layout (location = 0) in vec3 vsiPosition;\n			\
     layout (location = 1) in vec3 vsiNormal;\n				\
     layout (location = 2) in vec2 vsiTexCoord;\n			\
     out vec2 vsoTexCoord;\n						\
     void main(void) {\n						\
       gl_Position = vec4(vsiPosition, 1.0);\n				\
       vsoTexCoord = vsiTexCoord;\n					\
     }\n";
  const char * imfs =
    "<imfs>gl4dp_basic.fs</imfs>\n\
     #version 330\n							\
     uniform sampler2D myTexture;\n					\
     in  vec2 vsoTexCoord;\n						\
     out vec4 fragColor;\n						\
     void main(void) {\n						\
       fragColor = texture(myTexture, vsoTexCoord);\n			\
     }";
  assert(width && height);
  assert(_pixels == NULL); /* empêche d'appeler init plus d'une fois */
  if(!gl4duwCreateWindow(argc, argv, title, 0, 0, ww, wh, GL4DW_SHOWN))
    return 0;
  
  _width  = width;
  _height = height;
  glGenTextures(1, &_id);
  assert(&_id);
  _pixels = calloc(_width * _height, sizeof *_pixels);
  assert(_pixels);
  _depths = calloc(_width * _height, sizeof *_depths);
  assert(_depths);
  atexit(_quit);
  int vp[] = { 0, 0, _width, _height };
  elSetViewport(vp);
  
  glBindTexture(GL_TEXTURE_2D, _id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  if(_pId == 0) {
    _quadId = gl4dgGenQuadf();
    _pId = gl4duCreateProgram(imvs, imfs, NULL);
  }
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glBindTexture(GL_TEXTURE_2D, 0);
  return 1;
}

/*!\brief retourne la largeur de l'écran. */
uint32_t elGetWidth(void) {
  return _width;
}

/*!\brief retourne la largeur de l'écran. */
uint32_t elGetHeight(void) {
  return _height;
}

/*!\brief Efface l'écran en mettant tous les octets à 0. */
void elClear(void) {
  memset(_pixels, 0, _width * _height * sizeof *_pixels);
  memset(_depths, 0, _width * _height * sizeof *_depths);
}

/*!\brief retourne le pointeur vers les pixels de l'écran courant. */
uint32_t * elGetPixels(void) {
  return _pixels;
}

/*!\brief retourne le pointeur vers les pixels de l'écran courant. */
float    * elGetDepths(void) {
  return _depths;
}

/*!\brief met à jour l'écran
 */
void elUpdate(void) {
  glBindTexture(GL_TEXTURE_2D, _id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _pixels);
  glDisable(GL_DEPTH);
  glUseProgram(_pId);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _id);
  glUniform1i (glGetUniformLocation(_pId, "myTexture"), 0);
  gl4dgDraw(_quadId);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void _quit(void) {
  if(_pixels) {
    free(_pixels);
    _pixels = NULL;
  }
  if(_depths) {
    free(_depths);
    _depths = NULL;
  }
}
