#include "claude.h"

static uint32_t _width = 0, _height = 0;
static uint32_t * _pixels = NULL;
static float * _depth = NULL;

/* ** interne ** */
static uint32_t _id = 0;
static uint32_t _pId = 0;
static uint32_t _quadId = 0;
/******************/

/* options à activer/désactiver */
static uint32_t _options = 31; /* voir enum dans claude.h */

int claude_init(int argc, char ** argv, const char * title, int ww, int wh, int width, int height) {
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
  if(!gl4duwCreateWindow(argc, argv, title, 0, 0, ww, wh, GL4DW_SHOWN))
    return 0;
  
  _width  = width;
  _height = height;
  glGenTextures(1, &_id);
  assert(&_id);
  _pixels = calloc(_width * _height, sizeof *_pixels);
  assert(_pixels);
  _depth = calloc(_width * _height, sizeof *_depth);
  assert(_depth);
  
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

/*!\brief Efface l'écran en mettant 0. */
void claude_clear(void) {
  memset(_pixels, 0, _width * _height * sizeof *_pixels);
  memset(_depth, 0, _width * _height * sizeof *_depth);
}

/*!\brief retourne la largeur de l'écran. */
uint32_t get_width(void) {
  return _width;
}

/*!\brief retourne la largeur de l'écran. */
uint32_t get_height(void) {
  return _height;
}

/*!\brief retourne le pointeur vers les pixels de l'écran courant. */
uint32_t * get_pixels(void) {
  return _pixels;
}

/*!\brief retourne le pointeur vers le depth buffer. */
float * get_depths(void) {
  return _depth;
}

/*!\brief met à jour l'écran
 */
void update_screen(void) {
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

void claude_enable(uint32_t opt) {
  _options |= opt;
}

void claude_disable(uint32_t opt) {
  _options &= ~opt;
}

int claude_is_enabled(uint32_t opt) {
  return _options & opt;
}
