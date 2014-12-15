#version 330

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
layout (location = 0) in vec3 vsiPosition;
layout (location = 1) in vec3 vsiNormal;
layout (location = 2) in vec2 vsiTexCoord;
 
out vec2 vsoTexCoord;
out vec3 vsoNormal;
out vec4 vsoModPosition;

void main(void) {
  vsoNormal = (transpose(inverse(modelViewMatrix)) * vec4(vsiNormal.xyz, 0.0)).xyz;
  vsoModPosition = modelViewMatrix * vec4(vsiPosition.xyz, 1.0);
  gl_Position = projectionMatrix * modelViewMatrix * vec4(vsiPosition.xyz, 1.0);
  vsoTexCoord = vsiTexCoord;
}
