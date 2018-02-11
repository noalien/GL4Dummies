/*!\file basic.vs
 *
 * \brief vertex shader basique gérant les transformations modelview
 * et projection pour un sommet possédant les attributs texture et
 * couleur.
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date January 20 2017
 */

/* Vertex shader, directement en relatrion avec le code-CPU */
/* Version GLSL 3.30 */
#version 330

/* variables uniformes envoyées directement et uniformément depuis le code-CPU */
/* ces variables sont de type mat4 càd matrice 4x4 de flottants */
uniform mat4 modelViewMatrix, projectionMatrix;
/* variable cablée sur l'attribut 0 du sommet dans le code CPU (glVertexAttribPointer(0, ...) le 0 correspond au location = 0) */
layout (location = 0) in vec2 vsiPosition;
/* variable cablée sur l'attribut 1 du sommet dans le code CPU (glVertexAttribPointer(1, ...) le 1 correspond au location = 1) */
layout (location = 1) in vec3 vsiColor;
/* variable cablée sur l'attribut 2 du sommet dans le code CPU (glVertexAttribPointer(2, ...) le 2 correspond au location = 2) */
layout (location = 2) in vec2 vsiTexCoord;

/* une sortie du vertex shader vers le fragment shader (voir basic.fs, in vec2 vsoTexCoord) */
out vec2 vsoTexCoord;
/* une sortie du vertex shader vers le fragment shader (voir basic.fs, in vec2 vsoColor) */
out vec3 vsoColor;

void main(void) {
  /* gl_Position, nom réservé produisant un sommet GL */
  /* l'entrée vsiPosition est complétée en vecteur 4D (x, y, z, w) où
     z = 0.0 et w = 1.0, elle est multipliée apr la matrice de
     modélisation puis la matrice de projection */
  gl_Position = projectionMatrix * modelViewMatrix * vec4(vsiPosition, 0.0, 1.0);
  /* la coordonée de texture en sortie du vertex shader vers le fragment shader */
  vsoTexCoord = vec2(vsiTexCoord.s, 1.0 - vsiTexCoord.t);
  /* la couleur en sortie du vertex shader vers le fragment shader */
  vsoColor = vsiColor;
}
