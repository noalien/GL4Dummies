/*!\file gl4df.h
 *
 * \brief The GL4Dummies filters : filtres de rendu de la bibliothèque
 * GL4Dummies.
 *
 * \author Farès BELHADJ amsi@ai.univ-paris8.fr
 * \date April 14, 2016
 * 
 */
#ifndef _GL4DF_H
#define _GL4DF_H

#include <GL4D/gl4dummies.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef enum GL4DFenum GL4DFenum;
  enum GL4DFenum {
    GL4DF_SOBEL_RESULT_RGB = 0,
    GL4DF_SOBEL_RESULT_INV_RGB,
    GL4DF_SOBEL_RESULT_LUMINANCE,
    GL4DF_SOBEL_RESULT_INV_LUMINANCE, /* par défault */
    GL4DF_SOBEL_MIX_NONE,
    GL4DF_SOBEL_MIX_ADD,
    GL4DF_SOBEL_MIX_MULT
  };

  /* Dans gl4dfBlur.c */
  GL4DAPI void GL4DAPIENTRY gl4dfBlur(GLuint in, GLuint out, GLuint radius, GLuint nb_iterations, GLuint weight, GLboolean flipV);
  /* Dans gl4dfSobel.c */
  GL4DAPI void GL4DAPIENTRY gl4dfSobel(GLuint in, GLuint out, GLboolean flipV);
  GL4DAPI void GL4DAPIENTRY gl4dfSobelSetResultMode(GL4DFenum mode);
  GL4DAPI void GL4DAPIENTRY gl4dfSobelSetMixMode(GL4DFenum mode);
  GL4DAPI void GL4DAPIENTRY gl4dfSobelSetMixFactor(GLfloat factor);

#ifdef __cplusplus
}
#endif

#endif
