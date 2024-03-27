/*!\file basic.vs
 *
 * \brief vertex shader basique
 */

/* Version GLSL 3.30 */
#version 330

layout (location = 0) in vec3 vsiPosition;
layout (location = 1) in vec3 vsiNormal;
layout (location = 2) in vec2 vsiTexCoord;

out vec4 mp;
out vec3 vsoNormal;

uniform float temps;

uniform sampler2D tex;
uniform vec2 offset;
uniform mat4 projectionMatrix, viewMatrix, modelMatrix;

float altitude(vec3 p) {
  vec2 tc = (p.xz + vec2(1.0)) / 2.0;
  /* un y qui vient s'ajouter au reste et produire une gaussienne */
  float y = exp(-8.0 * p.x * p.x) * exp(-8.0 * p.z * p.z);
  /* un v qui vient ajouter une perturbation sinusoidale */
  float v = 0.02 * sin(p.x * 60.0 + temps);
  return texture(tex, tc).r + v + y;
}

vec3 update(vec3 p) {
  return vec3(p.x, altitude(p), p.z);
}

vec3 normale(vec3 a, vec3 b, vec3 c) {
  vec3 v0 = b - a;
  vec3 v1 = c - b;
  return normalize(cross(v0, v1));
}

vec3 normale(vec3 p) {
  vec3 p0 = update(p);
  vec3 p1 = update(p + vec3(0.0, 0.0, offset.y));
  vec3 p2 = update(p + vec3(offset.x, 0.0, 0.0));
  vec3 p3 = update(p + vec3(offset.x, 0.0, -offset.y));
  vec3 p4 = update(p + vec3(0.0, 0.0, -offset.y));
  vec3 p5 = update(p + vec3(-offset.x, 0.0, 0.0));
  vec3 p6 = update(p + vec3(-offset.x, 0.0, offset.y));
  vec3 n = vec3(0.0);
  n += normale(p0, p1, p2);
  n += normale(p0, p2, p3);
  n += normale(p0, p3, p4);
  n += normale(p0, p4, p5);
  n += normale(p0, p5, p6);
  n += normale(p0, p6, p1);
  return n / 6.0;
}

void main(void) {
  vec3 pos = update(vsiPosition);
  mp = modelMatrix * vec4(pos, 1.0);
  /* gl_Position, nom réservé produisant un sommet GL */
  /* l'entrée vsiPosition est complétée en vecteur 4D (x, y, z, w) où
   * w = 1.0 ; elle est multipliée à gauche par une matrice de
   * modélisation puis vue puis projection (lire de droite à gauche à
   * partir du sommet) */
  gl_Position = projectionMatrix * viewMatrix * mp;
  vsoNormal = normale(vsiPosition);
}
