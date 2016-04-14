/*!\file gl4dfSobel.h
 *
 * \brief filre réalisant un sobel à partir d'une texture ou l'écran
 * vers une texture ou l'écran.
 *
 * \author Farès BELHADJ amsi@ai.univ-paris8.fr
 * \date April 14, 2016
 * 
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "gl4du.h"
#include "gl4df.h"
#include "gl4dfCommon.h"

static GLfloat _mixFactor = 0.5f;
static GLuint _sobelPId = 0, _width = 0, _height = 0, _tempTId[2] = {0}, _plan = 0, _fbo = 0, _mixMode = 0 /* none */;
static GLboolean _isLuminance = GL_TRUE, _isInvert = GL_TRUE;

static void init(void);
static void quit(void);

static void sobelfinit(GLuint, GLuint, GLboolean);
static void sobelfsobel(GLuint, GLuint, GLboolean);
static void (*sobelfptr)(GLuint, GLuint, GLboolean) = sobelfinit;

void gl4dfSobel(GLuint in, GLuint out, GLboolean flipV) {
  sobelfptr(in, out, flipV);
}

void gl4dfSobelSetResultMode(GL4DFenum mode) {
  switch(mode) {
  case GL4DF_SOBEL_RESULT_RGB:
    _isLuminance = GL_FALSE;
    _isInvert = GL_FALSE;
    break;
  case GL4DF_SOBEL_RESULT_INV_RGB:
    _isLuminance = GL_FALSE;
    _isInvert = GL_TRUE;
    break;
  case GL4DF_SOBEL_RESULT_LUMINANCE:
    _isLuminance = GL_TRUE;
    _isInvert = GL_FALSE;
    break;
  case GL4DF_SOBEL_RESULT_INV_LUMINANCE:
    _isLuminance = GL_TRUE;
    _isInvert = GL_TRUE;
    break;
  default:
    fprintf(stderr, "%s: this value (%d) has no effect\n", __func__, mode);
    break;
  }
}

void gl4dfSobelSetMixMode(GL4DFenum mode) {
  switch(mode) {
  case GL4DF_SOBEL_MIX_NONE:
    _mixMode = 0;
    break;
  case GL4DF_SOBEL_MIX_ADD:
    _mixMode = 1;
    break;
  case GL4DF_SOBEL_MIX_MULT:
    _mixMode = 2;
    break;
  default:
    fprintf(stderr, "%s: this value (%d) has no effect\n", __func__, mode);
    break;
  }
}

void gl4dfSobelSetMixFactor(GLfloat factor) {
  _mixFactor = factor;
}


/* appelée la première fois */
static void sobelfinit(GLuint in, GLuint out, GLboolean flipV) {
  init();
  sobelfptr = sobelfsobel;
  sobelfptr(in, out, flipV);
}

