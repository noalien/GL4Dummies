/*!\file gl4dfCanny.c
 *
 * \brief filtre Canny à partir d'une texture ou l'écran vers une
 * texture ou l'écran.
 *
 * \author Farès BELHADJ amsi@ai.univ-paris8.fr
 * \date March 08, 2019
 * 
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "gl4du.h"
#include "gl4df.h"
#include "gl4dfCommon.h"

static GLfloat _mixFactor = 0.5f, _lowTh = 0.3f, _highTh = 0.5f;
static GLuint _cannyPId[2] = {0}, _mixMode = 0 /* none */, _tempTexId[4] = {0};
static GLboolean _isLuminance = GL_TRUE, _isInvert = GL_TRUE;

static void init(void);
static void quit(void);

MKFWINIT3(canny, void, GLuint, GLuint, GLboolean);

void gl4dfCanny(GLuint in, GLuint out, GLboolean flipV) {
  cannyfptr(in, out, flipV);
}

void gl4dfCannySetResultMode(GL4DFenum mode) {
  switch(mode) {
  case GL4DF_CANNY_RESULT_RGB:
    _isLuminance = GL_FALSE;
    _isInvert = GL_FALSE;
    break;
  case GL4DF_CANNY_RESULT_INV_RGB:
    _isLuminance = GL_FALSE;
    _isInvert = GL_TRUE;
    break;
  case GL4DF_CANNY_RESULT_LUMINANCE:
    _isLuminance = GL_TRUE;
    _isInvert = GL_FALSE;
    break;
  case GL4DF_CANNY_RESULT_INV_LUMINANCE:
    _isLuminance = GL_TRUE;
    _isInvert = GL_TRUE;
    break;
  default:
    fprintf(stderr, "%s: this value (%d) has no effect\n", __func__, mode);
    break;
  }
}

void gl4dfCannySetMixMode(GL4DFenum mode) {
  switch(mode) {
  case GL4DF_CANNY_MIX_NONE:
    _mixMode = 0;
    break;
  case GL4DF_CANNY_MIX_ADD:
    _mixMode = 1;
    break;
  case GL4DF_CANNY_MIX_MULT:
    _mixMode = 2;
    break;
  default:
    fprintf(stderr, "%s: this value (%d) has no effect\n", __func__, mode);
    break;
  }
}

void gl4dfCannySetMixFactor(GLfloat factor) {
  _mixFactor = factor;
}

void gl4dfCannySetThresholds(GLfloat lowTh, GLfloat highTh) {
  _lowTh = lowTh;
  _highTh = highTh;
}

/* appelée la première fois */
static void cannyfinit(GLuint in, GLuint out, GLboolean flipV) {
  init();
  cannyfptr = cannyffunc;
  cannyfptr(in, out, flipV);
}

/* appelée les autres fois (après la première qui lance init) */
static void cannyffunc(GLuint in, GLuint out, GLboolean flipV) {
  GLuint rout = out, fbo;
  GLint n, vp[4], w, h, cfbo, ctex, cpId;
  GLboolean dt = glIsEnabled(GL_DEPTH_TEST), bl = glIsEnabled(GL_BLEND);
#ifndef __GLES4D__
  GLint polygonMode[2];
  glGetIntegerv(GL_POLYGON_MODE, polygonMode);
#endif
  glGetIntegerv(GL_VIEWPORT, vp);
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &cfbo);
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &ctex);
  glGetIntegerv(GL_CURRENT_PROGRAM, &cpId);
  if(in == 0) { /* Pas d'entrée, donc l'entrée est le dernier draw */
    fcommMatchTex(in = _tempTexId[0], 0);
    gl4dfConvFrame2Tex(&_tempTexId[0]);
  } else if(in == out) {
    fcommMatchTex(in = _tempTexId[0], out);
    gl4dfConvTex2Tex(out, _tempTexId[0], GL_FALSE);
  }
  if(out == 0) { /* Pas de sortie, donc sortie aux dimensions du viewport */
    w = vp[2] - vp[0]; 
    h = vp[3] - vp[1];
    fcommMatchTex(rout = _tempTexId[1], out);
  } else {
    glBindTexture(GL_TEXTURE_2D, out);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
  }
  fcommMatchTex(_tempTexId[2], rout);
  fcommMatchTex(_tempTexId[3], rout);
