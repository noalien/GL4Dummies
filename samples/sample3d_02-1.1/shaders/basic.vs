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
  /* pourquoi la transposée inverse ? 
     la preuve :
     http://www.ai.univ-paris8.fr/~amsi/PG3D1718S2/dl/The_Normal_Matrix_Lighthouse3d.pdf
   */
  vsoNormal = (transpose(inverse(modelViewMatrix))  * vec4(vsiNormal, 0.0)).xyz;
  vsoPosition = vsiPosition;
  /* position transformée du sommet afin de calculer l'incidence de la
   * lumière plus loin */
  vsoModPos = mp.xyz;
  gl_Position = projectionMatrix * mp;
  vsoTexCoord = vsiTexCoord;
}
