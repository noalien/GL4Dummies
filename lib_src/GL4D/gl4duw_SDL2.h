/*!\file gl4duw_SDL2.h
 *
 * \brief Fonctions haut-niveau de GL4Dummies pour la gestion des fenêtres avec SDL2
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date Februry 01 2016
 */

#ifndef _GL4DUW_SDL2_H
#define _GL4DUW_SDL2_H

#include "gl4du.h"
#include <SDL.h>
#include <SDL_opengl.h>

#ifdef __cplusplus
extern "C" {
#endif
  
GL4DAPI void GL4DAPIENTRY gl4duwSetGLAttributes(int glMajorVersion, int glMinorVersion, int glProfileMask, int glDoubleBuffer, int glDepthSize);
GL4DAPI int  GL4DAPIENTRY gl4duwCreateWindow(int argc, char ** argv, const char * title, int x, int y, int width, int height, Uint32 wflags);
GL4DAPI void GL4DAPIENTRY gl4duwMainLoop(void);
GL4DAPI void GL4DAPIENTRY gl4duwResizeFunc(void (*func)(int, int));
GL4DAPI void GL4DAPIENTRY gl4duwKeyDownFunc(void (*func)(int));
GL4DAPI void GL4DAPIENTRY gl4duwKeyUpFunc(void (*func)(int));
GL4DAPI void GL4DAPIENTRY gl4duwIdleFunc(void (*func)(void));
GL4DAPI void GL4DAPIENTRY gl4duwDisplayFunc(void (*func)(void));

#ifdef __cplusplus
}
#endif

#endif
