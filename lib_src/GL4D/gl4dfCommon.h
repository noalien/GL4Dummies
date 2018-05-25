/*!\file gl4dfCommon.h
 *
 * \brief The GL4Dummies filters : éléments communs pour les fichiers gl4dfxxx.c
 *
 * A usage interne à la lib.
 *
 * \author Farès BELHADJ amsi@ai.univ-paris8.fr
 * \date April 14, 2016
 * 
 */
#ifndef _GL4DFCOMMON_H
#define _GL4DFCOMMON_H

#pragma GCC diagnostic ignored "-Wunused-variable"

#ifdef __cplusplus
extern "C" {
#endif

  static const char * gl4dfBasicVS = 
    "<imvs>gl4dfBasic.vs</imvs>\n"
#ifdef __GLES4D__
    "#version 300 es\n"
#else
    "#version 330\n"
#endif
    "layout (location = 0) in vec3 vsiPosition;\n		\
     layout (location = 1) in vec3 vsiNormal;\n			\
     layout (location = 2) in vec2 vsiTexCoord;\n		\
     uniform int inv;\n						\
     out vec2 vsoTexCoord;\n						\
     void main(void) {\n						\
       gl_Position = vec4(vsiPosition, 1.0);\n				\
       if(inv != 0)\n							\
         vsoTexCoord = vec2(vsiTexCoord.s, 1.0 - vsiTexCoord.t);\n	\
       else\n								\
         vsoTexCoord = vec2(vsiTexCoord.s, vsiTexCoord.t);\n		\
     }";

  static const char * gl4dfTexFS = 
    "<imfs>gl4dfTex.fs</imfs>\n\
     #version 330\n							\
     uniform sampler2D tex;\n						\
     in vec2 vsoTexCoord;\n						\
     out vec4 fragColor;\n						\
     void main(void) {\n						\
         fragColor = texture(tex, vsoTexCoord);\n			\
     }";

  extern void   fcommMatchTex(GLuint goal, GLuint orig);
  extern GLuint fcommGetPlane(void);


#ifdef __cplusplus
}
#endif

#endif
