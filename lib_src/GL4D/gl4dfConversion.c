/*!\file gl4dfConversion.c
 *
 * \brief filres de conversions de couleurs à partir d'une texture ou
 * l'écran vers une texture ou l'écran.
 *
 * \author Farès BELHADJ amsi@ai.univ-paris8.fr
 * \date May 27, 2016
 * 
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "gl4du.h"
#include "gl4df.h"
#include "gl4dfCommon.h"

static GLenum _filter = GL_NEAREST;
static GLuint _pId = 0;

void gl4dfConvFrame2Tex(GLuint * out) {
  GLint vp[4], w, h, n, cpId;
  GLuint cfbo;
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &n);
  cfbo = n;
  glGetIntegerv(GL_VIEWPORT, vp);
  if(*out == 0) {
    w = vp[2] - vp[0];
    h = vp[3] - vp[1];
    *out = fcommGetTempTex(0);
    glBindTexture(GL_TEXTURE_2D, *out);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  } else {
    glBindTexture(GL_TEXTURE_2D, *out);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
  }
  glGetIntegerv(GL_CURRENT_PROGRAM, &cpId);
  glUseProgram(0);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, cfbo);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _getFBO());
  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *out,  0);
  glBlitFramebuffer(vp[0], vp[1], vp[2], vp[3], 0, 0, w, h, GL_COLOR_BUFFER_BIT, _filter);
  glBindFramebuffer(GL_FRAMEBUFFER, cfbo);
  glUseProgram(cpId);
}

void gl4dfConvTex2Frame(GLuint in) {
  GLint vp[4], w, h, n, cpId;
  GLuint cfbo;
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &n);
  cfbo = n;
  glGetIntegerv(GL_VIEWPORT, vp);
  glBindTexture(GL_TEXTURE_2D, in);
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);

  glGetIntegerv(GL_CURRENT_PROGRAM, &cpId);
  glUseProgram(0);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, _getFBO());
  glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, in,  0);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, cfbo);
  glBlitFramebuffer(0, 0, w, h, vp[0], vp[1], vp[2], vp[3], GL_COLOR_BUFFER_BIT, _filter);
  glBindFramebuffer(GL_FRAMEBUFFER, cfbo);
  glUseProgram(cpId);
}

void gl4dfConvTex2Tex(GLuint in, GLuint out, GLboolean flipV) {
  GLint vp[4];
  GLboolean dt = glIsEnabled(GL_DEPTH_TEST), bl = glIsEnabled(GL_BLEND);
  GLint polygonMode[2], cpId = 0;
  GLint inw, inh, outw, outh, n;
  GLuint cfbo;
  _pId = _pId ? _pId : gl4duCreateProgram(gl4dfBasicVS, gl4dfTexFS, NULL);
  glGetIntegerv(GL_POLYGON_MODE, polygonMode);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glGetIntegerv(GL_VIEWPORT, vp);
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &n);
  cfbo = n;
  glGetIntegerv(GL_CURRENT_PROGRAM, &cpId);

  glBindTexture(GL_TEXTURE_2D, in);
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &inw);
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &inh);

  glBindTexture(GL_TEXTURE_2D, out);
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &outw);
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &outh);

  glViewport(0, 0, outw, outh);
  glBindFramebuffer(GL_FRAMEBUFFER, _getFBO()); {
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, out,  0);
    glUseProgram(_pId);
    glUniform1i(glGetUniformLocation(_pId,  "tex"), 0);
    glUniform1i(glGetUniformLocation(_pId,  "inv"), flipV ? 1 : 0);
    if(dt) glDisable(GL_DEPTH_TEST);
    if(bl) glDisable(GL_BLEND);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, in);
    gl4dgDraw(fcommGetPlane());
  }
  glViewport(vp[0], vp[1], vp[2], vp[3]);
  glBindFramebuffer(GL_FRAMEBUFFER, cfbo);
  glPolygonMode(GL_FRONT_AND_BACK, polygonMode[0]);
  glUseProgram(cpId);
  if(dt) glEnable(GL_DEPTH_TEST);
  if(bl) glEnable(GL_BLEND);
}

void gl4dfConvSetFilter(GLenum filter) {
  _filter = filter;
}