/* appelée les autres fois (après la première qui lance init) */
static void sobelfsobel(GLuint in, GLuint out, GLboolean flipV) {
  int n;
  GLint vp[4], w, h;
  GLboolean dt = glIsEnabled(GL_DEPTH_TEST);
  GLuint rin = in, cfbo, rout = out ? out : _tempTId[1];
  GLint polygonMode[2];
  glGetIntegerv(GL_POLYGON_MODE, polygonMode);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glGetIntegerv(GL_VIEWPORT, vp);
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &n);
  cfbo = n;
  if(in == 0) { /* Pas d'entrée, donc l'entrée est le dernier draw */
    glUseProgram(0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _tempTId[0],  0);
    glBindTexture(GL_TEXTURE_2D, _tempTId[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, vp[2] - vp[0], vp[3] - vp[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBlitFramebuffer(vp[0], vp[1], vp[2], vp[3], 0, 0, _width, _height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    rin = _tempTId[0];
  } 
  if(out == 0) { /* Pas de sortie, donc sortie aux dimensions du viewport */
    w = vp[2] - vp[0]; 
    h = vp[3] - vp[1];
  } else {
    glBindTexture(GL_TEXTURE_2D, out);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
  }
  if(w != _width || h != _height) {
    _width  = w; _height = h;
    glBindTexture(GL_TEXTURE_2D, _tempTId[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);  
  }
  glViewport(0, 0, _width, _height);
  glBindFramebuffer(GL_FRAMEBUFFER, _fbo); {
    GLfloat step[] = {1.0f / (GLfloat)_width, 1.0f / (GLfloat)_height};
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rout,  0);
    glUseProgram(_sobelPId);
    glUniform1i(glGetUniformLocation(_sobelPId,  "myTexture"), 0);
    glUniform1i(glGetUniformLocation(_sobelPId,  "inv"), flipV);
    glUniform1i(glGetUniformLocation(_sobelPId,  "width"), _width);
    glUniform1i(glGetUniformLocation(_sobelPId,  "height"), _height);
    glUniform2fv(glGetUniformLocation(_sobelPId,  "step"), 1, step);
    glUniform1i(glGetUniformLocation(_sobelPId,  "invResult"), _isInvert);
    glUniform1i(glGetUniformLocation(_sobelPId,  "luminance"), _isLuminance);
    glUniform1i(glGetUniformLocation(_sobelPId,  "mixMode"), _mixMode);
    glUniform1f(glGetUniformLocation(_sobelPId,  "mixFactor"), _mixFactor);
    if(dt) glDisable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, rin);
    gl4dgDraw(_plan);
    glBindTexture(GL_TEXTURE_2D, 0);
    if(dt) glEnable(GL_DEPTH_TEST);
  }
  if(!out) { /* Copier à l'écran en cas de out nul */
    glUseProgram(0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, _width, _height, vp[0], vp[1], vp[2], vp[3], GL_COLOR_BUFFER_BIT, GL_LINEAR);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
  glViewport(vp[0], vp[1], vp[2], vp[3]);
  glBindFramebuffer(GL_FRAMEBUFFER, cfbo);
  glPolygonMode(GL_FRONT_AND_BACK, polygonMode[0]);
}

static void init(void) {
  GLint vp[4];
  glGetIntegerv(GL_VIEWPORT, vp);
  if(!_sobelPId) {
    const char * imfs =
      "<imfs>gl4df_sobel1D.fs</imfs>\n\
       #version 330\n		      \
       uniform sampler2D myTexture;\n \
       uniform vec2 step;\n	      \
       uniform int width, height, invResult, luminance, mixMode;\n	\
       uniform float mixFactor;\n					\
       in  vec2 vsoTexCoord;\n	      \
       out vec4 fragColor;\n	      \
       const int ossize = 9;\n						\
       const vec2 G[ossize] = vec2[]( vec2(1.0,  1.0), vec2(0.0,  2.0), vec2(-1.0,  1.0),\n \
                                      vec2(2.0,  0.0), vec2(0.0,  0.0), vec2(-2.0,  0.0),\n \
                                      vec2(1.0, -1.0), vec2(0.0, -2.0), vec2(-1.0, -1.0) );\n \
       vec2 p = step;\n							\
       vec2 offset[ossize] = vec2[](vec2(-p.x , -p.y), vec2( 0.0, -p.y), vec2( p.x , -p.y),\n \
                                    vec2(-p.x, 0.0),        vec2( 0.0, 0.0),     vec2( p.x, 0.0),\n \
                                    vec2(-p.x,   p.y),  vec2( 0.0, p.y), vec2( p.x ,  p.y) );\n	\
       vec3 sobel(sampler2D s, vec2 c) {\n				\
         vec2 r = vec2(0.0, 0.0), g = vec2(0.0, 0.0), b = vec2(0.0, 0.0);\n \
         for(int i = 0; i < ossize; i++) {\n				\
           r += texture(s, c + offset[i]).r * G[i];\n			\
           g += texture(s, c + offset[i]).g * G[i];\n			\
           b += texture(s, c + offset[i]).b * G[i];\n			\
         }\n								\
         return vec3(length(r), length(g), length(b));\n		\
       }\n								\
       void main(void) {\n						\
         vec4 c = texture(myTexture, vsoTexCoord);\n			\
         vec3 r = sobel(myTexture, vsoTexCoord);\n			\
         if(invResult != 0)\n						\
           r = vec3(1) - r;\n						\
         if(luminance != 0)\n						\
           r = vec3(dot(vec3(0.299, 0.587, 0.114), r));\n		\
         if(mixMode == 0)\n						\
           fragColor = vec4(r, c.a);\n					\
         else if(mixMode == 1)\n					\
           fragColor = vec4(mix(c.rgb, r, mixFactor), c.a);\n		\
         else\n								\
           fragColor = vec4(c.rgb * r, c.a);\n				\
       }";
    _sobelPId = gl4duCreateProgram(gl4dfBasicVS, imfs, NULL);
  }
  if(!_plan) {
    _plan = gl4dgGenQuadf();
  }
  if(!_fbo) {
    glGenFramebuffers(1, &_fbo);
  }
  if(!_tempTId[0]) {
    int i;
    glGenTextures(2, _tempTId);
    for(i = 0; i < 2; i++) {
      glBindTexture(GL_TEXTURE_2D, _tempTId[i]);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    gl4duAtExit(quit);
  }
}

static void quit(void) {
  if(_tempTId[0]) {
    glDeleteTextures(2, _tempTId);
    _tempTId[0] = _tempTId[1] = 0;
  }
  if(_fbo) {
    glDeleteFramebuffers(1, &_fbo);
    _fbo = 0;
  }
  sobelfptr = sobelfinit;
}
