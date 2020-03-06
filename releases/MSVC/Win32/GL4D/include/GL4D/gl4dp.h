/*!\file gl4dp.h
 *
 * \brief Primitives de dessin 2D et de manipulation de couleurs
 *
 * A été étendue pour s'intégrer à la bibliothèque demoHelper.
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date March 14 2014
 */

#ifndef _DEMO_HELPER_PRIMITIVES_H

#define _DEMO_HELPER_PRIMITIVES_H

#include "gl4du.h"

#if SDL_BYTEORDER == SDL_BIGENDIAN 
/*!\brief décalage pour la composante rouge */
#  define R_SHIFT 24
/*!\brief décalage pour la composante verte */
#  define G_SHIFT 16
/*!\brief décalage pour la composante bleue */
#  define B_SHIFT  8
/*!\brief décalage pour la composante alpha */
#  define A_SHIFT  0
/*!\brief masque pour la composante rouge */
#  define R_MASK 0xff000000
/*!\brief masque pour la composante verte */
#  define G_MASK 0x00ff0000
/*!\brief masque pour la composante bleue */
#  define B_MASK 0x0000ff00
/*!\brief masque pour la composante alpha */
#  define A_MASK 0x000000ff
#else
/*!\brief décalage pour la composante rouge */
#  define R_SHIFT  0
/*!\brief décalage pour la composante verte */
#  define G_SHIFT  8
/*!\brief décalage pour la composante bleue */
#  define B_SHIFT 16
/*!\brief décalage pour la composante alpha */
#  define A_SHIFT 24
/*!\brief masque pour la composante rouge */
#  define R_MASK 0x000000ff
/*!\brief masque pour la composante verte */
#  define G_MASK 0x0000ff00
/*!\brief masque pour la composante bleue */
#  define B_MASK 0x00ff0000
/*!\brief masque pour la composante alpha */
#  define A_MASK 0xff000000
#endif
/*!\brief macro qui convertie un r, un g, un b et un a en couleur Uint32 rgba */
#define RGBA(r, g, b, a) (((Uint32)(Uint8)(r)) << R_SHIFT |	\
			  ((Uint32)(Uint8)(g)) << G_SHIFT |	\
			  ((Uint32)(Uint8)(b)) << B_SHIFT |	\
			  ((Uint32)(Uint8)(a)) << A_SHIFT )
/*!\brief macro qui convertie un r, un g et un b en couleur Uint32 rgba dont l'alpha est à 255 */
#ifdef RGB //FOR MSVC
#undef RGB
#endif
#define RGB(r, g, b) RGBA(r, g, b, 255)
/*!\brief macro permettant d'extraire une composante en utilisant l'argument \a shift */
#define EXTRACT_COMP(coul, shift) ((((Uint32)(coul)) >> (shift)) & 0xFF)
/*!\brief macro permettant d'extraire une composante rouge de l'Uint32 \a coul */
#define   RED(coul) EXTRACT_COMP(coul, R_SHIFT)
/*!\brief macro permettant d'extraire une composante verte de l'Uint32 \a coul */
#define GREEN(coul) EXTRACT_COMP(coul, G_SHIFT)
/*!\brief macro permettant d'extraire une composante bleue de l'Uint32 \a coul */
#define  BLUE(coul) EXTRACT_COMP(coul, B_SHIFT)
/*!\brief macro permettant d'extraire une composante alpha de l'Uint32 \a coul */
#define ALPHA(coul) EXTRACT_COMP(coul, A_SHIFT)
/*!\brief macro pour tester si x et y sont dans limites de l'ecran */
#define IN_SCREEN(x, y) ((x) >= 0 && (x) < (int)gl4dpGetWidth() && (y) >= 0 && (y) < (int)gl4dpGetHeight())

#ifdef __cplusplus
extern "C" {
#endif

  GL4DAPI Uint32    GL4DAPIENTRY gl4dpGetColor(void);
  GL4DAPI void      GL4DAPIENTRY gl4dpSetColor(Uint32 color);
  GL4DAPI GLuint    GL4DAPIENTRY gl4dpInitScreenWithDimensions(GLuint w, GLuint h);
  GL4DAPI GLuint    GL4DAPIENTRY gl4dpInitScreen(void);
  GL4DAPI int       GL4DAPIENTRY gl4dpSetScreen(GLuint id);
  GL4DAPI void      GL4DAPIENTRY gl4dpDeleteScreen(void);
  GL4DAPI GLuint    GL4DAPIENTRY gl4dpGetTextureId(void);
  GL4DAPI GLuint    GL4DAPIENTRY gl4dpGetWidth(void);
  GL4DAPI GLuint    GL4DAPIENTRY gl4dpGetHeight(void);
  GL4DAPI void      GL4DAPIENTRY gl4dpClearScreen(void);
  GL4DAPI void      GL4DAPIENTRY gl4dpClearScreenWith(Uint32 color);
  GL4DAPI void      GL4DAPIENTRY gl4dpUpdateScreen(GLint * rect);
  GL4DAPI void      GL4DAPIENTRY gl4dpRect(GLint * rect);
  GL4DAPI void      GL4DAPIENTRY gl4dpLine(int x0, int y0, int x1, int y1);
  GL4DAPI void      GL4DAPIENTRY gl4dpCircle(int x0, int y0, int r);
  GL4DAPI Uint32    GL4DAPIENTRY gl4dpGetPixel(int x, int y);
  GL4DAPI void      GL4DAPIENTRY gl4dpPutPixel(int x, int y);
  GL4DAPI void      GL4DAPIENTRY gl4dpHLine(int x0, int x1, int y);
  GL4DAPI void      GL4DAPIENTRY gl4dpFilledCircle(int x0, int y0, int r);
  GL4DAPI void      GL4DAPIENTRY gl4dpCopyFromSDLSurfaceWithTransforms(SDL_Surface * s, const GLfloat scale[2], const GLfloat translate[2]);
  GL4DAPI void      GL4DAPIENTRY gl4dpCopyFromSDLSurface(SDL_Surface * s);
  GL4DAPI GLfloat * GL4DAPIENTRY gl4dpSDLSurfaceToLuminanceMap(SDL_Surface * s);
  GL4DAPI void      GL4DAPIENTRY gl4dpMap(GLuint dstSId, GLuint srcSId, const GLfloat pRect[4], const GLfloat tRect[4], GLfloat rotation);

#ifdef __cplusplus
}
#endif

#endif
