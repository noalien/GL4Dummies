/*!\file gl4dq.h
 *
 * \brief fonctions de manipulation de Quaternions
 *
 * \author Vincent Boyer boyer@ai.univ-paris8.fr 
 * \author Sylvia Chalencon chalencon@ai.univ-paris8.fr
 *
 * \date 2013-2015
*/

#ifndef __GL4DQ_H__
#define __GL4DQ_H__


#ifdef __cplusplus
extern "C" {
#endif
  /** 
      \typedef gl4dqQuaternion
      
      La partie scalaire est dans Quaternion[0]\n
      La partie imaginaire i, j, k est dans Quaternion[1, 2 et 3]
  */
  typedef float gl4dqQuaternion[4];
  
  /**
     \brief fonction de calcul de la partie scalaire d'un gl4dqQuaternion unitaire où
     seule la partie imaginaire est donnée
     \param q un quaternion
  */
  void gl4dqComputeScalaire(gl4dqQuaternion q);
  
  /**
     \brief fonction de calcul du conjugué d'un gl4dqQuaternion
     \param q un quaternion
     \param conjugate son conjugué
  */
  void gl4dqConjugate(const gl4dqQuaternion q, gl4dqQuaternion conjugate);
  
  /**
     \brief fonction de calcul de la multiplication de deux gl4dqQuaternions
     \param qa premier quaternion
     \param qb second quaternion
     \param qr le quaternion résultant de la multiplication des deux précédents
  */
  void gl4dqMultiply(const gl4dqQuaternion qa, const gl4dqQuaternion qb, gl4dqQuaternion qr);
  
  /**
     \brief fonction qui calcule la norme d'un gl4dqQuaternion 
     \param q un quaternion
  */
  float gl4dqMagnitude(gl4dqQuaternion q);
  
  /**
     \brief fonction qui transforme un gl4dqQuaternion en un gl4dqQuaternion unitaire
     \param q un quaternion
  */
  void gl4dqNormalize(gl4dqQuaternion q);
  
  /**
     \brief fonction qui créé un gl4dqQuaternion pur à partir d'un vecteur
     \param v un vecteur
     \param qr le quaternion pur
  */
  void gl4dqCreatePurFromPoint(float * v, gl4dqQuaternion qr);
  
  /**
     \brief fonction qui applique une rotation sur un vecteur à l'aide
     d'un gl4dqQuaternion
     \param q un quaternion
     \param vsrc le vecteur source
     \param vdst le vecteur destination
  */
  void gl4dqRotatePoint(const gl4dqQuaternion q, float* vsrc, float* vdst);
  
  /**
     \brief fonction qui génère la matrice de rotation sur un vecteur à
     partir d'un gl4dqQuaternion 
     \param q un quaternion
     \param mat la matrice de rotation
  */
  void gl4dqQuaternionToMatrix (const gl4dqQuaternion q, float * mat);

#ifdef __cplusplus
}
#endif

#endif
