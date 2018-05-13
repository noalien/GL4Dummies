/*!\file depTex.vs
 *
 * \brief Transformations matricielles standards pour les sommets, les
 * normales et les coordonnées de texture
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr 
 * \date May 13 2018
 */
#version 330

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
layout (location = 0) in vec3 vsiPosition;
layout (location = 1) in vec3 vsiNormal;
layout (location = 2) in vec2 vsiTexCoord;

out vec2 vsoTexCoord; 
out vec3 vsoNormal;

void main(void) {
  vsoTexCoord = vsiTexCoord;
  vsoNormal = (transpose(inverse(modelViewMatrix))  * vec4(vsiNormal, 0.0)).xyz;
  gl_Position = projectionMatrix * modelViewMatrix * vec4(vsiPosition, 1.0);
}
