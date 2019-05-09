/*!\file gl4dfOp.c
 *
 * \brief filre permettant de réaliser des opérations entre deux
 * textures vers une troisième.
 *
 * \author Farès BELHADJ amsi@ai.univ-paris8.fr
 * \date March 14, 2018
 *
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "gl4du.h"
#include "gl4df.h"
#include "gl4dfCommon.h"

static const GLuint _op0 = GL4DF_OP_ADD;
static GLuint _opPId = 0, _op = GL4DF_OP_ADD, _tempTexId[3] = { 0 };

static void init(void);
static void quit(void);

MKFWINIT4(op, void, GLuint, GLuint, GLuint, GLboolean);

void gl4dfOp(GLuint in1, GLuint in2, GLuint out, GLboolean flipV) {
  opfptr(in1, in2, out, flipV);
}

void gl4dfOpSetOp(GL4DFenum op) {
  _op = op;
}

/* appelée la première fois */
static void opfinit(GLuint in1, GLuint in2, GLuint out, GLboolean flipV) {
  init();
  opfptr = opffunc;
  opfptr(in1, in2, out, flipV);
}

/* appelée les autres fois (après la première qui lance init) */
static void opffunc(GLuint in1, GLuint in2, GLuint out, GLboolean flipV) {
  GLuint fbo, rout = out;
  GLint vp[4], w, h, cpId, cfbo, ctex;
  GLboolean dt = glIsEnabled(GL_DEPTH_TEST), bl = glIsEnabled(GL_BLEND);
#ifndef __GLES4D__
  GLint polygonMode[2];
  glGetIntegerv(GL_POLYGON_MODE, polygonMode);
#endif
  glGetIntegerv(GL_VIEWPORT, vp);
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &cfbo);
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &ctex);
  glGetIntegerv(GL_CURRENT_PROGRAM, &cpId);

  /* vérifier toutes les dimensions */
  if( out == 0 /* pas de sortie, donc la sortie est l'écran */ ||
      (out == in1 || out == in2) /* ou une des entrées est la même que la sortie*/ )
    fcommMatchTex(rout = _tempTexId[2], out);
  if(in1 == 0) { /* Pas d'entrée 1, donc l'entrée est le dernier draw */
    fcommMatchTex(in1 = _tempTexId[0], 0);
    gl4dfConvFrame2Tex(&_tempTexId[0]);
  }
  if(in2 == 0) { /* Pas d'entrée 2, donc l'entrée est le dernier draw */
    fcommMatchTex(in2 = _tempTexId[1], 0);
    gl4dfConvFrame2Tex(&_tempTexId[1]);
  }

  glBindTexture(GL_TEXTURE_2D, rout);
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
  if(!bl) glEnable(GL_BLEND);
  if(dt) glDisable(GL_DEPTH_TEST);
#ifndef __GLES4D__
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
  glViewport(0, 0, w, h);
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo); {
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rout,  0);
    glUseProgram(_opPId);
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, in1);
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, in2);
    glUniform1i(glGetUniformLocation(_opPId,  "tex0"), 0);
    glUniform1i(glGetUniformLocation(_opPId,  "tex1"), 1);
    glUniform1i(glGetUniformLocation(_opPId,  "inv"), flipV);
    glUniform1i(glGetUniformLocation(_opPId,  "op"), _op - _op0);
    gl4dgDraw(fcommGetPlane());
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, 0);
  }
  if(!out) { /* Copier à l'écran en cas de out nul */
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, w, h, vp[0], vp[1], vp[0] + vp[2], vp[1] + vp[3], GL_COLOR_BUFFER_BIT, GL_NEAREST);
  } else if(rout == _tempTexId[2])
    gl4dfConvTex2Tex(_tempTexId[2], out, GL_FALSE);
  glViewport(vp[0], vp[1], vp[2], vp[3]);
  glUseProgram(cpId);
  glBindFramebuffer(GL_FRAMEBUFFER, cfbo);
  glBindTexture(GL_TEXTURE_2D, ctex);
#ifndef __GLES4D__
  glPolygonMode(GL_FRONT_AND_BACK, polygonMode[0]);
#endif
  if(!bl) glDisable(GL_BLEND);
  if(dt) glEnable(GL_DEPTH_TEST);
  glDeleteFramebuffers(1, &fbo);
}

static void init(void) {
  GLint ctex;
  GLuint i;
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &ctex);
  if(!_tempTexId[0])
    glGenTextures((sizeof _tempTexId / sizeof *_tempTexId), _tempTexId);
  for(i = 0; i < (sizeof _tempTexId / sizeof *_tempTexId); i++) {
    glBindTexture(GL_TEXTURE_2D, _tempTexId[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  }
  glBindTexture(GL_TEXTURE_2D, ctex);
  if(!_opPId) {
    const char * imfs =
      "<imfs>gl4df_op.fs</imfs>\n"
#ifdef __GLES4D__
      "#version 300 es\n"
#else
      "#version 330\n"
#endif
      "uniform sampler2D tex0, tex1;\n \
       uniform int op;\n	       \
       in  vec2 vsoTexCoord;\n						\
       out vec4 fragColor;\n						\
       void main(void) {\n						\
         vec4 c0 = texture(tex0, vsoTexCoord);\n			\
         vec4 c1 = texture(tex1, vsoTexCoord);\n			\
         switch(op) {\n							\
         case 0: /*  */\n						\
           fragColor = c0 + c1;\n					\
           break;\n							\
         case 1: /*  */\n						\
           fragColor = c0 - c1;\n					\
           break;\n							\
         case 2: /*  */\n						\
           fragColor = c0 * c1;\n					\
           break;\n							\
         case 3: /*  */\n						\
           fragColor = c0 / c1;\n					\
           break;\n							\
         case 4: /*  */\n						\
           fragColor = (length(c1) > 0.0) ? c1 : c0;\n			\
           break;\n							\
         default:\n							\
           fragColor = vec4(1.0, 0.0, 0.0, 1.0);\n				\
           break;\n							\
         }\n								\
       }";
    _opPId = gl4duCreateProgram(gl4dfBasicVS, imfs, NULL);
    gl4duAtExit(quit);
  }
}

static void quit(void) {
  if(_tempTexId[0]) {
    glDeleteTextures((sizeof _tempTexId / sizeof *_tempTexId), _tempTexId);
    _tempTexId[0] = 0;
  }
  _opPId = 0;
  opfptr = opfinit;
}
