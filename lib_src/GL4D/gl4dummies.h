/*!\file gl4dummies.h
 *
 * \brief gestion simplifiée de GL sous différents OS et fonctionnalité de cryptage des shaders.
 *
 * \author Farès BELHADJ amsi@grafeet.fr (ou amsi@ai.univ-paris8.fr)
 * \date March 28, 2008 - February 07, 2014
*/

#ifndef _GL4DUMMIES_H
#define _GL4DUMMIES_H

/****************************************************/
/********* Gestion des cdecl & des declspec *********/
/****************************************************/
#if !defined(_MSC_VER) && !defined(__cdecl)
    /* Define __cdecl for non-Microsoft compilers. */
#   define __cdecl
#   define __declspec(type)
#endif
#ifndef _CRTIMP
#   ifdef _NTSDK
       /* Definition compatible with NT SDK */
#      define _CRTIMP
#   else
       /* Current definition */
#      ifdef _DLL
#         define _CRTIMP __declspec(dllimport)
#      else
#         define _CRTIMP
#      endif
#   endif
#endif

/****************************************************/
/********* Gestion des modes import/(export) ********/
/****************************************************/
#if !defined(DLLSPEC) && !defined(GL4dummies_EXPORTS)
#   define _DLLSPEC dllimport
#else
#   define _DLLSPEC dllexport
#endif

/****************************************************/
/********* Gestion des inline ***********************/
/****************************************************/
#ifndef INLINE
#  ifdef __cplusplus
#    define INLINE inline
#  else
#    define INLINE static inline
#  endif
#endif

/****************************************************/
/**** Gestion des AUTRES spécificité MS_VStudio *****/
/****************************************************/
#if defined(_MSC_VER)
#pragma warning(disable:4244)
#  ifndef _CRT_SECURE_NO_WARNINGS
#    define _CRT_SECURE_NO_WARNINGS
#  endif
#  ifndef _CRT_NONSTDC_NO_DEPRECATE
#    define _CRT_NONSTDC_NO_DEPRECATE
#  endif
#  ifndef inline
#    define inline __inline
#  endif
#  ifndef __func__
#    define __func__ "FUNC"
#  endif
#  ifndef snprintf
#    define snprintf sprintf_s
#  endif
#  ifndef ALL_IN_ONE
#    define GL4DAPI extern __declspec(_DLLSPEC)
#    define GL4DAPIENTRY __cdecl
#  else
#    define GL4DAPI
#    define GL4DAPIENTRY
#  endif
#  define strcasecmp( s1, s2 ) strcmpi( s1, s2 )
#  define strncasecmp( s1, s2, n ) strnicmp( s1, s2, n )
#else
#  define GL4DAPI extern
#  define GL4DAPIENTRY
#endif

/************************************************/
/**************** Partie Liée à GL **************/
/************************************************/

#ifndef GL_GLEXT_PROTOTYPES
#  define GL_GLEXT_PROTOTYPES
#endif

#ifdef __ANDROID__
#define __GL4D_ES2__
#endif

#ifdef __ANDROID__
#include <android/log.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
typedef double GLdouble;
#define GL_DOUBLE GL_FLOAT
#else
#include <SDL.h>
#include <SDL_opengl.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#if defined(_WIN32)
#include "gl4wdummies.h"
#endif

#ifdef __GL4D_ES2__
#define GL4D_VAO_INDEX GL_UNSIGNED_SHORT
typedef GLushort GL4Dvaoindex;
#else
#define GL4D_VAO_INDEX GL_UNSIGNED_INT
typedef GLuint GL4Dvaoindex;
#endif

