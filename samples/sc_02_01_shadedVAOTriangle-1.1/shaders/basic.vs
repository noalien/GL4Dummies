/*!\file basic.vs
 *
 * \brief vertex shader basique un sommet possédant les attributs
 * coordonnées spaciales 2D et couleur.
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date February 11 2018
 */

/* Vertex shader, directement en relatrion avec le code-CPU */
/* Version GLSL 3.30 */
#version 330

/* variable cablée sur l'attribut 0 du sommet dans le code CPU (glVertexAttribPointer(0, ...) le 0 correspond au location = 0) */
layout (location = 0) in vec2 vsiPosition;
/* variable cablée sur l'attribut 1 du sommet dans le code CPU (glVertexAttribPointer(1, ...) le 1 correspond au location = 1) */
layout (location = 1) in vec3 vsiColor;

/* une sortie du vertex shader vers le fragment shader (voir basic.fs, in vec2 vsoColor) */
out vec4 vsoColor;

void main(void) {
  /* gl_Position, nom réservé produisant un sommet GL */
  /* l'entrée vsiPosition est complétée en vecteur 4D (x, y, z, w) où
     z = 0.0 et w = 1.0 */
  gl_Position = vec4(vsiPosition, 0.0, 1.0);
  /* la couleur en sortie du vertex shader vers le fragment shader, vsoColor est un vecteur 4D (w = 1) alors que la couleur reçu est 3D */
  vsoColor = vec4(vsiColor, 1.0);
}
