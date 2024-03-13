#version 330

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform float temps;
layout (location = 0) in vec3 vsiPosition;
layout (location = 1) in vec3 vsiNormal;
layout (location = 2) in vec2 vsiTexCoord;

out vec2 vsoTexCoord;
out vec4 vsoPos;

void main(void) {
  vsoTexCoord = vsiTexCoord;
  vsoPos = vec4(vsiPosition, temps);
  gl_Position = projectionMatrix * modelViewMatrix * vec4(vsiPosition, 1.0);
}