#ifndef __GLES4D__
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
  gl4dfBlur(in, in, 15, 1, 0, GL_FALSE);
  if(dt) glDisable(GL_DEPTH_TEST);
  if(bl) glDisable(GL_BLEND);
  glViewport(0, 0, w, h);
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo); {
    GLfloat step[] = {1.0f / (GLfloat)w, 1.0f / (GLfloat)h};
    GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, buffers);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _tempTexId[2],  0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _tempTexId[3],  0);
    glUseProgram(_cannyPId[0]);
    glUniform1i(glGetUniformLocation(_cannyPId[0],  "inv"), 0);
    glUniform1i(glGetUniformLocation(_cannyPId[0],  "myTexture"), 0);
    glUniform2fv(glGetUniformLocation(_cannyPId[0], "step"), 1, step);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, in);
    gl4dgDraw(fcommGetPlane());
    glBindTexture(GL_TEXTURE_2D, 0);
    glDrawBuffers(1, buffers);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rout,  0);
    glUseProgram(_cannyPId[1]);
    glUniform1i(glGetUniformLocation(_cannyPId[1],  "len"), 0);
    glUniform1i(glGetUniformLocation(_cannyPId[1],  "dir"), 1);
    glUniform1i(glGetUniformLocation(_cannyPId[1],  "inv"), flipV);
    glUniform2fv(glGetUniformLocation(_cannyPId[1],  "step"), 1, step);
    glUniform1i(glGetUniformLocation(_cannyPId[1],  "invResult"), _isInvert);
    glUniform1i(glGetUniformLocation(_cannyPId[1],  "luminance"), _isLuminance);
    glUniform1i(glGetUniformLocation(_cannyPId[1],  "mixMode"), _mixMode);
    glUniform1f(glGetUniformLocation(_cannyPId[1],  "mixFactor"), _mixFactor);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _tempTexId[2]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _tempTexId[3]);
    gl4dgDraw(fcommGetPlane());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDrawBuffers(1, buffers);
  }
  if(!out) { /* Copier à l'écran en cas de out nul */
    glUseProgram(0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, w, h, vp[0], vp[1], vp[0] + vp[2], vp[1] + vp[3], GL_COLOR_BUFFER_BIT, GL_LINEAR);
  }
  glViewport(vp[0], vp[1], vp[2], vp[3]);
  glBindFramebuffer(GL_FRAMEBUFFER, (GLuint)cfbo);
  glUseProgram(cpId);
#ifndef __GLES4D__
  glPolygonMode(GL_FRONT_AND_BACK, polygonMode[0]);
#endif
  if(bl) glEnable(GL_BLEND);
  if(dt) glEnable(GL_DEPTH_TEST);
  glDeleteFramebuffers(1, &fbo);
}

