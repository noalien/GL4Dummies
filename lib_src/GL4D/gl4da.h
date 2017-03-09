/*!\file gl4da.h
 *
 * \brief fonctions de manipulation du repère caméra 
 *
 * \author Sylvia Chalençon chalencon@ai.univ-paris8.fr 
 *
 * \date 2017
*/

#ifndef __GL4DA_H__
#define __GL4DA_H__

//#include "gl4dq.h"
#include <GL4D/gl4dq.h>

#ifdef __cplusplus
extern "C" {
#endif

  /** 
      \typedef gl4daArcball
      m_center stocke le centre de l'écran\n
      m_radius rayon de la sphère par rapport à l'écran
      m_position est la position de l'arcball
  */
  typedef struct gl4daArcball gl4daArcball;
  struct gl4daArcball {
    float center [2];
    float radius;
    float position [3];
  };
  
  /**
     \brief fonction de calcul des coordonnées d'un oint écran sur la sphère
     \param a arcball 
     \param x abscisse du point à l'écran 
     \param y ordonnée du point à l'écran 
     \param pointSphere point qui va recevoir les coordonnées du point sur la sphère
  */
  void gl4daMapToSphere (gl4daArcball *a, int x, int y, float * pointSphere);
  
  /**
     \brief fonction à appeler au resize de la fenêtre
     \param a arcball 
     \param w nouvelle largeur de la fenêtre 
     \param h nouvelle hauteur de la fenêtre 
  */
  void gl4daResize (gl4daArcball *a, int w, int h);
  
  /**
     \brief fonction qui initialise la position de l'arcball
     \param a arcball 
     \param x abscisse de la souris dans la fenêtre
     \param y ordonnée de la souris dans la fenêtre
  */
  void gl4daInit (gl4daArcball *a, int x, int y);
  
  /**
     \brief fonction qui met à jour le quaternion en fonction du déplacement de la souris
     \param a arcball 
     \param x abscisse de la souris dans la fenêtre
     \param y ordonnée de la souris dans la fenêtre
     \param quat quaternion mis à jour
  */
  void gl4daTrack (gl4daArcball *a, int x, int y, float * quat);
  
#ifdef __cplusplus
}
#endif

#endif
