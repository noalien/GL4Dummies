/*!\file gl4dm.h
 *
 * \brief macros et fonctions liées aux calculs mathématiques propres
 * à l'utilisation de GL4Dummies.
 *
 * \author Farès BELHADJ amsi@ai.univ-paris8.fr
 * \date November 12, 2014
 *
 * \todo voir ce qui est fait avec les w des vec4 et corriger toutes
 * les macros si nécessaire.
 */

#ifndef _GL4DM_H
#define _GL4DM_H

#include "gl4dummies.h"

#include <math.h>

/* MACROS DIVERS */

#define GL4DM_E		        2.7182818284590452354
#define GL4DM_LOG2E	        1.4426950408889634074
#define GL4DM_LOG10E	        0.43429448190325182765
#define GL4DM_LN2		0.69314718055994530942
#define GL4DM_LN10		2.30258509299404568402
#define GL4DM_PI		3.14159265358979323846
#define GL4DM_PI_2		1.57079632679489661923
#define GL4DM_PI_4		0.78539816339744830962
#define GL4DM_PI_180		0.01745329251994329576
#define GL4DM_1_PI		0.31830988618379067154
#define GL4DM_2_PI		0.63661977236758134308
#define GL4DM_2_SQRTPI	        1.12837916709551257390
#define GL4DM_SQRT2	        1.41421356237309504880
#define GL4DM_SQRT1_2	        0.70710678118654752440
#define GL4DM_EPSILON           1.19209290e-07F

#ifdef MIN
#  undef MIN
#endif
#ifdef MAX
#  undef MAX
#endif
#ifdef SIGN
#  undef SIGN
#endif
#ifdef SQUARE
#  undef SQUARE
#endif
#ifdef COTANGENT
#  undef COTANGENT
#endif
#ifdef RADIAN
#  undef RADIAN
#endif
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define SIGN(i) ((i) < 0 ? -1 : 1)
#define SQUARE(X) ((X)*(X))
#define COTANGENT(X) (cos(X)/sin(X))
#define RADIAN(X) (GL4DM_PI * (X) / 180)

/* MACROS LIEES AUX CALCULS VECTORIELS ET/OU MATRICIELS */

/*!\brief Multiplication de deux matrices 4x4. Les matrices \a a et \a
 * b sont multipliées et le résultat est stocké dans \a r. */
#define MMAT4XMAT4(r, a, b) do {					\
    (r)[0]  = (a)[0]  * (b)[0] + (a)[1]  * (b)[4] + (a)[2]  * (b)[8]  + (a)[3]  * (b)[12]; \
    (r)[1]  = (a)[0]  * (b)[1] + (a)[1]  * (b)[5] + (a)[2]  * (b)[9]  + (a)[3]  * (b)[13]; \
    (r)[2]  = (a)[0]  * (b)[2] + (a)[1]  * (b)[6] + (a)[2]  * (b)[10] + (a)[3]  * (b)[14]; \
    (r)[3]  = (a)[0]  * (b)[3] + (a)[1]  * (b)[7] + (a)[2]  * (b)[11] + (a)[3]  * (b)[15]; \
    (r)[4]  = (a)[4]  * (b)[0] + (a)[5]  * (b)[4] + (a)[6]  * (b)[8]  + (a)[7]  * (b)[12]; \
    (r)[5]  = (a)[4]  * (b)[1] + (a)[5]  * (b)[5] + (a)[6]  * (b)[9]  + (a)[7]  * (b)[13]; \
    (r)[6]  = (a)[4]  * (b)[2] + (a)[5]  * (b)[6] + (a)[6]  * (b)[10] + (a)[7]  * (b)[14]; \
    (r)[7]  = (a)[4]  * (b)[3] + (a)[5]  * (b)[7] + (a)[6]  * (b)[11] + (a)[7]  * (b)[15]; \
    (r)[8]  = (a)[8]  * (b)[0] + (a)[9]  * (b)[4] + (a)[10] * (b)[8]  + (a)[11] * (b)[12]; \
    (r)[9]  = (a)[8]  * (b)[1] + (a)[9]  * (b)[5] + (a)[10] * (b)[9]  + (a)[11] * (b)[13]; \
    (r)[10] = (a)[8]  * (b)[2] + (a)[9]  * (b)[6] + (a)[10] * (b)[10] + (a)[11] * (b)[14]; \
    (r)[11] = (a)[8]  * (b)[3] + (a)[9]  * (b)[7] + (a)[10] * (b)[11] + (a)[11] * (b)[15]; \
    (r)[12] = (a)[12] * (b)[0] + (a)[13] * (b)[4] + (a)[14] * (b)[8]  + (a)[15] * (b)[12]; \
    (r)[13] = (a)[12] * (b)[1] + (a)[13] * (b)[5] + (a)[14] * (b)[9]  + (a)[15] * (b)[13]; \
    (r)[14] = (a)[12] * (b)[2] + (a)[13] * (b)[6] + (a)[14] * (b)[10] + (a)[15] * (b)[14]; \
    (r)[15] = (a)[12] * (b)[3] + (a)[13] * (b)[7] + (a)[14] * (b)[11] + (a)[15] * (b)[15]; \
  } while(0)

