#version 330

layout (location = 0) in vec3 vsiPosition;
layout (location = 1) in vec3 vsiNormal;
layout (location = 2) in vec2 vsiTexCoord;
uniform int inv; 
uniform mat4 modelViewMatrix, projectionMatrix;
out vec2 vsoTexCoord;

void main(void) {
  gl_Position = projectionMatrix * modelViewMatrix * vec4(vsiPosition, 1.0);
  if(inv != 0)
    vsoTexCoord = vec2(vsiTexCoord.s, 1.0 - vsiTexCoord.t);
  else
    vsoTexCoord = vec2(vsiTexCoord.s, vsiTexCoord.t);
}
