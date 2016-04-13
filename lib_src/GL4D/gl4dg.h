/*!\file gl4dg.h
 *
 * \brief The GL4Dummies Geometry
 *
 * \author Fares BELHADJ amsi@ai.univ-paris8.fr
 * \date February 22, 2016
 */

#ifndef _GL4DG_H
#define _GL4DG_H

#include "gl4dummies.h"

#ifdef __cplusplus
extern "C" {
#endif
  
  GL4DAPI void      GL4DAPIENTRY gl4dgInit(void);
  GL4DAPI void      GL4DAPIENTRY gl4dgClean(void);
  GL4DAPI void      GL4DAPIENTRY gl4dgSetGeometryOptimizationLevel(GLuint level);
  GL4DAPI GLuint    GL4DAPIENTRY gl4dgGetVAO(GLuint id);
  GL4DAPI GLuint    GL4DAPIENTRY gl4dgGenQuadf(void);
  GL4DAPI GLuint    GL4DAPIENTRY gl4dgGenCubef(void);
  GL4DAPI GLuint    GL4DAPIENTRY gl4dgGenSpheref(GLuint slices, GLuint stacks);
  GL4DAPI GLuint    GL4DAPIENTRY gl4dgGenConef(GLuint slices, GLboolean base);
  GL4DAPI GLuint    GL4DAPIENTRY gl4dgGenFanConef(GLuint slices, GLboolean base);
  GL4DAPI GLuint    GL4DAPIENTRY gl4dgGenCylinderf(GLuint slices, GLboolean base);
  GL4DAPI GLuint    GL4DAPIENTRY gl4dgGenDiskf(GLuint slices);
  GL4DAPI GLuint    GL4DAPIENTRY gl4dgGenTorusf(GLuint slices, GLuint stacks, GLfloat radius);
  GL4DAPI GLuint    GL4DAPIENTRY gl4dgGenGrid2df(GLuint width, GLuint height);
  GL4DAPI GLuint    GL4DAPIENTRY gl4dgGenGrid2dFromHeightMapf(GLuint width, GLuint height, GLfloat * heightmap);
  GL4DAPI void      GL4DAPIENTRY gl4dgDraw(GLuint id);
  GL4DAPI void      GL4DAPIENTRY gl4dgDelete(GLuint id);
  
#ifdef __cplusplus
}
#endif

#endif
