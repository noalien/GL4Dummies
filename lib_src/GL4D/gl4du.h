/*!\file gl4du.h
 *
 * \brief The GL4Dummies Utilities
 *
 * \author Fares BELHADJ amsi@grafeet.fr (ou amsi@ai.univ-paris8.fr)
 * \date March 28, 2008
 */

#ifndef _GL4DU_H
#define _GL4DU_H

#include "gl4dummies.h"
#include "gl4dm.h"
#include "gl4dg.h"

#ifdef __cplusplus
extern "C" {
#endif
  
  enum GL4DUenum {
    GL4DU_VERTEX_SHADER   = 1,
    GL4DU_FRAGMENT_SHADER = 2,
    GL4DU_GEOMETRY_SHADER = 4,
    GL4DU_MATRIX          = 1024, /* les data de la matrice */
    GL4DU_MATRIX_TYPE     = 1025,
#if defined(_MSC_VER) /* ENUM n'est que 32bits sous MSC !!! */
    GL4DU_SHADER          = 1 << 20,
    GL4DU_PROGRAM         = 1 << 21,
    GL4DU_MATRICES        = 1 << 22,
    GL4DU_GEOMETRY        = 1 << 23,
    GL4DU_DEMO_HELPER     = 1 << 24,
    GL4DU_AT_EXIT         = 1 << 25,
    GL4DU_ALL             = 0xffffffff
#else
    GL4DU_SHADER          = ((unsigned long long)1) << 32,
    GL4DU_PROGRAM         = ((unsigned long long)1) << 33,
    GL4DU_MATRICES        = ((unsigned long long)1) << 34,
    GL4DU_GEOMETRY        = ((unsigned long long)1) << 35,
    GL4DU_DEMO_HELPER     = ((unsigned long long)1) << 36,
    GL4DU_AT_EXIT         = ((unsigned long long)1) << 36,
    GL4DU_ALL             = (unsigned long long)0xffffffffffffffffLL
#endif
  };
  typedef enum GL4DUenum GL4DUenum;
  
  GL4DAPI void      GL4DAPIENTRY gl4duInit(int argc, char ** argv);
  GL4DAPI void      GL4DAPIENTRY gl4duMakeBinRelativePath(char * dst, size_t dst_size, const char * filename);
  GL4DAPI void      GL4DAPIENTRY gl4duPrintShaderInfoLog(GLuint object, FILE * f);
  GL4DAPI void      GL4DAPIENTRY gl4duPrintProgramInfoLog(GLuint object, FILE * f);
  GL4DAPI void      GL4DAPIENTRY gl4duPrintFPS(FILE * fp);
  GL4DAPI GLuint    GL4DAPIENTRY gl4duCreateShader(GLenum shadertype, const char * filename);
  GL4DAPI GLuint    GL4DAPIENTRY gl4duCreateShaderIM(GLenum shadertype, const char * filename, const char * shadercode);
  GL4DAPI GLuint    GL4DAPIENTRY gl4duCreateShaderFED(const char * decData, GLenum shadertype, const char * filename);
  GL4DAPI GLuint    GL4DAPIENTRY gl4duFindShader(const char * filename);
  GL4DAPI void      GL4DAPIENTRY gl4duDeleteShader(GLuint id);
  GL4DAPI GLuint    GL4DAPIENTRY gl4duCreateProgram(const char * firstone, ...);
  GL4DAPI GLuint    GL4DAPIENTRY gl4duCreateProgramFED(const char * encData, const char * firstone, ...);
  GL4DAPI void      GL4DAPIENTRY gl4duDeleteProgram(GLuint id);
  GL4DAPI void      GL4DAPIENTRY gl4duCleanUnattached(GL4DUenum what);
  GL4DAPI void      GL4DAPIENTRY gl4duAtExit(void (*func)(void));
  GL4DAPI void      GL4DAPIENTRY gl4duClean(GL4DUenum what);
  GL4DAPI int       GL4DAPIENTRY gl4duUpdateShaders(void);
  GL4DAPI GLboolean GL4DAPIENTRY gl4duGenMatrix(GLenum type, const char * name);
  GL4DAPI GLboolean GL4DAPIENTRY gl4duIsMatrix(const char * name);
  GL4DAPI GLboolean GL4DAPIENTRY gl4duBindMatrix(const char * name);
  GL4DAPI GLboolean GL4DAPIENTRY gl4duDeleteMatrix(const char * name);
  GL4DAPI void      GL4DAPIENTRY gl4duPushMatrix(void);
  GL4DAPI void      GL4DAPIENTRY gl4duPopMatrix(void);
  GL4DAPI void      GL4DAPIENTRY gl4duSendMatrix(void);
  GL4DAPI void      GL4DAPIENTRY gl4duSendMatrices(void);
  GL4DAPI void      GL4DAPIENTRY gl4duFrustumf(GLfloat  l, GLfloat  r, GLfloat  b, GLfloat  t, GLfloat  n, GLfloat  f);
  GL4DAPI void      GL4DAPIENTRY gl4duFrustumd(GLdouble l, GLdouble r, GLdouble b, GLdouble t, GLdouble n, GLdouble f);
  GL4DAPI void      GL4DAPIENTRY gl4duOrthof(GLfloat  l, GLfloat  r, GLfloat  b, GLfloat  t, GLfloat  n, GLfloat  f);
  GL4DAPI void      GL4DAPIENTRY gl4duOrthod(GLdouble l, GLdouble r, GLdouble b, GLdouble t, GLdouble n, GLdouble f);
/* Vince */
  GL4DAPI void      GL4DAPIENTRY gl4duPerspectivef(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);
  GL4DAPI void      GL4DAPIENTRY gl4duPerspectived(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);
/* Vince */
  GL4DAPI void      GL4DAPIENTRY gl4duLoadIdentityf(void);
  GL4DAPI void      GL4DAPIENTRY gl4duLoadIdentityd(void);
  GL4DAPI void      GL4DAPIENTRY gl4duLoadMatrixf(const GLfloat * matrix);
  GL4DAPI void      GL4DAPIENTRY gl4duLoadMatrixd(const GLdouble * matrix);
  GL4DAPI void      GL4DAPIENTRY gl4duMultMatrixf(const GLfloat * matrix);
  GL4DAPI void      GL4DAPIENTRY gl4duMultMatrixd(const GLdouble * matrix);
  GL4DAPI void      GL4DAPIENTRY gl4duRotatef(GLfloat  angle, GLfloat x, GLfloat y, GLfloat z);
  GL4DAPI void      GL4DAPIENTRY gl4duRotated(GLdouble angle, GLdouble x, GLdouble y, GLdouble z);
  GL4DAPI void      GL4DAPIENTRY gl4duTranslatef(GLfloat tx, GLfloat ty, GLfloat tz);
  GL4DAPI void      GL4DAPIENTRY gl4duTranslated(GLdouble tx, GLdouble ty, GLdouble tz);
  GL4DAPI void      GL4DAPIENTRY gl4duScalef(GLfloat sx, GLfloat sy, GLfloat sz);
  GL4DAPI void      GL4DAPIENTRY gl4duScaled(GLdouble sx, GLdouble sy, GLdouble sz);
  GL4DAPI void      GL4DAPIENTRY gl4duLookAtf(GLfloat eyeX, GLfloat eyeY, GLfloat eyeZ, GLfloat centerX, GLfloat centerY, GLfloat centerZ, GLfloat upX, GLfloat upY, GLfloat upZ);
  GL4DAPI void      GL4DAPIENTRY gl4duLookAtd(GLdouble eyeX, GLdouble eyeY, GLdouble eyeZ, GLdouble centerX, GLdouble centerY, GLdouble centerZ, GLdouble upX, GLdouble upY, GLdouble upZ);
  GL4DAPI void *    GL4DAPIENTRY gl4duGetMatrixData(void);
  GL4DAPI GLboolean GL4DAPIENTRY gl4duGetIntegerv(GL4DUenum pname, GLint * params);
  
#ifdef __cplusplus
}
#endif

#endif