/*!\brief Multiplication d'une matrices 4x4 par un vecteur. La matrice
 * \a m et le vecteur \a v sont multipliés et le résultat est stocké
 * dans le vecteur \a r. */
#define MMAT4XVEC4(r, m, v) do {					\
    (r)[0]  = (m)[0]  * (v)[0] + (m)[1]  * (v)[1] + (m)[2]  * (v)[2]  + (m)[3]  * (v)[3]; \
    (r)[1]  = (m)[4]  * (v)[0] + (m)[5]  * (v)[1] + (m)[6]  * (v)[2]  + (m)[7]  * (v)[3]; \
    (r)[2]  = (m)[8]  * (v)[0] + (m)[9]  * (v)[1] + (m)[10] * (v)[2]  + (m)[11] * (v)[3]; \
    (r)[3]  = (m)[12] * (v)[0] + (m)[13] * (v)[1] + (m)[14] * (v)[2]  + (m)[15] * (v)[3]; \
  } while(0)

/*!\brief divise les composantes x, y et z du vecteur \a v par sa
 * composante w et remet w à 1. */
#define MVEC4WEIGHT(v) do {\
    v[0] /= v[3];          \
    v[1] /= v[3];          \
    v[2] /= v[3];          \
    v[3]  = 1.0;           \
  } while(0)

/*!\brief Transposée de la matrice 4x4 \a m. */
#define MMAT4TRANSPOSE(m) do {				\
    double t;						\
    t = (m)[1];  (m)[1]  = (m)[4];  (m)[4]  = t;	\
    t = (m)[2];  (m)[2]  = (m)[8];  (m)[8]  = t;	\
    t = (m)[3];  (m)[3]  = (m)[12]; (m)[12] = t;	\
    t = (m)[6];  (m)[6]  = (m)[9];  (m)[9]  = t;	\
    t = (m)[7];  (m)[7]  = (m)[13]; (m)[13] = t;	\
    t = (m)[11]; (m)[11] = (m)[14]; (m)[14] = t;	\
  } while(0)

