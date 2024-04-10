/*!\file basic.vs
 *
 * \brief Transformations matricielles standards pour les sommets,
 * usage pour la shadow map
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr 
 * \date March 13 2017
 */
#version 330 core

uniform mat4 mod;
uniform mat4 lightView;
uniform mat4 lightProj;
layout (location = 0) in vec3 vsiPosition;
layout (location = 1) in vec3 vsiNormal;
layout (location = 2) in vec2 vsiTexCoord;

void main(void) {
  gl_Position = lightProj * lightView * mod * vec4(vsiPosition, 1.0);
}
