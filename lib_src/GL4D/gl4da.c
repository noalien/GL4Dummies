/*!\file gl4da.c
 *
 * \brief fonctions de manipulation du repère caméra 
 * \author Sylvia Chalencon chalencon@ai.univ-paris8.fr
 *
 * \date 2017
*/

#include "gl4da.h"
#include <math.h>

void gl4daMapToSphere (gl4daArcball *a, int x, int y, float * pointSphere){
  float p [2];
  p[0]= (x - a->center[0]) /a->radius;
  p[1]= (a->center[1] - y) /a->radius;
  
  float d2 = (p[0]*p[0]) + (p[1]*p[1]);
  if (d2 > 1.0f) {
    float sqrtd2 = sqrtf (d2);
    pointSphere[0] = p[0]/ sqrtd2;
    pointSphere[1] = p[1]/ sqrtd2;
    pointSphere[2] = 0.0f;
  }
  else{
    pointSphere[0] = p[0];
    pointSphere[1] = p[1];
    pointSphere[2] = sqrtf (1.0f - d2);
  }
}

void gl4daResize (gl4daArcball *a, int w, int h){
  a->center[0]   = 0.5f * w;
  a->center[1]   = 0.5f * h;
  a->radius   = 0.5f * fmax (w, h);
}

void gl4daInit (gl4daArcball *a, int x, int y){
  gl4daMapToSphere (a, x, y, a->position);
}

void gl4daTrack (gl4daArcball * a, int x, int y, float * quat){
  float p2 [3];
  gl4daMapToSphere (a, x, y, p2);
  float crossProdPositionP2 [3];
  crossProdPositionP2 [0] = a->position[1] * p2[2] - a->position[2] * p2[1];
  crossProdPositionP2 [1] = a->position[2] * p2[0] - a->position[0] * p2[2];
  crossProdPositionP2 [2] = a->position[0] * p2[1] - a->position[1] * p2[0];

  float dotProdPositionP2 = a->position[0] * p2[0] + a->position[1] * p2[1] + a->position[2] * p2[2];
  quat[0] = 0.5f * dotProdPositionP2;
  quat[1] = crossProdPositionP2 [0];
  quat[2] = crossProdPositionP2 [1];
  quat[3] = crossProdPositionP2 [2];
  gl4dqNormalize (quat);
}
