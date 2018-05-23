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
//#define __GL4D_ES__
#endif

#ifdef __ANDROID__
#define __GLES4D__
#include <android/log.h>

#if __ANDROID_API__ >= 24
#include <GLES3/gl32.h>
#elif __ANDROID_API__ >= 21
#include <GLES3/gl31.h>
#else
#include <GLES3/gl3.h>
#endif

typedef double GLdouble;
typedef unsigned long Uint32;
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

//#ifdef __GLES4D__
//#define GL4D_VAO_INDEX GL_UNSIGNED_SHORT
//typedef GLushort GL4Dvaoindex;
//#else
#define GL4D_VAO_INDEX GL_UNSIGNED_INT
typedef GLuint GL4Dvaoindex;
//#endif

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

#ifdef __GLES4D__

#  ifndef GL_POLYGON_MODE
#    define GL_POLYGON_MODE 0x0B40
#  endif
#  ifndef GL_FILL
#    define GL_FILL 0x1B02
#  endif
#  ifndef GL_TEXTURE_WIDTH
#    define GL_TEXTURE_WIDTH                  0x1000
#  endif
#  ifndef GL_TEXTURE_HEIGHT
#    define GL_TEXTURE_HEIGHT                 0x1001
#  endif

#  define fprintf(file, ...) do {\
    if(file == stdout)\
        __android_log_print(ANDROID_LOG_VERBOSE, "GL4D", __VA_ARGS__);\
    else if(file == stderr)\
        __android_log_print(ANDROID_LOG_ERROR, "GL4D", __VA_ARGS__);\
    else\
        fprintf(file, __VA_ARGS__);\
} while(0)
#  include <assert.h>
#  include <string.h>
#  include <stdlib.h>
typedef struct _es30_tex_info_t _es30_tex_info_t;
struct _es30_tex_info_t {
    GLint id;
    GLint width;
    GLint height;
};
extern _es30_tex_info_t * _tex_info;
extern GLint _tex_info_size;
static void _free_tex_info(void) {
    if(_tex_info) {
        free(_tex_info);
        _tex_info = NULL;
    }
    _tex_info_size = 0;
}
inline static void glGetTexLevelParameteriv_ES30(GLenum target, GLint level, GLenum pname, GLint * params) {
    GLint ctex;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &ctex);
    if(_tex_info == NULL || ctex >= _tex_info_size || _tex_info[ctex].id == 0) {
        fprintf(stderr, "Unknown texture info for current texture\n");
        return;
    }
    if(pname == GL_TEXTURE_WIDTH)
        *params = _tex_info[ctex].width;
    else if(pname == GL_TEXTURE_HEIGHT)
        *params = _tex_info[ctex].height;
}
void gl4duAtExit(void (*func)(void));
inline static void glTexImage2D_ES30(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid * data) {
    GLint ctex;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &ctex);
    if(_tex_info == NULL) {
        _tex_info_size = ctex + 1;
        _tex_info = (_es30_tex_info_t *)calloc(_tex_info_size, sizeof *_tex_info);
        assert(_tex_info);
        gl4duAtExit(_free_tex_info);
    } else if(ctex >= _tex_info_size) {
        _tex_info = (_es30_tex_info_t *)realloc(_tex_info, (ctex + 1) * sizeof *_tex_info);
        assert(_tex_info);
        memset(&_tex_info[_tex_info_size], 0, (ctex - _tex_info_size + 1) * sizeof *_tex_info);
        _tex_info_size = ctex + 1;
    }
    _tex_info[ctex].id = ctex;
    _tex_info[ctex].width = width;
    _tex_info[ctex].height = height;
    glTexImage2D(target, level, internalFormat, width, height, border, format, type, data);
}
inline static void glPolygonMode(int a, int b) {}
#  define glGetTexLevelParameteriv glGetTexLevelParameteriv_ES30
#  define glTexImage2D glTexImage2D_ES30
#endif


#define GL4D_GL_ERROR do {\
    GLint err = glGetError();\
    if (err != GL_NO_ERROR)\
        fprintf(stderr, "GL error at %s/%s()/%d: 0x%08x\n", __FILE__, __func__, __LINE__, err);\
    } while(0)


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
