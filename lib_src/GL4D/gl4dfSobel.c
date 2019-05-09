/*!\file gl4dfSobel.c
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

static GLfloat _mixFactor = 0.5f/* , _color[4] = {1, 1, 1, 1} */;
static GLuint _sobelPId = 0, _mixMode = 0 /* none */, _tempTexId[2] = {0};
static GLboolean _isLuminance = GL_TRUE, _isInvert = GL_TRUE;

static void init(void);
static void quit(void);

MKFWINIT3(sobel, void, GLuint, GLuint, GLboolean);

void gl4dfSobel(GLuint in, GLuint out, GLboolean flipV) {
  sobelfptr(in, out, flipV);
}

/* void gl4dfSobelSetColor(GLfloat * vec4Color) { */
/*   _color[0] = vec4Color[0]; _color[1] = vec4Color[1]; _color[2] = vec4Color[2]; _color[3] = vec4Color[3];  */
/* } */

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
  sobelfptr = sobelffunc;
  sobelfptr(in, out, flipV);
}

/* appelée les autres fois (après la première qui lance init) */
static void sobelffunc(GLuint in, GLuint out, GLboolean flipV) {
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
    w = vp[2];// - vp[0];
    h = vp[3];// - vp[1];
    fcommMatchTex(rout = _tempTexId[1], out);
  } else {
    glBindTexture(GL_TEXTURE_2D, out);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
  }
#ifndef __GLES4D__
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
  if(dt) glDisable(GL_DEPTH_TEST);
  if(bl) glDisable(GL_BLEND);
  glViewport(0, 0, w, h);
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo); {
    GLfloat step[] = {1.0f / (GLfloat)w, 1.0f / (GLfloat)h};
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rout,  0);
    glUseProgram(_sobelPId);
    glUniform1i(glGetUniformLocation(_sobelPId,  "myTexture"), 0);
    glUniform1i(glGetUniformLocation(_sobelPId,  "inv"), flipV);
    glUniform1i(glGetUniformLocation(_sobelPId,  "width"), w);
    glUniform1i(glGetUniformLocation(_sobelPId,  "height"), h);
    glUniform2fv(glGetUniformLocation(_sobelPId,  "step"), 1, step);
    /* glUniform4fv(glGetUniformLocation(_sobelPId,  "color"), 1, _color); */
    glUniform1i(glGetUniformLocation(_sobelPId,  "invResult"), _isInvert);
    glUniform1i(glGetUniformLocation(_sobelPId,  "luminance"), _isLuminance);
    glUniform1i(glGetUniformLocation(_sobelPId,  "mixMode"), _mixMode);
    glUniform1f(glGetUniformLocation(_sobelPId,  "mixFactor"), _mixFactor);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, in);
    gl4dgDraw(fcommGetPlane());
    glBindTexture(GL_TEXTURE_2D, 0);
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
  GLint ctex;
  GLuint i;
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
  if(!_sobelPId) {
    const char * imfs =
      "<imfs>gl4df_sobel.fs</imfs>\n"
#ifdef __GLES4D__
      "#version 300 es\n"
#else
      "#version 330\n"
#endif
      "uniform sampler2D myTexture;\n  \
       uniform vec2 step;\n	      \
       uniform int width, height, invResult, luminance, mixMode;\n	\
       uniform float mixFactor;\n					\
       in  vec2 vsoTexCoord;\n	      \
       out vec4 fragColor;\n	      \
       const int ossize = 9;\n						\
       const vec2 G[ossize] = vec2[]( vec2(1.0,  1.0), vec2(0.0,  2.0), vec2(-1.0,  1.0),\n \
                                      vec2(2.0,  0.0), vec2(0.0,  0.0), vec2(-2.0,  0.0),\n \
                                      vec2(1.0, -1.0), vec2(0.0, -2.0), vec2(-1.0, -1.0) );\n \
       vec2 offset[ossize] = vec2[](vec2(-step.x , -step.y), vec2( 0.0, -step.y), vec2( step.x , -step.y),\n \
                                    vec2(-step.x, 0.0),        vec2( 0.0, 0.0),     vec2( step.x, 0.0),\n \
                                    vec2(-step.x,   step.y),  vec2( 0.0, step.y), vec2( step.x ,  step.y) );\n	\
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
    gl4duAtExit(quit);
  }
}

static void quit(void) {
  if(_tempTexId[0]) {
    glDeleteTextures((sizeof _tempTexId / sizeof *_tempTexId), _tempTexId);
    _tempTexId[0] = 0;
  }
  _sobelPId = 0;
  sobelfptr = sobelfinit;
}
