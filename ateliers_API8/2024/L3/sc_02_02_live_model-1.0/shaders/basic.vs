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
out vec3 vsoPos;

uniform mat4 projectionMatrix, viewMatrix, modelMatrix;

void main(void) {
  mp = modelMatrix * vec4(vsiPosition, 1.0);
  /* gl_Position, nom réservé produisant un sommet GL */
  /* l'entrée vsiPosition est complétée en vecteur 4D (x, y, z, w) où
   * w = 1.0 ; elle est multipliée à gauche par une matrice de
   * modélisation puis vue puis projection (lire de droite à gauche à
   * partir du sommet) */
  gl_Position = projectionMatrix * viewMatrix * mp;
  /* la couleur en sortie du vertex shader vers le fragment shader,
   * vsoColor est un vecteur 4D (w = 1) alors que la couleur reçu est
   * 3D */
  vsoPos = vsiPosition;
  vsoNormal = vsiNormal;
}