#ifdef __cplusplus
extern "C" {
#endif

GL4DAPI int       GL4DAPIENTRY gl4dExtensionSupported(const char * extension);
GL4DAPI void      GL4DAPIENTRY gl4dQuit(void);
GL4DAPI char *    GL4DAPIENTRY gl4dReadTextFile(const char * filename);
GL4DAPI char *    GL4DAPIENTRY gl4dExtractFromDecData(const char * decData, const char * filename);
GL4DAPI void      GL4DAPIENTRY gl4dInitTime0(void);
GL4DAPI double    GL4DAPIENTRY gl4dGetElapsedTime(void);
GL4DAPI void      GL4DAPIENTRY gl4dInitTime(void);
GL4DAPI double    GL4DAPIENTRY gl4dGetTime(void);
GL4DAPI double    GL4DAPIENTRY gl4dGetFps(void);

GL4DAPI int       GL4DAPIENTRY mv(const char * src, const char * dst);
GL4DAPI char *    GL4DAPIENTRY pathOf(const char * path);
GL4DAPI char *    GL4DAPIENTRY filenameOf(const char * path);


#ifdef __cplusplus
}
#endif

#define MKFWINIT0(prefix, ret)	  \
static ret prefix ## finit(void); \
static ret prefix ## ffunc(void); \
static ret (*prefix ## fptr) (void) = prefix ## finit

#define MKFWINIT1(prefix, ret, t1) \
static ret prefix ## finit(t1); \
static ret prefix ## ffunc(t1); \
static ret (*prefix ## fptr) (t1) = prefix ## finit
  
#define MKFWINIT2(prefix, ret, t1, t2) \
static ret prefix ## finit(t1, t2); \
static ret prefix ## ffunc(t1, t2); \
static ret (*prefix ## fptr) (t1, t2) = prefix ## finit
  
#define MKFWINIT3(prefix, ret, t1, t2, t3) \
static ret prefix ## finit(t1, t2, t3); \
static ret prefix ## ffunc(t1, t2, t3); \
static ret (*prefix ## fptr) (t1, t2, t3) = prefix ## finit  

#define MKFWINIT4(prefix, ret, t1, t2, t3, t4) \
static ret prefix ## finit(t1, t2, t3, t4); \
static ret prefix ## ffunc(t1, t2, t3, t4); \
static ret (*prefix ## fptr) (t1, t2, t3, t4) = prefix ## finit  

#define MKFWINIT5(prefix, ret, t1, t2, t3, t4, t5) \
static ret prefix ## finit(t1, t2, t3, t4, t5); \
static ret prefix ## ffunc(t1, t2, t3, t4, t5); \
static ret (*prefix ## fptr) (t1, t2, t3, t4, t5) = prefix ## finit  

#define MKFWINIT6(prefix, ret, t1, t2, t3, t4, t5, t6) \
static ret prefix ## finit(t1, t2, t3, t4, t5, t6); \
static ret prefix ## ffunc(t1, t2, t3, t4, t5, t6); \
static ret (*prefix ## fptr) (t1, t2, t3, t4, t5, t6) = prefix ## finit  

#define MKFWINIT7(prefix, ret, t1, t2, t3, t4, t5, t6, t7) \
static ret prefix ## finit(t1, t2, t3, t4, t5, t6, t7); \
static ret prefix ## ffunc(t1, t2, t3, t4, t5, t6, t7); \
static ret (*prefix ## fptr) (t1, t2, t3, t4, t5, t6, t7) = prefix ## finit  

#define MKFWINIT8(prefix, ret, t1, t2, t3, t4, t5, t6, t7, t8) \
static ret prefix ## finit(t1, t2, t3, t4, t5, t6, t7, t8); \
static ret prefix ## ffunc(t1, t2, t3, t4, t5, t6, t7, t8); \
static ret (*prefix ## fptr) (t1, t2, t3, t4, t5, t6, t7, t8) = prefix ## finit  

#define MKFWINIT9(prefix, ret, t1, t2, t3, t4, t5, t6, t7, t8, t9) \
static ret prefix ## finit(t1, t2, t3, t4, t5, t6, t7, t8, t9); \
static ret prefix ## ffunc(t1, t2, t3, t4, t5, t6, t7, t8, t9); \
static ret (*prefix ## fptr) (t1, t2, t3, t4, t5, t6, t7, t8, t9) = prefix ## finit  

#endif
