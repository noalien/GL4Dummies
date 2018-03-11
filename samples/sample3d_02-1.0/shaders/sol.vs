#version 330

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
layout (location = 0) in vec3 vsiPosition;
layout (location = 1) in vec2 vsiTexCoord;
 
void main(void) {
  gl_Position = projectionMatrix * modelViewMatrix * vec4(vsiPosition, 1.0);
}
