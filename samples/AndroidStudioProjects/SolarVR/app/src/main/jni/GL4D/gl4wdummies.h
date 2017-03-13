/*!\file gl4wdummies.h
 *
 * \brief Partie ne concernant que windows de gl4dummies, ici se trouvent
 *  des wrappers vers les fonctions n'existant pas dans opengl32
 *
 * \author Farès BELHADJ amsi@grafeet.fr (ou amsi@ai.univ-paris8.fr)
 * \date March 28, 2008 - February 07, 2014
*/

#ifndef _GL4WDUMMIES_H
#define _GL4WDUMMIES_H

#include "gl4dummies.h"

#if defined(_WIN32)

/*********** TESTES D'EXISTANCE DE CERTAINES MACROS ***********/
    #ifndef GL_GEOMETRY_SHADER
    #  define GL_GEOMETRY_SHADER GL_GEOMETRY_SHADER_ARB
    #endif
    #ifndef GL_INFO_LOG_LENGTH
    #  define GL_INFO_LOG_LENGTH		0x8B84
    #endif
    #ifndef GL_FRAGMENT_SHADER
    #  define GL_FRAGMENT_SHADER		0x8B30
    #endif
    #ifndef GL_VERTEX_SHADER
    #  define GL_VERTEX_SHADER	        0x8B31
    #endif
    #ifndef GL_GEOMETRY_SHADER_ARB
    #  define GL_GEOMETRY_SHADER_ARB	0x8DD9
    #endif
    #ifndef GL_CURRENT_PROGRAM
    #  define GL_CURRENT_PROGRAM		0x8B8D
    #endif
    #ifndef GL_SHADING_LANGUAGE_VERSION
    #  define GL_SHADING_LANGUAGE_VERSION 0x8B8C
    #endif
    #ifndef GL_COLOR_ATTACHMENT0
    #  define GL_COLOR_ATTACHMENT0              0x8CE0
    #  define GL_COLOR_ATTACHMENT1              0x8CE1
    #  define GL_COLOR_ATTACHMENT2              0x8CE2
    #  define GL_COLOR_ATTACHMENT3              0x8CE3
    #  define GL_COLOR_ATTACHMENT4              0x8CE4
    #  define GL_COLOR_ATTACHMENT5              0x8CE5
    #  define GL_COLOR_ATTACHMENT6              0x8CE6
    #  define GL_COLOR_ATTACHMENT7              0x8CE7
    #  define GL_COLOR_ATTACHMENT8              0x8CE8
    #  define GL_COLOR_ATTACHMENT9              0x8CE9
    #  define GL_COLOR_ATTACHMENT10             0x8CEA
    #  define GL_COLOR_ATTACHMENT11             0x8CEB
    #  define GL_COLOR_ATTACHMENT12             0x8CEC
    #  define GL_COLOR_ATTACHMENT13             0x8CED
    #  define GL_COLOR_ATTACHMENT14             0x8CEE
    #  define GL_COLOR_ATTACHMENT15             0x8CEF
    #endif
    #ifndef GL_RGBA32F
    #define GL_RGBA32F                          0x8814
    #define GL_RGBA32UI                         0x8D70
    #define GL_RGBA32I                          0x8D82
    #define GL_RGBA32F_ARB                      0x8814
    #define GL_RGBA32UI_EXT                     0x8D70
    #define GL_RGBA32I_EXT                      0x8D82
    #endif

    #ifndef GL_DRAW_FRAMEBUFFER
    #  define GL_DRAW_FRAMEBUFFER               0x8CA9
    #  define GL_DRAW_FRAMEBUFFER_EXT           0x8CA9
    #endif
    #ifndef GL_READ_FRAMEBUFFER
    #  define GL_READ_FRAMEBUFFER               0x8CA8
    #  define GL_READ_FRAMEBUFFER_EXT           0x8CA8
    #endif
    #ifndef GL_FRAMEBUFFER
    #  define GL_FRAMEBUFFER                    0x8D40
    #  define GL_FRAMEBUFFER_EXT                0x8D40
    #endif
    #ifndef GL_DEPTH_ATTACHMENT
    #  define GL_DEPTH_ATTACHMENT               0x8D00
    #  define GL_DEPTH_ATTACHMENT_EXT           0x8D00
    #endif
    #ifndef GL_DEPTH_COMPONENT16
    #  define GL_DEPTH_COMPONENT16              0x81A5
    #  define GL_DEPTH_COMPONENT16_ARB          0x81A5
    #endif
    #ifndef GL_ARRAY_BUFFER
    #  define GL_ARRAY_BUFFER                   0x8892
    #  define GL_ARRAY_BUFFER_ARB               0x8892
    #endif
    #ifndef GL_ELEMENT_ARRAY_BUFFER
    #  define GL_ELEMENT_ARRAY_BUFFER           0x8893
    #  define GL_ELEMENT_ARRAY_BUFFER_ARB       0x8893
    #endif
    #ifndef GL_STATIC_DRAW
    #  define GL_STATIC_DRAW                    0x88E4
    #  define GL_STATIC_DRAW_ARB                0x88E4
    #endif

