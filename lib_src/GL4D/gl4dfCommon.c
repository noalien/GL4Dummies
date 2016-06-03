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
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "gl4du.h"
#include "gl4df.h"
#include "gl4dfCommon.h"

static GLuint _tempTId[4] = {0}, _nbTempTex = sizeof _tempTId / sizeof *_tempTId, _plan = 0, _fbo = 0;
static void init(void);
static void quit(void);

MKFWINIT1(temptex, GLuint, GLuint);
MKFWINIT0(plane, GLuint);
MKFWINIT0(fbo, GLuint);

static void init(void) {
  if(gl4dfBasicVS); /* éviter des warnings stupides */
  if(!_plan)
    _plan = gl4dgGenQuadf();
  if(!_fbo)
    glGenFramebuffers(1, &_fbo);
  if(!_tempTId[0]) {
    int i;
    glGenTextures(_nbTempTex, _tempTId);
    for(i = 0; i < _nbTempTex; i++) {
      glBindTexture(GL_TEXTURE_2D, _tempTId[i]);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    gl4duAtExit(quit);
  }
  temptexptr = temptexffunc;
  planeptr = planeffunc;
  fboptr = fboffunc;
}

static void quit(void) {
  if(_tempTId[0]) {
    int i;
    glDeleteTextures(_nbTempTex, _tempTId);
    for(i = 0; i < _nbTempTex; i++)
      _tempTId[i] = 0;
  }
  if(_fbo) {
    glDeleteFramebuffers(1, &_fbo);
    _fbo = 0;
  }
  temptexptr = temptexfinit;
  planeptr = planefinit;
  fboptr = fbofinit;
}

GLuint fcommGetTempTex(GLuint i) {
  return temptexptr(i);
}

GLuint fcommGetPlane(void) {
  return planeptr();
}

GLuint fcommGetFBO(void) {
  return fboptr();
}

static GLuint temptexfinit(GLuint i) {
  init();
  return temptexptr(i);
}

static GLuint planefinit(void) {
  init();
  return planeptr();
}

static GLuint fbofinit(void) {
  init();
  return fboptr();
}

static GLuint temptexffunc(GLuint i) {
  return i < _nbTempTex ? _tempTId[i] : 0;
}

static GLuint planeffunc(void) {
  return _plan;
}

static GLuint fboffunc(void) {
  return _fbo;
}
