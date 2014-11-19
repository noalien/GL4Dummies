/*!\file gl4dm.h
 *
 * \brief macros et fonctions liées aux calculs mathématiques propres
 * à l'utilisation de GL4Dummies.
 *
 * \author Farès BELHADJ amsi@ai.univ-paris8.fr
 * \date November 12, 2014
*/

#ifndef _GL4DM_H
#define _GL4DM_H

#include <gl4dummies.h>

/* MACROS DIVERS */

#ifdef MIN
#  undef MIN
#endif
#ifdef MAX
#  undef MAX
#endif
#ifdef SIGN
#  undef SIGN
#endif
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define SIGN(i) ((i) > 0 ? 1 : -1)

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

/*!\brief Multiplication d'une deux matrices 4x4. Les matrices \a a et \a
 * b sont multipliées et le résultat est stocké dans \a r. */
#define MMAT4XVEC4(r, m, v) do {					\
    (r)[0]  = (m)[0]  * (v)[0] + (m)[1]  * (v)[1] + (m)[2]  * (v)[2]  + (m)[3]  * (v)[3]; \
    (r)[1]  = (m)[4]  * (v)[0] + (m)[5]  * (v)[1] + (m)[6]  * (v)[2]  + (m)[7]  * (v)[3]; \
    (r)[2]  = (m)[8]  * (v)[0] + (m)[9]  * (v)[1] + (m)[10] * (v)[2]  + (m)[11] * (v)[3]; \
    (r)[3]  = (m)[12] * (v)[0] + (m)[13] * (v)[1] + (m)[14] * (v)[2]  + (m)[15] * (v)[3]; \
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

/*!\brief Normalise le vecteur 4D \a v. */
#define MVEC4NORMALIZE(v) do {						\
    double n = sqrt((v)[0] * (v)[0] + (v)[1] * (v)[1] + (v)[2] * (v)[2] + (v)[3] * (v)[3]); \
    if(n > 0) {								\
      (v)[0] /= n;							\
      (v)[1] /= n;							\
      (v)[2] /= n;							\
      (v)[3] /= n;							\
    }									\
  } while(0)

/*!\brief Normalise le vecteur 3D \a v. */
#define MVEC3NORMALIZE(v) do {						\
    double n = sqrt((v)[0] * (v)[0] + (v)[1] * (v)[1] + (v)[2] * (v)[2]);		\
    if(n > 0) {								\
      (v)[0] /= n;							\
      (v)[1] /= n;							\
      (v)[2] /= n;							\
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

/*!\brief Création d'une matrice de projection selon l'ancienne
 * fonction glFrustum.
 *
 * \todo rajouter la ORTHO, l'équivalent de gluLookAt
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

/*!\brief Chargement d'une matrice identitéé dans \a m. */
#define MIDENTITY(m) do {						\
    (m)[1] = (m)[2] = (m)[3] = (m)[4] = (m)[6] = (m)[7] = (m)[8] = (m)[9] = (m)[11] = (m)[12] = (m)[13] = (m)[14] = 0.0; \
    (m)[0] = (m)[5] = (m)[10] = (m)[15] = 1.0;				\
  } while(0)
      
#ifdef __cplusplus
extern "C" {
#endif

GL4DAPI double    GL4DAPIENTRY u_rand(void);
GL4DAPI double    GL4DAPIENTRY su_rand(void);
GL4DAPI double    GL4DAPIENTRY g_rand(void);
GL4DAPI double    GL4DAPIENTRY gsu_rand(void);
GL4DAPI double    GL4DAPIENTRY gu_rand(void);

#ifdef __cplusplus
}
#endif

#endif
