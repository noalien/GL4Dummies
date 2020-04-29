/*!\file basic.vs
 *
 * \brief vertex shader basique un sommet possédant les attributs
 * coordonnées spaciales 2D et couleur. De plus une variable d'angle
 * uniform est envoyée dans "uniform float angle;" nous l'utilisons
 * pour construire une matrice de rotation autour de y et nous
 * l'utilisons.
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

uniform float angle;

void main(void) {
  /* une matrice de rotation autour de l'axe y telle que définie en
     écriture conventionnelle pour les mathématiciens. En GL, nous
     utilisons la transposée (miroir par rapport à la diagonale de la
     matrice) de l'écriture conventionnelle. Translatez le point de 1
     en y pour voir quelle est la différence (en math la translation
     est stockée sur les 3 premières lignes de la 4ème colonne).*/
  mat4 roty = mat4(vec4(cos(angle), 0, -sin(angle), 0),
		   vec4(         0, 1,           0, 0),
		   vec4(sin(angle), 0,  cos(angle), 0),
		   vec4(         0, 0,           0, 1));
  /* gl_Position, nom réservé produisant un sommet GL */
  /* l'entrée vsiPosition est complétée en vecteur 4D (x, y, z, w) où
     z = 0.0 et w = 1.0 ; elle est multipliée à gauche par une matrice
     de modélisation */
  gl_Position = transpose(roty) * vec4(vsiPosition, 0.0, 1.0);
  /* la couleur en sortie du vertex shader vers le fragment shader, vsoColor est un vecteur 4D (w = 1) alors que la couleur reçu est 3D */
  vsoColor = vec4(vsiColor, 1.0);
}
