/*!\file gl4dfMedian.c
 *
 * \brief filre médian à partir d'une texture ou l'écran
 * vers une texture ou l'écran.
 *
 * \author Farès BELHADJ amsi@ai.univ-paris8.fr
 * \date March 08, 2017
 *
 */

#include "gl4du.h"
#include "gl4df.h"
#include "gl4dfCommon.h"

static GLuint _medianPId = 0, _tempTexId[3] = {0};

static void init(void);
static void quit(void);

MKFWINIT4(median, void, GLuint, GLuint, GLuint, GLboolean);

void gl4dfMedian(GLuint in, GLuint out, GLuint nb_iterations, GLboolean flipV) {
  medianfptr(in, out, nb_iterations, flipV);
}

/* appelée la première fois */
static void medianfinit(GLuint in, GLuint out, GLuint nb_iterations, GLboolean flipV) {
  init();
  medianfptr = medianffunc;
  medianfptr(in, out, nb_iterations, flipV);
}

/* appelée les autres fois (après la première qui lance init) */
static void medianffunc(GLuint in, GLuint out, GLuint nb_iterations, GLboolean flipV) {
  GLuint rout = out, fbo, flipflop[2];
  GLint vp[4], w, h, cfbo, ctex, cpId;
  GLuint i;
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
  fcommMatchTex(_tempTexId[2], rout);
  flipflop[!(nb_iterations&1)] = rout;
  flipflop[nb_iterations&1] = _tempTexId[2];
#ifndef __GLES4D__
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
  if(dt) glDisable(GL_DEPTH_TEST);
  if(bl) glDisable(GL_BLEND);
  glViewport(0, 0, w, h);
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo); {
    GLfloat step[2] = { 1.0f / (w - 1.0f), 1.0f / (h - 1.0f) };
    glUseProgram(_medianPId);
    glUniform1i(glGetUniformLocation(_medianPId,  "myTex"), 0);
    glUniform1i(glGetUniformLocation(_medianPId,  "inv"), flipV);
    glUniform2fv(glGetUniformLocation(_medianPId,  "step"), 1, step);
    glActiveTexture(GL_TEXTURE0);
    for(i = 0; i < nb_iterations; ++i) {
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, flipflop[i&1],  0);
      glBindTexture(GL_TEXTURE_2D, (i == 0) ? in : flipflop[!(i&1)]);
      gl4dgDraw(fcommGetPlane());
      glUniform1i(glGetUniformLocation(_medianPId,  "inv"), 0);
    }
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
  if(!_medianPId) {
    const char * imfs =
      "<imfs>gl4df_median.fs</imfs>\n"
#ifdef __GLES4D__
      "#version 300 es\n"
#else
      "#version 330\n"
#endif
      "in  vec2 vsoTexCoord;\n\
       out vec4 fragColor;\n\
       uniform sampler2D myTex;\n\
       uniform vec2 step;\n\
       vec2 offset[9] = vec2[](vec2(-step.x , -step.y), vec2( 0.0, -step.y), vec2( step.x , -step.y), \n\
                                    vec2(-step.x, 0.0),        vec2( 0.0, 0.0),     vec2( step.x, 0.0), \n\
                                    vec2(-step.x,   step.y),  vec2( 0.0, step.y), vec2( step.x ,  step.y) );\n\
       vec4 median(void) {\n\
         vec4 echantillon[9], c;\n\
         echantillon[0].rgb = texture(myTex, vsoTexCoord.st + offset[0]).rgb;\n\
         echantillon[0].a = dot(echantillon[0].rgb, echantillon[0].rgb);\n\
         for(int i = 1, j; i < 9; i++) {\n\
           c.rgb = texture(myTex, vsoTexCoord.st + offset[i]).rgb;\n\
           c.a = dot(c.rgb, c.rgb);\n\
           for(j = 0; j < i; j++)\n\
             if(c.a > echantillon[j].a) break; // PUTAIN !!!!!!!\n\
           for(int k = i - 1; k >= j; k--)\n\
             echantillon[k + 1] = echantillon[k];\n\
           echantillon[j] = c;\n\
         }\n\
         return vec4(echantillon[4].rgb, 1.0);\n\
       }\n\
       void main(void) {\n\
         fragColor = median();\n\
       }";
    _medianPId = gl4duCreateProgram(gl4dfBasicVS, imfs, NULL);
    gl4duAtExit(quit);
  }
}

static void quit(void) {
  if(_tempTexId[0]) {
    glDeleteTextures((sizeof _tempTexId / sizeof *_tempTexId), _tempTexId);
    _tempTexId[0] = 0;
  }
  _medianPId = 0;
  medianfptr = medianfinit;
}
