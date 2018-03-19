#version 330

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
layout (location = 0) in vec3 vsiPosition;
layout (location = 1) in vec3 vsiNormal;
layout (location = 2) in vec2 vsiTexCoord;
 
out vec3 vsoModPos;
out vec2 vsoTexCoord;

void main(void) {
  vec4 mp = modelViewMatrix * vec4(vsiPosition, 1.0);
  vsoModPos = mp.xyz;
  vsoTexCoord = vsiTexCoord;
  gl_Position = projectionMatrix * mp;
}