/*!\brief Inverse de la matrice 4x4 \a m. */
#define MMAT4INVERSE(m) do {\
    GLdouble tmp[16], det;\
    int i;\
    tmp[0] = (m)[5]  * (m)[10] * (m)[15] - \
        (m)[5]  * (m)[11] * (m)[14] - \
        (m)[9]  * (m)[6]  * (m)[15] + \
        (m)[9]  * (m)[7]  * (m)[14] + \
        (m)[13] * (m)[6]  * (m)[11] - \
        (m)[13] * (m)[7]  * (m)[10]; \
    tmp[4] = -(m)[4]  * (m)[10] * (m)[15] + \
        (m)[4]  * (m)[11] * (m)[14] + \
        (m)[8]  * (m)[6]  * (m)[15] - \
        (m)[8]  * (m)[7]  * (m)[14] - \
        (m)[12] * (m)[6]  * (m)[11] + \
        (m)[12] * (m)[7]  * (m)[10]; \
    tmp[8] = (m)[4]  * (m)[9] * (m)[15] - \
        (m)[4]  * (m)[11] * (m)[13] - \
        (m)[8]  * (m)[5] * (m)[15] + \
        (m)[8]  * (m)[7] * (m)[13] + \
        (m)[12] * (m)[5] * (m)[11] - \
        (m)[12] * (m)[7] * (m)[9]; \
    tmp[12] = -(m)[4]  * (m)[9] * (m)[14] + \
        (m)[4]  * (m)[10] * (m)[13] + \
        (m)[8]  * (m)[5] * (m)[14] - \
        (m)[8]  * (m)[6] * (m)[13] - \
        (m)[12] * (m)[5] * (m)[10] + \
        (m)[12] * (m)[6] * (m)[9]; \
    tmp[1] = -(m)[1]  * (m)[10] * (m)[15] + \
        (m)[1]  * (m)[11] * (m)[14] + \
        (m)[9]  * (m)[2] * (m)[15] - \
        (m)[9]  * (m)[3] * (m)[14] - \
        (m)[13] * (m)[2] * (m)[11] + \
        (m)[13] * (m)[3] * (m)[10]; \
    tmp[5] = (m)[0]  * (m)[10] * (m)[15] - \
        (m)[0]  * (m)[11] * (m)[14] - \
        (m)[8]  * (m)[2] * (m)[15] + \
        (m)[8]  * (m)[3] * (m)[14] + \
        (m)[12] * (m)[2] * (m)[11] - \
        (m)[12] * (m)[3] * (m)[10]; \
    tmp[9] = -(m)[0]  * (m)[9] * (m)[15] + \
        (m)[0]  * (m)[11] * (m)[13] + \
        (m)[8]  * (m)[1] * (m)[15] - \
        (m)[8]  * (m)[3] * (m)[13] - \
        (m)[12] * (m)[1] * (m)[11] + \
        (m)[12] * (m)[3] * (m)[9]; \
    tmp[13] = (m)[0]  * (m)[9] * (m)[14] - \
        (m)[0]  * (m)[10] * (m)[13] - \
        (m)[8]  * (m)[1] * (m)[14] + \
        (m)[8]  * (m)[2] * (m)[13] + \
        (m)[12] * (m)[1] * (m)[10] - \
        (m)[12] * (m)[2] * (m)[9]; \
    tmp[2] = (m)[1]  * (m)[6] * (m)[15] - \
        (m)[1]  * (m)[7] * (m)[14] - \
        (m)[5]  * (m)[2] * (m)[15] + \
        (m)[5]  * (m)[3] * (m)[14] + \
        (m)[13] * (m)[2] * (m)[7] - \
        (m)[13] * (m)[3] * (m)[6]; \
    tmp[6] = -(m)[0]  * (m)[6] * (m)[15] + \
        (m)[0]  * (m)[7] * (m)[14] + \
        (m)[4]  * (m)[2] * (m)[15] - \
        (m)[4]  * (m)[3] * (m)[14] - \
        (m)[12] * (m)[2] * (m)[7] + \
        (m)[12] * (m)[3] * (m)[6]; \
    tmp[10] = (m)[0]  * (m)[5] * (m)[15] - \
        (m)[0]  * (m)[7] * (m)[13] - \
        (m)[4]  * (m)[1] * (m)[15] + \
        (m)[4]  * (m)[3] * (m)[13] + \
        (m)[12] * (m)[1] * (m)[7] - \
        (m)[12] * (m)[3] * (m)[5]; \
    tmp[14] = -(m)[0]  * (m)[5] * (m)[14] + \
        (m)[0]  * (m)[6] * (m)[13] + \
        (m)[4]  * (m)[1] * (m)[14] - \
        (m)[4]  * (m)[2] * (m)[13] - \
        (m)[12] * (m)[1] * (m)[6] + \
        (m)[12] * (m)[2] * (m)[5]; \
    tmp[3] = -(m)[1] * (m)[6] * (m)[11] + \
        (m)[1] * (m)[7] * (m)[10] + \
        (m)[5] * (m)[2] * (m)[11] - \
        (m)[5] * (m)[3] * (m)[10] - \
        (m)[9] * (m)[2] * (m)[7] + \
        (m)[9] * (m)[3] * (m)[6]; \
    tmp[7] = (m)[0] * (m)[6] * (m)[11] - \
        (m)[0] * (m)[7] * (m)[10] - \
        (m)[4] * (m)[2] * (m)[11] + \
        (m)[4] * (m)[3] * (m)[10] + \
        (m)[8] * (m)[2] * (m)[7] - \
        (m)[8] * (m)[3] * (m)[6]; \
    tmp[11] = -(m)[0] * (m)[5] * (m)[11] + \
        (m)[0] * (m)[7] * (m)[9] + \
        (m)[4] * (m)[1] * (m)[11] - \
        (m)[4] * (m)[3] * (m)[9] - \
        (m)[8] * (m)[1] * (m)[7] + \
        (m)[8] * (m)[3] * (m)[5]; \
    tmp[15] = (m)[0] * (m)[5] * (m)[10] - \
        (m)[0] * (m)[6] * (m)[9] - \
        (m)[4] * (m)[1] * (m)[10] + \
        (m)[4] * (m)[2] * (m)[9] + \
        (m)[8] * (m)[1] * (m)[6] - \
        (m)[8] * (m)[2] * (m)[5]; \
    det = (m)[0] * tmp[0] + (m)[1] * tmp[4] + (m)[2] * tmp[8] + (m)[3] * tmp[12]; \
    /*!\todo comment gérer det == 0.0 ? */\
    det = 1.0 / det; \
    for(i = 0; i < 16; i++) \
        (m)[i] = tmp[i] * det; \
  } while(0)


