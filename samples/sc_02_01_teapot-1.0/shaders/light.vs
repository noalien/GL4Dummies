/*!\file light.vs
 * \brief vertex shader progressif sur l'éclairage */
#version 330

layout (location = 0) in vec3 vsiPosition;
layout (location = 1) in vec3 vsiNormal;
layout (location = 2) in vec2 vsiTexCoord;

uniform mat4 projectionMatrix, viewMatrix, modelMatrix;

out vec2 vsoTexCoord;


void main(void) {
  /* gl_Position, nom réservé produisant un sommet GL */
  /* l'entrée vsiPosition est complétée en vecteur 4D (x, y, z, w) où
   * w = 1.0 ; elle est multipliée à gauche par une matrice de
   * modélisation puis vue puis projection (lire de droite à gauche à
   * partir du sommet) */
  gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vsiPosition, 1.0);
  vsoTexCoord = vsiTexCoord;
}
