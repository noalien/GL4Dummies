#include "gl4dq.h"
#include <math.h>

void gl4dqComputeScalaire(gl4dqQuaternion q){
  float t = 1.0f - (q[1] * q[1]) - (q[2] * q[2]) - (q[3] * q[3]);
  q[0] = (t>0.0) ? -sqrt(t) : 0.0;
}

void gl4dqConjuge(const gl4dqQuaternion q, gl4dqQuaternion conjuge){
  conjuge[0] = q[0];
  conjuge[1] = -q[1];
  conjuge[2] = -q[2];
  conjuge[3] = -q[3];
}


void gl4dqMultiply(const gl4dqQuaternion qa, const gl4dqQuaternion qb, gl4dqQuaternion r){

  r[0] = qa[0]*qb[0] - qa[1]*qb[1] - qa[2]*qb[2] - qa[3]*qb[3];
  r[1] = qa[1]*qb[0] + qa[0]*qb[1] + qa[2]*qb[3] - qa[3]*qb[2];
  r[2] = qa[2]*qb[0] + qa[0]*qb[2] + qa[3]*qb[1] - qa[1]*qb[3];
  r[3] = qa[3]*qb[0] + qa[0]*qb[3] + qa[1]*qb[2] - qa[2]*qb[1];
}

float gl4dqMagnitude(gl4dqQuaternion q){
  return sqrt((q[0]*q[0])+(q[1]*q[1]) +(q[2]*q[2])+(q[3]*q[3]));
}

void gl4dqNormalize(gl4dqQuaternion q){
  float mag = gl4dqMagnitude(q);

  if (mag > 0.0f) {
	q[0] /= mag;
	q[1] /= mag;
	q[2] /= mag;
	q[3] /= mag;
  }
}

void gl4dqCreatePurFromPoint(float *in, gl4dqQuaternion r){
  r[0] = 0.0;
  r[1] = in[0];
  r[2] = in[1];
  r[3] = in[2];
}

void gl4dqRotatePoint(const gl4dqQuaternion q, float *in, float *out) {
  gl4dqQuaternion conjuge, purfromin, r, final;
  
  gl4dqCreatePurFromPoint(in, purfromin);
  //gl4dqNormalize (purfromin);
  gl4dqConjuge(q, conjuge);
  gl4dqMultiply(q, purfromin, r);
  gl4dqMultiply(r, conjuge, final);

  out[0] = final[1];
  out[1] = final[2];
  out[2] = final[3];
}

void gl4dqQuaternionToMatrix (const gl4dqQuaternion q, float * mat){
  float w = q[0], x = q[1], y = q[2], z = q[3];
  mat [0]  = 1 - 2*y*y -2*z*z ; mat [1]  = 2*x*y + 2*w*z    ; mat [2]  = 2*x*z - 2*w*y    ; mat [3]  = 0;
  mat [4]  = 2*x*y - 2*w*z    ; mat [5]  = 1 - 2*x*x -2*z*z ; mat [6]  = 2*y*z + 2*w*x    ; mat [7]  = 0;
  mat [8]  = 2*x*z + 2*w*y    ; mat [9]  = 2*y*z - 2*w*x    ; mat [10] = 1 - 2*x*x -2*y*y ; mat [11] = 0;
  mat [12] = 0                ; mat [13] = 0                ; mat [14] = 0                ; mat [15] = 1;
}
