/*!\file gl4dqQuaternion.h
 *
 * \brief gl4dqQuaternion : Header utilise pour manipuler des gl4dqQuaternions
 *
 * \auteur : Vincent Boyer boyer@ai.univ-paris8.fr 
 * \auteur : Sylvia Chalencon chalencon@ai.univ-paris8.fr 
 *
 * \date : 2013-2015
*/


#ifndef __QUATERNION_H__
#define __QUATERNION_H__


#ifdef __cplusplus
extern "C" {
#endif
/** 
   \typedef gl4dqQuaternion

   La partie scalaire est dans Quaternion[0]
   La partie imaginaire i, j, k est dans Quaternion[1, 2 et 3]
*/
typedef float gl4dqQuaternion[4];
  
/**
  \brief fonction de calcul de la partie scalaire d'un gl4dqQuaternion unitaire ou
   seule la partie imaginaire est donnee
   \param un gl4dqQuaternion
*/
void gl4dqComputeScalaire(gl4dqQuaternion );

/**
  \brief fonction de calcul du conjuge d'un gl4dqQuaternion
  \param[in] un gl4dqQuaternion
  \param[out] son conjuge
*/
void gl4dqConjuge(const gl4dqQuaternion, gl4dqQuaternion);

/**
  \brief fonction de calcul de la multiplication de deux gl4dqQuaternions
  \param[in] deux gl4dqQuaternions
  \param[out] le resultat de la multiplication
*/
void gl4dqMultiply(const gl4dqQuaternion, const gl4dqQuaternion, gl4dqQuaternion);

/**
  \brief fonction qui calcule la norme d'un gl4dqQuaternion 
  \param un gl4dqQuaternion
*/
float gl4dqMagnitude(gl4dqQuaternion);

/**
  \brief fonction qui transforme un gl4dqQuaternion en un gl4dqQuaternion unitaire
  \param un gl4dqQuaternion
*/
void gl4dqNormalize(gl4dqQuaternion);


/**
  \brief fonction qui cree un gl4dqQuaternion pur a partir d'un vecteur
  \param[in] un vecteur
  \param[out] un gl4dqQuaternion
*/
void gl4dqCreatePurFromPoint(float *, gl4dqQuaternion);


/**
  \brief fonction qui applique une rotation sur un vecteur a l'aide d'un gl4dqQuaternion
  \param[in] un gl4dqQuaternion
  \param[in] un vecteur 
  \param[out] un vecteur 
*/
void gl4dqRotatePoint(const gl4dqQuaternion, float*, float*);

/**
  \brief fonction qui genere la matrice de rotation sur un vecteur a partir d'un gl4dqQuaternion
  \param[in] un gl4dqQuaternion
  \param[out] la matrice de rotation
*/
void gl4dqQuaternionToMatrix (const gl4dqQuaternion, float *);
#ifdef __cplusplus
}
#endif

#endif
