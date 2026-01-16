/*!\file gl4dfScattering.c
 *
 * \brief filre de dispersion des pixels à partir d'une texture ou
 * l'écran vers une texture ou l'écran.
 *
 * \author Farès BELHADJ amsi@ai.univ-paris8.fr
 * \date March 09, 2017
 *
 */
#include <stdlib.h>
#include <assert.h>
#include "gl4du.h"
#include "gl4df.h"
#include "gl4dfCommon.h"

static GLuint _scatteringPId = 0, _width = 1, _height = 1, _noiseTex = 0, _tempTexId[2] = {0};

static void init(void);
static void setDimensions(GLuint w, GLuint h);
static void quit(void);

MKFWINIT6(scattering, void, GLuint, GLuint, GLuint, GLuint, GLuint, GLboolean);

void gl4dfScattering(GLuint in, GLuint out, GLuint radius, GLuint displacementmap, GLuint weightmap, GLboolean flipV) {
  scatteringfptr(in, out, radius, displacementmap, weightmap, flipV);
}

void gl4dfScatteringChange(void) {
  init();
}

static void scatteringfinit(GLuint in, GLuint out, GLuint radius, GLuint displacementmap, GLuint weightmap, GLboolean flipV) {
  init();
  scatteringfptr = scatteringffunc;
  scatteringfptr(in, out, radius, displacementmap, weightmap, flipV);
}

static void scatteringffunc(GLuint in, GLuint out, GLuint radius, GLuint displacementmap, GLuint weightmap, GLboolean flipV) {
  GLuint rout = out, fbo;
  GLint vp[4], w, h, cfbo, ctex, cpId;
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
  if((GLuint)w != _width || (GLuint)h != _height)
    setDimensions(w, h);
#ifndef __GLES4D__
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
  if(dt) glDisable(GL_DEPTH_TEST);
  if(bl) glDisable(GL_BLEND);
  glViewport(0, 0, w, h);
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo); {
    GLfloat d[] = {radius / (GLfloat)_width, radius / (GLfloat)_height};
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rout,  0);
    glUseProgram(_scatteringPId);
    glUniform1i(glGetUniformLocation(_scatteringPId,  "myTexture"), 0);
    glUniform1i(glGetUniformLocation(_scatteringPId,  "noiseTexture"), 1);
    glUniform1i(glGetUniformLocation(_scatteringPId,  "wmTexture"), 2);
    glUniform1i(glGetUniformLocation(_scatteringPId,  "dmTexture"), 3);
    glUniform1i(glGetUniformLocation(_scatteringPId,  "useWM"), weightmap);
    glUniform1i(glGetUniformLocation(_scatteringPId,  "useDM"), displacementmap);
    glUniform1i(glGetUniformLocation(_scatteringPId,  "inv"), flipV);
    glUniform1i(glGetUniformLocation(_scatteringPId,  "width"), _width);
    glUniform1i(glGetUniformLocation(_scatteringPId,  "height"), _height);
    glUniform2fv(glGetUniformLocation(_scatteringPId,  "delta"), 1, d);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, in);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _noiseTex);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, weightmap);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, displacementmap);
    gl4dgDraw(fcommGetPlane());
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
  }
  if(!out) { /* Copier à l'écran en cas de out nul */
    glUseProgram(0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, _width, _height, vp[0], vp[1], vp[0] + vp[2], vp[1] + vp[3], GL_COLOR_BUFFER_BIT, GL_LINEAR);
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
  GLint vp[4], ctex;
  long unsigned int i;
  glGetIntegerv(GL_VIEWPORT, vp);
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
  if(!_scatteringPId) {
    const char * imfs =
      "<imfs>gl4df_scattering.fs</imfs>\n"
#ifdef __GLES4D__
      "#version 300 es\n"
#else
      "#version 330\n"
#endif
      "precision mediump float;\n\
       uniform sampler2D myTexture;\n\
       uniform sampler2D wmTexture;\n\
       uniform sampler2D noiseTexture;\n\
       uniform sampler2D dmTexture;\n\
       uniform vec2 delta;\n\
       uniform int width, height, useWM, useDM;\n\
       in  vec2 vsoTexCoord;\n\
       out vec4 fragColor;\n\
       vec2 decale(vec2 st) {\n\
         if(useDM == 0)\n\
           return delta * texture(noiseTexture, st).rg;\n\
         return delta * texture(noiseTexture, st + texture(dmTexture, st).rg).rg;\n\
       }\n\
       void main(void) {\n\
         if(useWM == 0) {\n\
           fragColor = texture(myTexture, vsoTexCoord.st + decale(vsoTexCoord.st));\n\
         } else {\n\
           vec4 wm = clamp(2.0 * (vec4(1.0) - texture(wmTexture, vsoTexCoord.st)) - vec4(1.0), 0.0, 1.0);\n\
           fragColor = texture(myTexture, vsoTexCoord.st + wm.r * 3.0 * decale(vsoTexCoord.st));\n\
         }\n\
       }";
    _scatteringPId = gl4duCreateProgram(gl4dfBasicVS, imfs, NULL);
  }
  if(!_noiseTex) {
    glGenTextures(1, &_noiseTex);
    gl4duAtExit(quit);
  }
  setDimensions(vp[2]/* - vp[0]*/, vp[3]/* - vp[1]*/);
}

static void setDimensions(GLuint w, GLuint h) {
  unsigned int i;
  GLfloat * noise = NULL;
  _width  = w;
  _height = h;
  noise = malloc(2 * _width * _height * sizeof *noise);
  assert(noise);
  for(i = 0; i < 2 * _width * _height; i++)
    noise[i] = gl4dmSURand();
  glBindTexture(GL_TEXTURE_2D, _noiseTex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, _width, _height, 0, GL_RG, GL_FLOAT, noise);
  free(noise);
}

static void quit(void) {
  if(_tempTexId[0]) {
    glDeleteTextures((sizeof _tempTexId / sizeof *_tempTexId), _tempTexId);
    _tempTexId[0] = 0;
  }
  if(_noiseTex) {
    glDeleteTextures(1, &_noiseTex);
    _noiseTex = 0;
    scatteringfptr = scatteringfinit;
  }
  _scatteringPId = 0;
}