/*!\brief Produit vectoriel 3D entre \a u et \a v écrit dans \a r. */
#define MVEC3CROSS(r, u, v) do {		\
    (r)[0] = (u)[1] * (v)[2] - (u)[2] * (v)[1];	\
    (r)[1] = (u)[2] * (v)[0] - (u)[0] * (v)[2];	\
    (r)[2] = (u)[0] * (v)[1] - (u)[1] * (v)[0];	\
  } while(0)

/*!\brief Produit vectoriel 3D entre \a u et \a v écrit dans \a r avec
 * un 1.0 pour la 4ème dimension de r. */
#define MVEC4CROSS(r, u, v) do {		\
    MVEC3CROSS(r, u, v);			\
    (r)[3] = 1.0;				\
  } while(0)

/*!\brief Renvoie le produit scalaire entre deux vecteurs 2D entre \a u et \a v. */
#define MVEC2DOT(u, v) ((u)[0] * (v)[0] + (u)[1] * (v)[1])

/*!\brief Renvoie le produit scalaire entre deux vecteurs 3D entre \a u et \a v. */
#define MVEC3DOT(u, v) ((u)[0] * (v)[0] + (u)[1] * (v)[1] + (u)[2] * (v)[2])

/*!\brief Renvoie le produit scalaire entre deux vecteurs 4D entre \a u et \a v. */
#define MVEC4DOT(u, v) ((u)[0] * (v)[0] + (u)[1] * (v)[1] + (u)[2] * (v)[2] + (u)[3] * (v)[3])

/*!\brief Normalise le vecteur 4D \a v. */
#define MVEC4NORMALIZE(v) do {						\
    double MVEC4NORMALIZE_n = sqrt((v)[0] * (v)[0] + (v)[1] * (v)[1] + (v)[2] * (v)[2] + (v)[3] * (v)[3]); \
    if(MVEC4NORMALIZE_n > 0) {						\
      (v)[0] /= MVEC4NORMALIZE_n;					\
      (v)[1] /= MVEC4NORMALIZE_n;					\
      (v)[2] /= MVEC4NORMALIZE_n;					\
      (v)[3] /= MVEC4NORMALIZE_n;					\
    }									\
  } while(0)

/*!\brief Normalise le vecteur 3D \a v. */
#define MVEC3NORMALIZE(v) do {						\
    double MVEC4NORMALIZE_n = sqrt((v)[0] * (v)[0] + (v)[1] * (v)[1] + (v)[2] * (v)[2]); \
    if(MVEC4NORMALIZE_n > 0) {						\
      (v)[0] /= MVEC4NORMALIZE_n;					\
      (v)[1] /= MVEC4NORMALIZE_n;					\
      (v)[2] /= MVEC4NORMALIZE_n;					\
    }									\
  } while(0)

/*!\brief Normalise le vecteur 2D v. */
#define MVEC2NORMALIZE(v) do {						\
    double n = sqrt((v)[0] * (v)[0] + (v)[1] * (v)[1]);			\
    if(n > 0) {								\
      (v)[0] /= n;							\
      (v)[1] /= n;							\
    }									\
  } while(0)