/*********** REMPLACER CERTAINS APPELS GL PAR GL4D ***********/

    #define glCreateShader               gl4dCreateShader
    #define glCreateShaderObjectARB      gl4dCreateShader
    #define glShaderSource               gl4dShaderSource
    #define glShaderSourceARB            gl4dShaderSource
    #define glCompileShader              gl4dCompileShader
    #define glCompileShaderARB           gl4dCompileShader
    #define glCreateProgram              gl4dCreateProgram
    #define glCreateProgramObjectARB     gl4dCreateProgram
    #define glAttachShader               gl4dAttachShader
    #define glAttachObjectARB            gl4dAttachShader
    #define glLinkProgram                gl4dLinkProgram
    #define glLinkProgramARB             gl4dLinkProgram
    #define glUseProgram                 gl4dUseProgram
    #define glUseProgramObjectARB        gl4dUseProgram
    #define glGetShaderiv                gl4dGetShaderiv
    #define glGetProgramiv               gl4dGetProgramiv
    #define glGetObjectParameterivARB(a,b,c) do {\
      fprintf(stderr, "%s (%d): Ne peut determiner si glGetShaderiv ou glGetProgramiv. Veuillez modifier manuellement.\n",\
      __FILE__, __LINE__); glGetObjectParameterivARB(a,b,c);} while(0)

    #define glGetShaderInfoLog           gl4dGetShaderInfoLog
    #define glGetProgramInfoLog          gl4dGetProgramInfoLog
    #define glGetInfoLogARB(a,b,c,d) do {\
      fprintf(stderr, "%s (%d): Ne peut determiner si glGetShaderInfoLog ou glGetProgramInfoLog. Veuillez modifier manuellement.\n",\
      __FILE__, __LINE__); glGetInfoLogARB(a,b,c,d);} while(0)

    #define glDetachShader               gl4dDetachShader
    #define glDetachObjectARB            gl4dDetachShader
    #define glDeleteShader               gl4dDeleteShader
    #define glDeleteProgram              gl4dDeleteProgram
    #define glDeleteObjectARB(a) do {\
      fprintf(stderr, "%s (%d): Ne peut determiner si glDeleteShader ou glDeleteProgram. Veuillez modifier manuellement.\n",\
      __FILE__, __LINE__); glDeleteObjectARB(a);} while(0)

    #define glGetUniformLocation            gl4dGetUniformLocation
    #define glGetUniformLocationARB         gl4dGetUniformLocation
    #define glBindFragDataLocation          gl4dBindFragDataLocation
    #define glBindFragDataLocationEXT       gl4dBindFragDataLocation
    #define glBlitFramebuffer               gl4dBlitFramebuffer
    #define glGenFramebuffers               gl4dGenFramebuffers
    #define glDeleteFramebuffers            gl4dDeleteFramebuffers
    #define glBindFramebuffer               gl4dBindFramebuffer
    #define glFramebufferTexture2D          gl4dFramebufferTexture2D
    #define glDrawBuffers                   gl4dDrawBuffers
    #define glActiveTexture                 gl4dActiveTexture
    #define glBindBuffer                    gl4dBindBuffer
    #define glBufferData                    gl4dBufferData
    #define glGenBuffers                    gl4dGenBuffers
    #define glDeleteBuffers                 gl4dDeleteBuffers
    #define glBlendFuncSeparate             gl4dBlendFuncSeparate

    #define glUniform1i                     gl4dUniform1i
    #define glUniform1iARB                  gl4dUniform1i
    #define glUniform1f                     gl4dUniform1f
    #define glUniform1fARB                  gl4dUniform1f
    #define glUniform2i                     gl4dUniform2i
    #define glUniform2iARB                  gl4dUniform2i
    #define glUniform2f                     gl4dUniform2f
    #define glUniform2fARB                  gl4dUniform2f
    #define glUniform3i                     gl4dUniform3i
    #define glUniform3iARB                  gl4dUniform3i
    #define glUniform3f                     gl4dUniform3f
    #define glUniform3fARB                  gl4dUniform3f
    #define glUniform4i                     gl4dUniform4i
    #define glUniform4iARB                  gl4dUniform4i
    #define glUniform4f                     gl4dUniform4f
    #define glUniform4fARB                  gl4dUniform4f
    #define glUniform1iv                    gl4dUniform1iv
    #define glUniform1ivARB                 gl4dUniform1iv
    #define glUniform1fv                    gl4dUniform1fv
    #define glUniform1fvARB                 gl4dUniform1fv
    #define glUniform2iv                    gl4dUniform2iv
    #define glUniform2ivARB                 gl4dUniform2iv
    #define glUniform2fv                    gl4dUniform2fv
    #define glUniform2fvARB                 gl4dUniform2fv
    #define glUniform3iv                    gl4dUniform3iv
    #define glUniform3ivARB                 gl4dUniform3iv
    #define glUniform3fv                    gl4dUniform3fv
    #define glUniform3fvARB                 gl4dUniform3fv
    #define glUniform4iv                    gl4dUniform4iv
    #define glUniform4ivARB                 gl4dUniform4iv
    #define glUniform4fv                    gl4dUniform4fv
    #define glUniform4fvARB                 gl4dUniform4fv
    #define glUniformMatrix2fv              gl4dUniformMatrix2fv
    #define glUniformMatrix2fvARB           gl4dUniformMatrix2fv
    #define glUniformMatrix3fv              gl4dUniformMatrix3fv
    #define glUniformMatrix3fvARB           gl4dUniformMatrix3fv
    #define glUniformMatrix4fv              gl4dUniformMatrix4fv
    #define glUniformMatrix4fvARB           gl4dUniformMatrix4fv
    #define glIsProgram                     gl4dIsProgram
    #define glIsProgramARB                  gl4dIsProgram
    #define glIsShader                      gl4dIsShader
    #define glIsShaderARB                   gl4dIsShader
    #define glValidateProgram               gl4dValidateProgram
    #define glValidateProgramARB            gl4dValidateProgram
    #define glGetAttribLocation             gl4dGetAttribLocation
    #define glGetAttribLocationARB          gl4dGetAttribLocation
    #define glBindAttribLocation            gl4dBindAttribLocation
    #define glBindAttribLocationARB         gl4dBindAttribLocation
    #define glVertexAttrib1s                gl4dVertexAttrib1s
    #define glVertexAttrib1f                gl4dVertexAttrib1f
    #define glVertexAttrib1d                gl4dVertexAttrib1d
    #define glVertexAttrib2s                gl4dVertexAttrib2s
    #define glVertexAttrib2f                gl4dVertexAttrib2f
    #define glVertexAttrib2d                gl4dVertexAttrib2d
    #define glVertexAttrib3s                gl4dVertexAttrib3s
    #define glVertexAttrib3f                gl4dVertexAttrib3f
    #define glVertexAttrib3d                gl4dVertexAttrib3d
    #define glVertexAttrib4s                gl4dVertexAttrib4s
    #define glVertexAttrib4f                gl4dVertexAttrib4f
    #define glVertexAttrib4d                gl4dVertexAttrib4d
    #define glVertexAttrib4Nub              gl4dVertexAttrib4Nub
    #define glVertexAttrib1sv               gl4dVertexAttrib1sv
    #define glVertexAttrib1fv               gl4dVertexAttrib1fv
    #define glVertexAttrib1dv               gl4dVertexAttrib1dv
    #define glVertexAttrib2sv               gl4dVertexAttrib2sv
    #define glVertexAttrib2fv               gl4dVertexAttrib2fv
    #define glVertexAttrib2dv               gl4dVertexAttrib2dv
    #define glVertexAttrib3sv               gl4dVertexAttrib3sv
    #define glVertexAttrib3fv               gl4dVertexAttrib3fv
    #define glVertexAttrib3dv               gl4dVertexAttrib3dv
    #define glVertexAttrib4bv               gl4dVertexAttrib4bv
    #define glVertexAttrib4sv               gl4dVertexAttrib4sv
    #define glVertexAttrib4iv               gl4dVertexAttrib4iv
    #define glVertexAttrib4fv               gl4dVertexAttrib4fv
    #define glVertexAttrib4dv               gl4dVertexAttrib4dv
    #define glVertexAttrib4ubv              gl4dVertexAttrib4ubv
    #define glVertexAttrib4usv              gl4dVertexAttrib4usv
    #define glVertexAttrib4uiv              gl4dVertexAttrib4uiv
    #define glVertexAttrib4Nbv              gl4dVertexAttrib4Nbv
    #define glVertexAttrib4Nsv              gl4dVertexAttrib4Nsv
    #define glVertexAttrib4Niv              gl4dVertexAttrib4Niv
    #define glVertexAttrib4Nubv             gl4dVertexAttrib4Nubv
    #define glVertexAttrib4Nusv             gl4dVertexAttrib4Nusv
    #define glVertexAttrib4Nuiv             gl4dVertexAttrib4Nuiv
    #define glVertexAttrib1sARB             gl4dVertexAttrib1s
    #define glVertexAttrib1fARB             gl4dVertexAttrib1f
    #define glVertexAttrib1dARB             gl4dVertexAttrib1d
    #define glVertexAttrib2sARB             gl4dVertexAttrib2s
    #define glVertexAttrib2fARB             gl4dVertexAttrib2f
    #define glVertexAttrib2dARB             gl4dVertexAttrib2d
    #define glVertexAttrib3sARB             gl4dVertexAttrib3s
    #define glVertexAttrib3fARB             gl4dVertexAttrib3f
    #define glVertexAttrib3dARB             gl4dVertexAttrib3d
    #define glVertexAttrib4sARB             gl4dVertexAttrib4s
    #define glVertexAttrib4fARB             gl4dVertexAttrib4f
    #define glVertexAttrib4dARB             gl4dVertexAttrib4d
    #define glVertexAttrib4NubARB           gl4dVertexAttrib4Nub
    #define glVertexAttrib1svARB            gl4dVertexAttrib1sv
    #define glVertexAttrib1fvARB            gl4dVertexAttrib1fv
    #define glVertexAttrib1dvARB            gl4dVertexAttrib1dv
    #define glVertexAttrib2svARB            gl4dVertexAttrib2sv
    #define glVertexAttrib2fvARB            gl4dVertexAttrib2fv
    #define glVertexAttrib2dvARB            gl4dVertexAttrib2dv
    #define glVertexAttrib3svARB            gl4dVertexAttrib3sv
    #define glVertexAttrib3fvARB            gl4dVertexAttrib3fv
    #define glVertexAttrib3dvARB            gl4dVertexAttrib3dv
    #define glVertexAttrib4bvARB            gl4dVertexAttrib4bv
    #define glVertexAttrib4svARB            gl4dVertexAttrib4sv
    #define glVertexAttrib4ivARB            gl4dVertexAttrib4iv
    #define glVertexAttrib4fvARB            gl4dVertexAttrib4fv
    #define glVertexAttrib4dvARB            gl4dVertexAttrib4dv
    #define glVertexAttrib4ubvARB           gl4dVertexAttrib4ubv
    #define glVertexAttrib4usvARB           gl4dVertexAttrib4usv
    #define glVertexAttrib4uivARB           gl4dVertexAttrib4uiv
    #define glVertexAttrib4NbvARB           gl4dVertexAttrib4Nbv
    #define glVertexAttrib4NsvARB           gl4dVertexAttrib4Nsv
    #define glVertexAttrib4NivARB           gl4dVertexAttrib4Niv
    #define glVertexAttrib4NubvARB          gl4dVertexAttrib4Nubv
    #define glVertexAttrib4NusvARB          gl4dVertexAttrib4Nusv
    #define glVertexAttrib4NuivARB          gl4dVertexAttrib4Nuiv
    #define glVertexAttribPointer           gl4dVertexAttribPointer
    #define glVertexAttribPointerARB        gl4dVertexAttribPointer
    #define glEnableVertexAttribArray       gl4dEnableVertexAttribArray
    #define glEnableVertexAttribArrayARB    gl4dEnableVertexAttribArray
    #define glDisableVertexAttribArray      gl4dDisableVertexAttribArray
    #define glDisableVertexAttribArrayARB   gl4dDisableVertexAttribArray
    #define glDeleteVertexArrays            gl4dDeleteVertexArrays
    #define glGenVertexArrays               gl4dGenVertexArrays
    #define glBindVertexArray               gl4dBindVertexArray
    #define glGetFramebufferAttachmentParameteriv               gl4dGetFramebufferAttachmentParameteriv

    #ifdef __cplusplus
    extern "C" {
    #endif

    GL4DAPI GLuint    GL4DAPIENTRY gl4dCreateShader(GLenum shaderType);
    GL4DAPI void      GL4DAPIENTRY gl4dShaderSource(GLuint shader, GLint numOfStrings, const char ** strings, GLint * lenOfStrings);
    GL4DAPI void      GL4DAPIENTRY gl4dCompileShader(GLuint shader);
    GL4DAPI GLuint    GL4DAPIENTRY gl4dCreateProgram(void);
    GL4DAPI void      GL4DAPIENTRY gl4dAttachShader(GLuint program, GLuint shader);
    GL4DAPI void      GL4DAPIENTRY gl4dLinkProgram(GLuint program);
    GL4DAPI void      GL4DAPIENTRY gl4dUseProgram(GLuint program);
    GL4DAPI void      GL4DAPIENTRY gl4dGetShaderiv(GLuint object, GLenum type, GLint *param);
    GL4DAPI void      GL4DAPIENTRY gl4dGetProgramiv(GLuint object, GLenum type, GLint *param);
    GL4DAPI void      GL4DAPIENTRY gl4dGetShaderInfoLog(GLuint object, GLint maxLen, GLint * len, char * log);
    GL4DAPI void      GL4DAPIENTRY gl4dGetProgramInfoLog(GLuint object, GLint maxLen, GLint * len, char * log);
    GL4DAPI void      GL4DAPIENTRY gl4dDetachShader(GLuint program, GLuint shader);
    GL4DAPI void      GL4DAPIENTRY gl4dDeleteShader(GLuint object);
    GL4DAPI void      GL4DAPIENTRY gl4dDeleteProgram(GLuint object);
    GL4DAPI GLboolean GL4DAPIENTRY gl4dIsProgram(GLuint program);
    GL4DAPI GLboolean GL4DAPIENTRY gl4dIsShader(GLuint shader);
    GL4DAPI void      GL4DAPIENTRY gl4dValidateProgram(GLuint program);
    GL4DAPI GLint     GL4DAPIENTRY gl4dGetUniformLocation(GLuint program, const char * name);
    GL4DAPI void      GL4DAPIENTRY gl4dBindFragDataLocation(GLuint program, GLuint colorNumber, const GLchar * name);
    GL4DAPI void      GL4DAPIENTRY gl4dBlitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
    GL4DAPI void      GL4DAPIENTRY gl4dGenFramebuffers(GLsizei n, GLuint * ids);
    GL4DAPI void      GL4DAPIENTRY gl4dDeleteFramebuffers(GLsizei n, GLuint * ids);
    GL4DAPI void      GL4DAPIENTRY gl4dBindFramebuffer(GLenum target, GLuint framebuffer);
    GL4DAPI void      GL4DAPIENTRY gl4dFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
    GL4DAPI void      GL4DAPIENTRY gl4dDrawBuffers(GLsizei n, const GLenum * bufs);
    GL4DAPI void      GL4DAPIENTRY gl4dActiveTexture(GLenum texture);
    GL4DAPI void      GL4DAPIENTRY gl4dDeleteBuffers(GLsizei n, const GLuint * buffers);
    GL4DAPI void      GL4DAPIENTRY gl4dGenBuffers(GLsizei n, GLuint * buffers);
    GL4DAPI void      GL4DAPIENTRY gl4dBufferData(GLenum target, GLsizeiptr size, const GLvoid * data, GLenum usage);
    GL4DAPI void      GL4DAPIENTRY gl4dBindBuffer(GLenum target, GLuint buffer);
    GL4DAPI void      GL4DAPIENTRY gl4dBlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
    GL4DAPI void      GL4DAPIENTRY gl4dUniform1i(GLint location, GLint v0);
    GL4DAPI void      GL4DAPIENTRY gl4dUniform1f(GLint location, GLfloat v0);
    GL4DAPI void      GL4DAPIENTRY gl4dUniform2i(GLint location, GLint v0, GLint v1);
    GL4DAPI void      GL4DAPIENTRY gl4dUniform2f(GLint location, GLfloat v0, GLfloat v1);
    GL4DAPI void      GL4DAPIENTRY gl4dUniform3i(GLint location, GLint v0, GLint v1, GLint v2);
    GL4DAPI void      GL4DAPIENTRY gl4dUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
    GL4DAPI void      GL4DAPIENTRY gl4dUniform4i(GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
    GL4DAPI void      GL4DAPIENTRY gl4dUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
    GL4DAPI void      GL4DAPIENTRY gl4dUniform1iv(GLint location, GLsizei count, const GLint * values);
    GL4DAPI void      GL4DAPIENTRY gl4dUniform1fv(GLint location, GLsizei count, const GLfloat * values);
    GL4DAPI void      GL4DAPIENTRY gl4dUniform2iv(GLint location, GLsizei count, const GLint * values);
    GL4DAPI void      GL4DAPIENTRY gl4dUniform2fv(GLint location, GLsizei count, const GLfloat * values);
    GL4DAPI void      GL4DAPIENTRY gl4dUniform3iv(GLint location, GLsizei count, const GLint * values);
    GL4DAPI void      GL4DAPIENTRY gl4dUniform3fv(GLint location, GLsizei count, const GLfloat * values);
    GL4DAPI void      GL4DAPIENTRY gl4dUniform4iv(GLint location, GLsizei count, const GLint * values);
    GL4DAPI void      GL4DAPIENTRY gl4dUniform4fv(GLint location, GLsizei count, const GLfloat * values);
    GL4DAPI void      GL4DAPIENTRY gl4dUniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat * values);
    GL4DAPI void      GL4DAPIENTRY gl4dUniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat * values);
    GL4DAPI void      GL4DAPIENTRY gl4dUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat * values);
    GL4DAPI GLint     GL4DAPIENTRY gl4dGetAttribLocation(GLuint program, const char * name);
    GL4DAPI void      GL4DAPIENTRY gl4dBindAttribLocation(GLuint program, GLuint index, const char * name);
    GL4DAPI void      GL4DAPIENTRY gl4dVertexAttrib1s(GLuint index, GLshort v0);
    GL4DAPI void      GL4DAPIENTRY gl4dVertexAttrib1f(GLuint index, GLfloat v0);
    GL4DAPI void      GL4DAPIENTRY gl4dVertexAttrib1d(GLuint index, GLdouble v0);
    GL4DAPI void      GL4DAPIENTRY gl4dVertexAttrib2s(GLuint index, GLshort v0, GLshort v1);
    GL4DAPI void      GL4DAPIENTRY gl4dVertexAttrib2f(GLuint index, GLfloat v0, GLfloat v1);
    GL4DAPI void      GL4DAPIENTRY gl4dVertexAttrib2d(GLuint index, GLdouble v0, GLdouble v1);
    GL4DAPI void      GL4DAPIENTRY gl4dVertexAttrib3s(GLuint index, GLshort v0, GLshort v1, GLshort v2);
    GL4DAPI void      GL4DAPIENTRY gl4dVertexAttrib3f(GLuint index, GLfloat v0, GLfloat v1, GLfloat v2);
    GL4DAPI void      GL4DAPIENTRY gl4dVertexAttrib3d(GLuint index, GLdouble v0, GLdouble v1, GLdouble v2);
    GL4DAPI void      GL4DAPIENTRY gl4dVertexAttrib4s(GLuint index, GLshort v0, GLshort v1, GLshort v2, GLshort v3);
    GL4DAPI void      GL4DAPIENTRY gl4dVertexAttrib4f(GLuint index, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
    GL4DAPI void      GL4DAPIENTRY gl4dVertexAttrib4d(GLuint index, GLdouble v0, GLdouble v1, GLdouble v2, GLdouble v3);
    GL4DAPI void      GL4DAPIENTRY gl4dVertexAttrib4Nub(GLuint index, GLubyte v0, GLubyte v1, GLubyte v2, GLubyte v3);
    GL4DAPI void      GL4DAPIENTRY gl4dVertexAttrib1sv(GLuint index, const GLshort * values);
    GL4DAPI void      GL4DAPIENTRY gl4dVertexAttrib1fv(GLuint index, const GLfloat * values);
    GL4DAPI void      GL4DAPIENTRY gl4dVertexAttrib1dv(GLuint index, const GLdouble * values);
    GL4DAPI void      GL4DAPIENTRY gl4dVertexAttrib2sv(GLuint index, const GLshort * values);
    GL4DAPI void      GL4DAPIENTRY gl4dVertexAttrib2fv(GLuint index, const GLfloat * values);
    GL4DAPI void      GL4DAPIENTRY gl4dVertexAttrib2dv(GLuint index, const GLdouble * values);
    GL4DAPI void      GL4DAPIENTRY gl4dVertexAttrib3sv(GLuint index, const GLshort * values);
    GL4DAPI void      GL4DAPIENTRY gl4dVertexAttrib3fv(GLuint index, const GLfloat * values);
    GL4DAPI void      GL4DAPIENTRY gl4dVertexAttrib3dv(GLuint index, const GLdouble * values);
    GL4DAPI void      GL4DAPIENTRY gl4dVertexAttrib4bv(GLuint index, const GLbyte * values);
    GL4DAPI void      GL4DAPIENTRY gl4dVertexAttrib4sv(GLuint index, const GLshort * values);
    GL4DAPI void      GL4DAPIENTRY gl4dVertexAttrib4iv(GLuint index, const GLint * values);
    GL4DAPI void      GL4DAPIENTRY gl4dVertexAttrib4fv(GLuint index, const GLfloat * values);
    GL4DAPI void      GL4DAPIENTRY gl4dVertexAttrib4dv(GLuint index, const GLdouble * values);
    GL4DAPI void      GL4DAPIENTRY gl4dVertexAttrib4ubv(GLuint index, const GLubyte * values);
    GL4DAPI void      GL4DAPIENTRY gl4dVertexAttrib4usv(GLuint index, const GLushort * values);
    GL4DAPI void      GL4DAPIENTRY gl4dVertexAttrib4uiv(GLuint index, const GLuint * values);
    GL4DAPI void      GL4DAPIENTRY gl4dVertexAttrib4Nbv(GLuint index, const GLbyte * values);
    GL4DAPI void      GL4DAPIENTRY gl4dVertexAttrib4Nsv(GLuint index, const GLshort * values);
    GL4DAPI void      GL4DAPIENTRY gl4dVertexAttrib4Niv(GLuint index, const GLint * values);
    GL4DAPI void      GL4DAPIENTRY gl4dVertexAttrib4Nubv(GLuint index, const GLubyte * values);
    GL4DAPI void      GL4DAPIENTRY gl4dVertexAttrib4Nusv(GLuint index, const GLushort * values);
    GL4DAPI void      GL4DAPIENTRY gl4dVertexAttrib4Nuiv(GLuint index, const GLuint * values);
    GL4DAPI void      GL4DAPIENTRY gl4dVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer);
    GL4DAPI void      GL4DAPIENTRY gl4dEnableVertexAttribArray(GLuint index);
    GL4DAPI void      GL4DAPIENTRY gl4dDisableVertexAttribArray(GLuint index);
    GL4DAPI void      GL4DAPIENTRY gl4dDeleteVertexArrays(GLsizei n, const GLuint * arrays);
    GL4DAPI void      GL4DAPIENTRY gl4dGenVertexArrays(GLsizei n, GLuint * arrays);
    GL4DAPI void      GL4DAPIENTRY gl4dBindVertexArray(GLuint array);
    GL4DAPI void      GL4DAPIENTRY glGetFramebufferAttachmentParameteriv(GLenum target,  GLenum attachment,  GLenum pname,  GLint * params);

#ifdef __cplusplus
}
#endif

#endif
#endif
