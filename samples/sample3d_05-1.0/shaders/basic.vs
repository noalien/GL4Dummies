/*!\file basic.vs
 *
 * \brief Transformations matricielles standards pour les sommets et
 * les normales, selon 2 points de vues. Le classique (celui sortant
 * gl_Position) utilise le point de vue de la caméra, l'autre (sort
 * vsoSMCoord) utilise le point de vue de la lumière.
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr 
 * \date April 15 2016
 */
#version 330 core

uniform mat4 modelMatrix;
uniform mat4 cameraViewMatrix;
uniform mat4 cameraProjectionMatrix;
uniform mat4 lightViewMatrix;
uniform mat4 lightProjectionMatrix;
layout (location = 0) in vec3 vsiPosition;
layout (location = 1) in vec3 vsiNormal;
layout (location = 2) in vec2 vsiTexCoord;
 
out vec4 vsoNormal;
out vec4 vsoMVPos;
out vec4 vsoSMCoord;

void main(void) {
  const mat4 bias = mat4( 0.5, 0.0, 0.0, 0.0,
			  0.0, 0.5, 0.0, 0.0,
			  0.0, 0.0, 0.5, 0.0,
			  0.5, 0.5, 0.5, 1.0 );
  vsoMVPos    = cameraViewMatrix * modelMatrix * vec4(vsiPosition, 1.0);
  vsoNormal   = transpose(inverse(cameraViewMatrix * modelMatrix))  * vec4(vsiNormal, 0.0);
  gl_Position = cameraProjectionMatrix * vsoMVPos;
  vsoSMCoord  = bias * lightProjectionMatrix * lightViewMatrix * modelMatrix * vec4(vsiPosition, 1.0);
}
