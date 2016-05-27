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
static void getViewport(GLuint fbo, GLint vp[4]);

void gl4dfConvFrame2Tex(GLuint * out) {
  GLint vp[4], w, h, n;
  GLuint cfbo;
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &n);
  getViewport((cfbo = n), vp);
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
  glUseProgram(0);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, cfbo);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fcommGetFBO());
  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *out,  0);
  glBlitFramebuffer(vp[0], vp[1], vp[2], vp[3], 0, 0, w, h, GL_COLOR_BUFFER_BIT, _filter);
  glBindFramebuffer(GL_FRAMEBUFFER, cfbo);
}

void gl4dfConvTex2Frame(GLuint in) {
  GLint vp[4], w, h, n;
  GLuint cfbo;
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &n);
  getViewport((cfbo = n), vp);
  glBindTexture(GL_TEXTURE_2D, in);
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);

  glUseProgram(0);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, fcommGetFBO());
  glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, in,  0);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, cfbo);
  glBlitFramebuffer(0, 0, w, h, vp[0], vp[1], vp[2], vp[3], GL_COLOR_BUFFER_BIT, _filter);
  glBindFramebuffer(GL_FRAMEBUFFER, cfbo);
}

void gl4dfConvSetFilter(GLenum filter) {
  _filter = filter;
}

static void getViewport(GLuint fbo, GLint vp[4]) {
  /* Pas sur que ça soit la bonne stratégie générale */
  /* if(fbo != 0) { */
  /*    GLint id; */
  /*    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &id); */
  /*    vp[0] = vp[1] = 0; */
  /*    glBindTexture(GL_TEXTURE_2D, id); */
  /*    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &vp[2]); */
  /*    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &vp[3]); */
  /*  } else */
  glGetIntegerv(GL_VIEWPORT, vp);
}
