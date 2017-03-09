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

static GLuint _medianPId = 0;

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
  int i, n;
  GLint vp[4], w, h;
  GLboolean dt = glIsEnabled(GL_DEPTH_TEST), bl = glIsEnabled(GL_BLEND);
  GLuint rin = in, cfbo, rout = out ? out : fcommGetTempTex(1);
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
  } else {
    glBindTexture(GL_TEXTURE_2D, out);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
  }
  glBindTexture(GL_TEXTURE_2D, fcommGetTempTex(1));
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);  
  glViewport(0, 0, w, h);
  glBindFramebuffer(GL_FRAMEBUFFER, fcommGetFBO()); {
    glUseProgram(_medianPId);
    glUniform1i(glGetUniformLocation(_medianPId,  "myTex"), 0);
    glUniform1i(glGetUniformLocation(_medianPId,  "inv"), flipV);
    glUniform1i(glGetUniformLocation(_medianPId,  "width"), w);
    glUniform1i(glGetUniformLocation(_medianPId,  "height"), h);
    if(dt) glDisable(GL_DEPTH_TEST);
    if(bl) glDisable(GL_BLEND);
    glActiveTexture(GL_TEXTURE0);
    for(i = 0; i < nb_iterations; i++) {
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, (i&1) ? rin : rout,  0);
      glBindTexture(GL_TEXTURE_2D, (i&1) ? rout : rin);
      gl4dgDraw(fcommGetPlane());
      glUniform1i(glGetUniformLocation(_medianPId,  "inv"), 0);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  if(!out) { /* Copier à l'écran en cas de out nul */
    glUseProgram(0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, cfbo);
    glBlitFramebuffer(0, 0, w, h, vp[0], vp[1], vp[2], vp[3], GL_COLOR_BUFFER_BIT, GL_LINEAR);
  }
  glViewport(vp[0], vp[1], vp[2], vp[3]);
  glBindFramebuffer(GL_FRAMEBUFFER, cfbo);
  glPolygonMode(GL_FRONT_AND_BACK, polygonMode[0]);
  glUseProgram(cpId);
  if(dt) glEnable(GL_DEPTH_TEST);
  if(bl) glEnable(GL_BLEND);
}

static void init(void) {
  if(!_medianPId) {
    const char * imfs =
      "<imfs>gl4df_median.fs</imfs>\n\
       #version 330\n\
       in  vec2 vsoTexCoord;\n\
       out vec4 fragColor;\n\
       uniform sampler2D myTex;\n\
       uniform int width, height;\n\
       const int ossize = 9;\n\
       const vec2 G[9] = vec2[]( vec2(1.0,  1.0), vec2(0.0,  2.0), vec2(-1.0,  1.0), \n\
                                 vec2(2.0,  0.0), vec2(0.0,  0.0), vec2(-2.0,  0.0), \n\
                                 vec2(1.0, -1.0), vec2(0.0, -2.0), vec2(-1.0, -1.0) );\n\
       vec2 pas = vec2(1.0 / float(width - 1), 1.0 / float(height - 1));\n\
       vec2 offset[ossize] = vec2[](vec2(-pas.x , -pas.y), vec2( 0.0, -pas.y), vec2( pas.x , -pas.y), \n\
                                    vec2(-pas.x, 0.0),        vec2( 0.0, 0.0),     vec2( pas.x, 0.0), \n\
                                    vec2(-pas.x,   pas.y),  vec2( 0.0, pas.y), vec2( pas.x ,  pas.y) );\n\
       vec4 median(void) {\n\
         vec4 sample[ossize], c;\n\
         sample[0].rgb = texture(myTex, vsoTexCoord.st + offset[0]).rgb;\n\
         sample[0].a = dot(sample[0].rgb, sample[0].rgb);\n\
         for(int i = 1, j; i < ossize; i++) {\n\
           c.rgb = texture(myTex, vsoTexCoord.st + offset[i]).rgb;\n\
           c.a = dot(c.rgb, c.rgb);\n\
           for(j = 0; j < i; j++)\n\
             if(c.a > sample[j].a) break; // PUTAIN !!!!!!!\n\
           for(int k = i - 1; k >= j; k--)\n\
             sample[k + 1] = sample[k];\n\
           sample[j] = c;\n\
         }\n\
         return vec4(sample[ossize >> 1].rgb, 1.0);\n\
       }\n\
       void main(void) {\n\
         fragColor = median();\n\
       }";
    _medianPId = gl4duCreateProgram(gl4dfBasicVS, imfs, NULL);
    gl4duAtExit(quit);
  }
}

static void quit(void) {
  medianfptr = medianfinit;
}
