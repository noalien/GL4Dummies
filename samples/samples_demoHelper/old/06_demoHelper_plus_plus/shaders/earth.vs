/*!\file basic.vs
 *
 * \brief Transformations matricielles standards pour les sommets, les
 * normales et les coordonnées de texture
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr 
 * \date April 15 2016
 */
#version 330

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
layout (location = 0) in vec3 vsiPosition;
layout (location = 1) in vec3 vsiNormal;
layout (location = 2) in vec2 vsiTexCoord;
 
out vec3 vsoNormal;
out vec3 vsoPosition;
out vec3 vsoModPos;
out vec2 vsoTexCoord;

void main(void) {
  vec4 mp = modelViewMatrix * vec4(vsiPosition, 1.0);
  vsoNormal = (transpose(inverse(modelViewMatrix))  * vec4(vsiNormal, 0.0)).xyz;
  vsoPosition = vsoPosition;
  vsoModPos   = mp.xyz;
  vsoTexCoord = vec2(vsiTexCoord.x, 1.0 - vsiTexCoord.y);
  gl_Position = projectionMatrix * mp;
}
