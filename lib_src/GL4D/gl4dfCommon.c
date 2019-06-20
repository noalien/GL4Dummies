/*!\file gl4dfCommon.c
 *
 * \brief The GL4Dummies filters : éléments communs pour les fichiers gl4dfxxx.c
 *
 * A usage interne à la lib.
 *
 * \author Farès BELHADJ amsi@ai.univ-paris8.fr
 * \date May 27, 2016
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include "gl4du.h"
#include "gl4df.h"
#include "gl4dfCommon.h"

static GLuint _plan = 0;
static void init(void);
static void quit(void);

MKFWINIT0(plane, GLuint);

static void init(void) {
  GLint ctex;
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &ctex);
  if(gl4dfBasicVS) {
    (void)ctex; /* avoid stupid warning */
  }

  if(!_plan) {
    _plan = gl4dgGenQuadf();
    gl4duAtExit(quit);
  }
  planefptr = planeffunc;
}

static void quit(void) {
  if(_plan) {
    gl4dgDelete(_plan);
    _plan = 0;
  }
  planefptr = planefinit;
}

void fcommMatchTex(GLuint goal, GLuint orig) {
  GLint vp[4], w, h, pw, ph, ctex;
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &ctex);
  if(orig) {
    glBindTexture(GL_TEXTURE_2D, orig);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
  } else {
    glGetIntegerv(GL_VIEWPORT, vp);
    w = vp[2];// - vp[0];
    h = vp[3];// - vp[1];
  }
  glBindTexture(GL_TEXTURE_2D, goal);
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &pw);
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &ph);
  if(pw != w || ph != h)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glBindTexture(GL_TEXTURE_2D, (GLuint)ctex);
}

GLuint fcommGetPlane(void) {
  return planefptr();
}

static GLuint planefinit(void) {
  init();
  return planefptr();
}

static GLuint planeffunc(void) {
  return _plan;
}
