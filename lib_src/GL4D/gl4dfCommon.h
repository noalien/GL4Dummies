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

#ifdef __cplusplus
extern "C" {
#endif

  static const char * gl4dfBasicVS = 
    "<imvs>gl4dfBasic.vs</imvs>\n\
     #version 330\n						\
     layout (location = 0) in vec3 vsiPosition;\n		\
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

  extern GLuint fcommGetTempTex(GLuint i);
  extern GLuint fcommGetPlane(void);
  extern GLuint fcommGetFBO(void);

#ifdef __cplusplus
}
#endif

#endif
