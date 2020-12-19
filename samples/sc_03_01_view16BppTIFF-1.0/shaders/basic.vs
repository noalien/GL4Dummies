/*!\file basic.vs
 *
 * \brief vertex shader <<basique>>
 * \author Farès BELHADJ, amsi@up8.edu
 * \date July 13 2020
 */

#version 330

layout (location = 0) in vec3 vsiPosition;
layout (location = 1) in vec3 vsiNormal;
layout (location = 2) in vec2 vsiTexCoord;

/* une coordonnée de textures de sortie du vertex shader vers le
 * fragment shader (voir basic.fs, in vec2 vsoTexCoord) */
out vec2 vsoTexCoord;

uniform mat4 projectionMatrix, modelViewMatrix;

/* uniforme qui sert de booléen pour inverser l'axe y des coordonnées
 * de textures */
uniform int inv;

void main(void) {
  gl_Position = projectionMatrix * modelViewMatrix * vec4(vsiPosition, 1.0);
  /* coordonnée de texture transmise du vertex shader vers le fragment
   * shader, si inv est différent de zéro, nous inversons la coordonné
   * de texture sur l'axe y */
  vsoTexCoord = vec2(vsiTexCoord.x, (inv != 0) ? 1.0 - vsiTexCoord.y : vsiTexCoord.y);
}