static void init(void) {
  GLint i, ctex;
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &ctex);
  if(!_tempTexId[0])
    glGenTextures((sizeof _tempTexId / sizeof *_tempTexId), _tempTexId);
  for(i = 0; i < (sizeof _tempTexId / sizeof *_tempTexId); ++i) {
    glBindTexture(GL_TEXTURE_2D, _tempTexId[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  }
  glBindTexture(GL_TEXTURE_2D, ctex);
  if(!_cannyPId[0]) {
    const char * imfs0 =
      "<imfs>gl4df_canny0.fs</imfs>\n"
#ifdef __GLES4D__
      "#version 300 es\n"
#else
      "#version 330\n"
#endif
      "uniform sampler2D myTexture;\n  \
       uniform vec2 step;\n						\
       in  vec2 vsoTexCoord;\n						\
       layout (location = 0) out vec4 fragLen;\n			\
       layout (location = 1) out vec4 fragDir;\n			\
       const int ossize = 9;\n						\
       const float _2pi = 6.283;\n					\
       const vec2 G[ossize] = vec2[]( vec2(1.0,  1.0), vec2(0.0,  2.0), vec2(-1.0,  1.0),\n \
                                      vec2(2.0,  0.0), vec2(0.0,  0.0), vec2(-2.0,  0.0),\n \
                                      vec2(1.0, -1.0), vec2(0.0, -2.0), vec2(-1.0, -1.0) );\n \
       vec2 offset[ossize] = vec2[](vec2(-step.x , -step.y), vec2( 0.0, -step.y), vec2( step.x , -step.y),\n \
                                    vec2(-step.x, 0.0),        vec2( 0.0, 0.0),     vec2( step.x, 0.0),\n \
                                    vec2(-step.x,   step.y),  vec2( 0.0, step.y), vec2( step.x ,  step.y) );\n	\
       void canny(in sampler2D s, in vec2 c, out vec3 len, out vec3 dir) {\n \
         vec2 r = vec2(0.0, 0.0), g = vec2(0.0, 0.0), b = vec2(0.0, 0.0);\n \
         for(int i = 0; i < ossize; i++) {\n				\
           r += texture(s, c + offset[i]).r * G[i];\n			\
           g += texture(s, c + offset[i]).g * G[i];\n			\
           b += texture(s, c + offset[i]).b * G[i];\n			\
         }\n								\
         len = vec3(length(r), length(g), length(b));\n			\
         dir = atan(vec3(r.y, g.y, b.y), vec3(r.x, g.x, b.x)) / _2pi;\n	\
         if(dir.x < 0.0) dir.x++;\n					\
         if(dir.y < 0.0) dir.y++;\n					\
         if(dir.z < 0.0) dir.z++;\n					\
       }\n								\
       void main(void) {\n						\
         vec4 c = texture(myTexture, vsoTexCoord);\n			\
         vec3 len, dir;\n						\
         canny(myTexture, vsoTexCoord, dir, len);\n			\
         fragLen = vec4(len, 1.0);\n					\
         fragDir = vec4(dir, 1.0);\n					\
       }";
    const char * imfs1 =
      "<imfs>gl4df_canny1.fs</imfs>\n"
#ifdef __GLES4D__
      "#version 300 es\n"
#else
      "#version 330\n"
#endif
      "uniform sampler2D len, dir;\n  \
       uniform vec2 step;\n						\
       uniform int invResult, luminance, mixMode;\n			\
       uniform float mixFactor;\n					\
       in  vec2 vsoTexCoord;\n						\
       out vec4 fragColor;\n						\
       const int ossize = 9;\n						\
       vec2 offset[ossize] = vec2[](vec2(-step.x , -step.y), vec2( 0.0, -step.y), vec2( step.x , -step.y),\n \
                                    vec2(-step.x, 0.0),        vec2( 0.0, 0.0),     vec2( step.x, 0.0),\n \
                                    vec2(-step.x,   step.y),  vec2( 0.0, step.y), vec2( step.x ,  step.y) );\n	\
       void main(void) {\n						\
         vec4 l = texture(len, vsoTexCoord);\n				\
         vec4 d = texture(d, vsoTexCoord);\n				\
         fragColor = l + d;\n						\
       }";
    _cannyPId[0] = gl4duCreateProgram(gl4dfBasicVS, imfs0, NULL);
    _cannyPId[1] = gl4duCreateProgram(gl4dfBasicVS, imfs1, NULL);
    gl4duAtExit(quit);
  }
}

static void quit(void) {
  if(_tempTexId[0]) {
    glDeleteTextures((sizeof _tempTexId / sizeof *_tempTexId), _tempTexId);
    _tempTexId[0] = 0;
  }
  _cannyPId[0] = 0;
  cannyfptr = cannyfinit;
}
