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
#include <GL4D/gl4du.h>
#include "gl4df.h"
#include "gl4dfCommon.h"

static GLuint _scatteringPId = 0, _width = 1, _height = 1, _noiseTex = 0;

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
  int n;
  GLint vp[4], w, h;
  GLboolean dt = glIsEnabled(GL_DEPTH_TEST), bl = glIsEnabled(GL_BLEND);
  GLuint rin = in, cfbo, rout;
  GLint polygonMode[2], cpId = 0;
  glGetIntegerv(GL_POLYGON_MODE, polygonMode);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glGetIntegerv(GL_VIEWPORT, vp);
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &n);
  cfbo = n;
  glGetIntegerv(GL_CURRENT_PROGRAM, &cpId);
  if(in == 0) { /* Pas d'entrée, donc l'entrée est le dernier draw */
    gl4dfConvFrame2Tex(&rin);
  } 
  if(out == 0) { /* Pas de sortie, donc sortie aux dimensions du viewport */
    w = vp[2] - vp[0]; 
    h = vp[3] - vp[1];
    rout = fcommGetTempTex(1);
    glBindTexture(GL_TEXTURE_2D, rout);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  } else {
    rout = out;
    glBindTexture(GL_TEXTURE_2D, out);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
  }
  if(w != _width || h != _height)
    setDimensions(w, h);
  glViewport(0, 0, _width, _height);
  glBindFramebuffer(GL_FRAMEBUFFER, fcommGetFBO()); {
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

    if(dt) glDisable(GL_DEPTH_TEST);
    if(bl) glDisable(GL_BLEND);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, rin);
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
  glBindFramebuffer(GL_FRAMEBUFFER, cfbo);
  glPolygonMode(GL_FRONT_AND_BACK, polygonMode[0]);
  glUseProgram(cpId);
  if(dt) glEnable(GL_DEPTH_TEST);
  if(bl) glEnable(GL_BLEND);
}

static void init(void) {
  GLint vp[4];
  glGetIntegerv(GL_VIEWPORT, vp);
  if(!_scatteringPId) {
    const char * imfs =
      "<imfs>gl4df_scattering.fs</imfs>\n\
       #version 330\n\
       precision mediump float;\n\
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
           vec4 wm = clamp(2.0 * (vec4(1.0) - texture(wmTexture, vsoTexCoord.st)) - vec4(1.0), 0, 1);\n\
           fragColor = texture(myTexture, vsoTexCoord.st + wm.r * 3.0 * decale(vsoTexCoord.st));\n\
         }\n\
       }";
    _scatteringPId = gl4duCreateProgram(gl4dfBasicVS, imfs, NULL);
  }
  if(!_noiseTex) {
    glGenTextures(1, &_noiseTex);
    gl4duAtExit(quit);
  }
  setDimensions(vp[2] - vp[0], vp[3] - vp[1]);
}

static void setDimensions(GLuint w, GLuint h) {
  int i;
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
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, _width, _height, 0, GL_RG, GL_FLOAT, noise);
  free(noise);
}

static void quit(void) {
  if(_noiseTex) {
    glDeleteTextures(1, &_noiseTex);
    _noiseTex = 0;
    scatteringfptr = scatteringfinit;
  }
  _scatteringPId = 0;
}
