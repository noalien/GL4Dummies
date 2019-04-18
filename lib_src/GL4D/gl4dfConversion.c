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
  GLint vp[4], w, h, cfbo, ctex;
  GLuint fbo;
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &ctex);
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &cfbo);
  glGetIntegerv(GL_VIEWPORT, vp);
  if(*out == 0) {
    w = vp[2];// - vp[0];
    h = vp[3];// - vp[1];
    glGenTextures(1, out);
    glBindTexture(GL_TEXTURE_2D, *out);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _filter);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  } else {
    glBindTexture(GL_TEXTURE_2D, *out);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
  }
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, (GLuint)cfbo);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *out,  0);
  glBlitFramebuffer(vp[0], vp[1], vp[0] + vp[2], vp[1] + vp[3], 0, 0, w, h, GL_COLOR_BUFFER_BIT, _filter);
  glDeleteFramebuffers(1, &fbo);
  glBindTexture(GL_TEXTURE_2D, (GLuint)ctex);
  glBindFramebuffer(GL_FRAMEBUFFER, (GLuint)cfbo);
}

void gl4dfConvTex2Frame(GLuint in) {
  GLint vp[4], w, h, cfbo, ctex;
  GLuint fbo;
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &ctex);
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &cfbo);
  glGetIntegerv(GL_VIEWPORT, vp);

  glBindTexture(GL_TEXTURE_2D, in);
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);

  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
  glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, in,  0);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, (GLuint)cfbo);
  glBlitFramebuffer(0, 0, w, h, vp[0], vp[1], vp[0] + vp[2], vp[1] + vp[3], GL_COLOR_BUFFER_BIT, _filter);

  glDeleteFramebuffers(1, &fbo);
  glBindTexture(GL_TEXTURE_2D, (GLuint)ctex);
  glBindFramebuffer(GL_FRAMEBUFFER, (GLuint)cfbo);
}

void gl4dfConvTex2Tex(GLuint in, GLuint out, GLboolean flipV) {
  GLint inw, inh, outw, outh, ctex, cfbo;
  GLuint fbo[2];
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &ctex);
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &cfbo);

  glBindTexture(GL_TEXTURE_2D, in);
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &inw);
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &inh);
  glBindTexture(GL_TEXTURE_2D, out);
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &outw);
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &outh);

  glGenFramebuffers(2, fbo);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo[0]);
  glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, in,  0);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo[1]);
  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, out,  0);
  
  if(flipV)
    glBlitFramebuffer(0, 0, inw, inh, 0, outh, outw, 0, GL_COLOR_BUFFER_BIT, _filter);
  else
    glBlitFramebuffer(0, 0, inw, inh, 0, 0, outw, outh, GL_COLOR_BUFFER_BIT, _filter);

  glDeleteFramebuffers(2, fbo);
  glBindTexture(GL_TEXTURE_2D, (GLuint)ctex);
  glBindFramebuffer(GL_FRAMEBUFFER, (GLuint)cfbo);
}

void gl4dfConvSetFilter(GLenum filter) {
  _filter = filter;
}

