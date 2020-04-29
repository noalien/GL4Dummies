/*!\file basic.vs
 *
 * \brief vertex shader récupérant une géométrie "classique"
 * GL4Dummies. De plus, une matrice de projection, une matrice vue et
 * une matrice de modélisation sont envoyées par GL4Dummies, nous les
 * réceptionnons sous la forme de "uniform mat4" et nous les
 * utilisons. Un int inv est aussi envoyé et sert de booléen pour
 * inverser l'axe y des coordonnées de textures.
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
 * c'est un vecteur normal */
layout (location = 1) in vec3 vsiNormal;
/* variable cablée sur l'attribut 2 du sommet dans le code CPU
 * (glVertexAttribPointer(2, ...) le 2 correspond au location = 2)
 * c'est une coordonnée de textures 2D */
layout (location = 2) in vec2 vsiTexCoord;

/* une coordonnée de textures de sortie du vertex shader vers le
 * fragment shader (voir basic.fs, in vec2 vsoTexCoord) */
out vec2 vsoTexCoord;
/* les trois matrices crées à l'aide de fonctionnalités GL4Dummies
 * côté CPU et envoyées via gl4duSendMatrices */
uniform mat4 projectionMatrix, viewMatrix, modelMatrix;
/* sert de booléen pour inverser l'axe y des coordonnées de
 * textures */
uniform int inv;
/* sert de facteur de coordonnée de textures pour la faire répéter */
uniform float tcScale;

void main(void) {
  /* gl_Position, nom réservé produisant un sommet GL */
  /* l'entrée vsiPosition est complétée en vecteur 4D (x, y, z, w) où
   * z = 0.0 et w = 1.0 ; elle est multipliée à gauche par une matrice
   * de modélisation puis de projection (lire de droite à gauche à
   * partir du sommet) */
  gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vsiPosition, 1.0);
  /* coordonnée de texture transmise du vertex shader vers le fragment
   * shader. Elle est multipliée par tcScale pour faire répéter la
   * texture. Si inv est différent de zéro, nous inversons la
   * coordonné de texture sur l'axe y */
  vsoTexCoord = tcScale * vec2(vsiTexCoord.x, (inv != 0) ? 1.0 - vsiTexCoord.y : vsiTexCoord.y);
}
