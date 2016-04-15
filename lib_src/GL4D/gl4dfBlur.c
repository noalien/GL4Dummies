/*!\file gl4dfBlur.h
 *
 * \brief filre réalisant un flou à partir d'une texture ou l'écran
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
#include "gl4dfBlurWeights.h"

static GLuint _blurPId = 0, _width = 1, _height = 1, _tempTId[3] = {0}, _plan = 0, _fbo = 0;
static GLfloat _offsetV[BLUR_MAX_RADIUS << 1], _offsetH[BLUR_MAX_RADIUS << 1];

static void init(void);
static void setDimensions(GLuint w, GLuint h);
static void quit(void);

static void blurfinit(GLuint, GLuint, GLuint, GLuint, GLuint, GLboolean);
static void blurfblur(GLuint, GLuint, GLuint, GLuint, GLuint, GLboolean);
static void (*blurfptr)(GLuint, GLuint, GLuint, GLuint, GLuint, GLboolean) = blurfinit;

void gl4dfBlur(GLuint in, GLuint out, GLuint radius, GLuint nb_iterations, GLuint weight, GLboolean flipV) {
  blurfptr(in, out, radius, nb_iterations, weight, flipV);
}

/* appelée la première fois */
static void blurfinit(GLuint in, GLuint out, GLuint radius, GLuint nb_iterations, GLuint weight, GLboolean flipV) {
  init();
  blurfptr = blurfblur;
  blurfptr(in, out, radius, nb_iterations, weight, flipV);
}

/* appelée les autres fois (après la première qui lance init) */
static void blurfblur(GLuint in, GLuint out, GLuint radius, GLuint nb_iterations, GLuint weight, GLboolean flipV) {
  int i, n;
  GLint vp[4], w, h;
  GLboolean dt = glIsEnabled(GL_DEPTH_TEST), bl = glIsEnabled(GL_BLEND);
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
  if(w != _width || h != _height)
    setDimensions(w, h);
  glViewport(0, 0, _width, _height);
  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
  radius = radius > BLUR_MAX_RADIUS ? BLUR_MAX_RADIUS : radius;
  for(n = 0; n < (int)nb_iterations; n++) {
    for(i = 0; i < 2; i++) {
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, i == 0 ? _tempTId[2] : rout,  0);
      glUseProgram(_blurPId);

      glUniform1i(glGetUniformLocation(_blurPId,  "myTexture"), 0);
      glUniform1i(glGetUniformLocation(_blurPId,  "myWeights"), 1);
      glUniform1i(glGetUniformLocation(_blurPId,  "useWeightMap"), weight ? 1 : 0);
      glUniform1i(glGetUniformLocation(_blurPId,  "inv"), i ? flipV : 0);
      glUniform1fv(glGetUniformLocation(_blurPId, "weight"), BLUR_MAX_RADIUS, &weights[(radius * (radius - 1)) >> 1]);
      glUniform2fv(glGetUniformLocation(_blurPId, "offset"), BLUR_MAX_RADIUS, (i % 2) ? _offsetH : _offsetV);
      glUniform1i(glGetUniformLocation(_blurPId,  "nweights"), radius);
      if(dt) glDisable(GL_DEPTH_TEST);
      if(bl) glDisable(GL_BLEND);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, i == 0 ? rin : _tempTId[2]);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, weight);
      gl4dgDraw(_plan);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, 0);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, 0);
    }
    rin = rout;
  }
  if(!out) { /* Copier à l'écran en cas de out nul */
    glUseProgram(0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, cfbo);
    glBlitFramebuffer(0, 0, _width, _height, vp[0], vp[1], vp[2], vp[3], GL_COLOR_BUFFER_BIT, GL_LINEAR);
  }
  glViewport(vp[0], vp[1], vp[2], vp[3]);
  glBindFramebuffer(GL_FRAMEBUFFER, cfbo);
  glPolygonMode(GL_FRONT_AND_BACK, polygonMode[0]);
  if(dt) glEnable(GL_DEPTH_TEST);
  if(bl) glEnable(GL_BLEND);
}

static void init(void) {
  GLint vp[4];
  glGetIntegerv(GL_VIEWPORT, vp);
  if(!_blurPId) {
    const char * imfs =
      "<imfs>gl4df_blur1D.fs</imfs>\n\
       #version 330\n				\
       uniform sampler2D myTexture;\n		\
       uniform sampler2D myWeights;\n		\
       uniform int nweights, useWeightMap;\n	\
       uniform float weight[128];\n		\
       uniform vec2 offset[128];\n		\
       in  vec2 vsoTexCoord;\n			\
       out vec4 fragColor;\n			\
       vec4 uniformBlur(void) {\n					\
         vec4 c = texture(myTexture, vsoTexCoord.st) * weight[0];\n	\
         for (int i = 1; i < nweights; i++) {\n				\
           c += texture(myTexture, vsoTexCoord.st + offset[i]) * weight[i];\n \
           c += texture(myTexture, vsoTexCoord.st - offset[i]) * weight[i];\n \
         }\n								\
         return c;\n							\
       }\n								\
       vec4 weightedBlur(void) {\n					\
         float w;\n							\
         int sub_nweights = 1 + int(float(nweights) * texture(myWeights, vsoTexCoord.st).r);\n \
         vec4 c = texture(myTexture, vsoTexCoord.st) * (w = weight[0]);\n \
         for (int i = 1; i < sub_nweights; i++) {\n			\
           w += 2.0 * weight[i];\n					\
         }\n								\
         for (int i = 1; i < sub_nweights; i++) {\n			\
           c += texture(myTexture, vsoTexCoord.st + offset[i]) * weight[i];\n \
           c += texture(myTexture, vsoTexCoord.st - offset[i]) * weight[i];\n \
         }\n								\
         return c / w;\n						\
       }\n								\
       void main(void) {\n						\
         fragColor = ((useWeightMap != 0) ? weightedBlur() : uniformBlur());\n \
       }";
    _blurPId = gl4duCreateProgram(gl4dfBasicVS, imfs, NULL);
  }
  if(!_plan) {
    _plan = gl4dgGenQuadf();
  }
  if(!_fbo) {
    glGenFramebuffers(1, &_fbo);
  }
  if(!_tempTId[0]) {
    int i;
    glGenTextures(3, _tempTId);
    for(i = 0; i < 3; i++) {
      glBindTexture(GL_TEXTURE_2D, _tempTId[i]);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    gl4duAtExit(quit);
  }
  setDimensions(vp[2] - vp[0], vp[3] - vp[1]);
}

static void setDimensions(GLuint w, GLuint h) {
  int i;
  _width  = w; 
  _height = h;
  for(i = 0; i < BLUR_MAX_RADIUS; i++) {
    _offsetH[(i << 1) + 0] = i / (GLfloat)_width;
    _offsetV[(i << 1) + 1] = i / (GLfloat)_height;
    _offsetH[(i << 1) + 1] = _offsetV[(i << 1) + 0] = 0.0;
  }
  for(i = 1; i < 3; i++) {
    glBindTexture(GL_TEXTURE_2D, _tempTId[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);  
  }
}

static void quit(void) {
  if(_tempTId[0]) {
    glDeleteTextures(3, _tempTId);
    _tempTId[0] = _tempTId[1] = _tempTId[2] = 0;
  }
  if(_fbo) {
    glDeleteFramebuffers(1, &_fbo);
    _fbo = 0;
  }
  blurfptr = blurfinit;
}
