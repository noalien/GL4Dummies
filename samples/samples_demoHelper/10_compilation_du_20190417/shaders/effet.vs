/*!\file effet.vs
 *
 * \brief vertex shader identité avec transfert de coordonnées de texture
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr 
 * \date May 14 2018
 */
#version 330

layout (location = 0) in vec3 vsiPosition;
layout (location = 1) in vec3 vsiNormal;
layout (location = 2) in vec2 vsiTexCoord;

out vec2 vsoTexCoord; 

void main(void) {
  vsoTexCoord = vsiTexCoord;
  gl_Position = vec4(vsiPosition, 1.0);
}