/*!\brief Création d'une matrice de projection en perspective selon
 * l'ancienne fonction glFrustum.
 */
#define MFRUSTUM(mat, l, r, b, t, n, f) do {		\
    (mat)[0]  = 2.0 * (n) / ((r) - (l));		\
    (mat)[1]  = 0.0;					\
    (mat)[2]  = ((r) + (l)) / ((r) - (l));		\
    (mat)[3]  = 0.0;					\
    (mat)[4]  = 0.0;					\
    (mat)[5]  = 2.0 * (n) / ((t) - (b));		\
    (mat)[6]  = ((t) + (b)) / ((t) - (b));		\
    (mat)[7]  = 0.0;					\
    (mat)[8]  = 0.0;					\
    (mat)[9]  = 0.0;					\
    (mat)[10] = -((f) + (n)) / ((f) - (n));		\
    (mat)[11] = -2.0 * (f) * (n) / ((f) - (n));		\
    (mat)[12] = 0.0;					\
    (mat)[13] = 0.0;					\
    (mat)[14] = -1.0;					\
    (mat)[15] = 0.0;					\
  } while(0)

/*!\brief Création d'une matrice de projection orthogonale selon
 * l'ancienne fonction glOrtho.
 */
#define MORTHO(mat, l, r, b, t, n, f) do {		\
    (mat)[0]  = 2.0 / ((r) - (l));		        \
    (mat)[1]  = 0.0;					\
    (mat)[2]  = 0.0;					\
    (mat)[3]  = -((r) + (l)) / ((r) - (l));		\
    (mat)[4]  = 0.0;					\
    (mat)[5]  = 2.0 / ((t) - (b));		        \
    (mat)[6]  = 0.0;					\
    (mat)[7]  = -((t) + (b)) / ((t) - (b));		\
    (mat)[8]  = 0.0;					\
    (mat)[9]  = 0.0;					\
    (mat)[10] = -2.0 / ((f) - (n));		        \
    (mat)[11] = -((f) + (n)) / ((f) - (n));		\
    (mat)[12] = 0.0;					\
    (mat)[13] = 0.0;					\
    (mat)[14] = 0.0;					\
    (mat)[15] = 1.0;					\
  } while(0)



/*!\brief Création d'une matrice de projection en perspective selon
 * l'ancienne fonction gluPerspective.
 */
#define MPERSPECTIVE(mat, fovy, aspect, zNear, zFar) do {		\
  memset((perspective), 0, sizeof(perspective));                        \
  (perspective)[0]  = (1.0/tan((fovy) * GL4DM_PI_180 / 2.0))/(aspect);  \
  (perspective)[5]  = 1.0/tan((fovy) * GL4DM_PI_180 / 2.0);		\
  (perspective)[10] = ((zFar)+(zNear)) / ((zNear)-(zFar));		\
  (perspective)[11] = 2*(zFar)*(zNear)/((zNear)-(zFar));		\
  (perspective)[14] = -1;                                               \
  } while(0)


/*!\brief Chargement d'une matrice identitéé dans \a m. */
#define MIDENTITY(m) do {						\
    (m)[1] = (m)[2] = (m)[3] = (m)[4] = (m)[6] = (m)[7] = (m)[8] = (m)[9] = (m)[11] = (m)[12] = (m)[13] = (m)[14] = 0.0; \
    (m)[0] = (m)[5] = (m)[10] = (m)[15] = 1.0;				\
  } while(0)
      
#ifdef __cplusplus
extern "C" {
#endif

GL4DAPI double    GL4DAPIENTRY gl4dmURand(void);
GL4DAPI double    GL4DAPIENTRY gl4dmSURand(void);
GL4DAPI double    GL4DAPIENTRY gl4dmGRand(void);
GL4DAPI double    GL4DAPIENTRY gl4dmGURand(void);
GL4DAPI GLfloat * GL4DAPIENTRY gl4dmTriangleEdge(GLuint width, GLuint height, GLfloat H);

#ifdef __cplusplus
}
#endif

/*!\brief Méthodes alternative utilisant des fonctions inline 
 * (compatible seulement a partir de la norme c99)*/

typedef struct
{
  float x, y, z, w;
} GL4DMVector;

typedef struct
{
  GL4DMVector r[4];
} GL4DMMatrix;

#include "gl4dm.inl"

#endif
