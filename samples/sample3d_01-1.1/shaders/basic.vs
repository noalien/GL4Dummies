/*!\file basic.vs
 *
 * \brief vertex shader basique, un sommet possédant les attributs
 * coordonnées spaciales 3D et couleur. De plus, une matrice de
 * projection, une matrice vue et une matrice de modélisation sont
 * envoyées par GL4Dummies, nous les réceptionnons sous la forme de
 * "uniform mat4" et nous les utilisons. Un int inv est aussi envoyé
 * et sert de booléen pour inverser l'axe y des coordonnées de
 * textures.
 *
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date February 11 2018
 */

/* Vertex shader, directement en relatrion avec le code-CPU */
/* Version GLSL 3.30 */
#version 330

/* variable cablée sur l'attribut 0 du sommet dans le code CPU
 * (glVertexAttribPointer(0, ...) le 0 correspond au location = 0)
 * c'est une coordonnée spaciale 3D */
layout (location = 0) in vec3 vsiPosition;
/* variable cablée sur l'attribut 1 du sommet dans le code CPU
 * (glVertexAttribPointer(1, ...) le 1 correspond au location = 1)
 * c'est une couleur 3D (r, g, b) */
layout (location = 1) in vec3 vsiColor;
/* variable cablée sur l'attribut 2 du sommet dans le code CPU
 * (glVertexAttribPointer(2, ...) le 2 correspond au location = 2)
 * c'est une coordonnée de textures 2D */
layout (location = 2) in vec2 vsiTexCoord;

/* une couleur de sortie du vertex shader vers le fragment shader
 * (voir basic.fs, in vec4 vsoColor) */
out vec4 vsoColor;
/* une coordonnée de textures de sortie du vertex shader vers le
 * fragment shader (voir basic.fs, in vec2 vsoTexCoord) */
out vec2 vsoTexCoord;
/* les trois matrices crées à l'aide de fonctionnalités GL4Dummies
 * côté CPU et envoyées via gl4duSendMatrices */
uniform mat4 projectionMatrix, viewMatrix, modelMatrix;
/* sert de booléen pour inverser l'axe y des coordonnées de
 * textures */
uniform int inv;

void main(void) {
  /* gl_Position, nom réservé produisant un sommet GL */
  /* l'entrée vsiPosition est complétée en vecteur 4D (x, y, z, w) où
   * z = 0.0 et w = 1.0 ; elle est multipliée à gauche par une matrice
   * de modélisation puis de projection (lire de droite à gauche à
   * partir du sommet) */
  gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vsiPosition, 1.0);
  /* la couleur en sortie du vertex shader vers le fragment shader,
   * vsoColor est un vecteur 4D (w = 1) alors que la couleur reçu est
   * 3D */
  vsoColor = vec4(vsiColor, 1.0);
  /* coordonnée de texture transmise du vertex shader vers le fragment
   * shader, si inv est différent de zéro, nous inversons la coordonné
   * de texture sur l'axe y */
  vsoTexCoord = vec2(vsiTexCoord.x, (inv != 0) ? 1.0 - vsiTexCoord.y : vsiTexCoord.y);
}
