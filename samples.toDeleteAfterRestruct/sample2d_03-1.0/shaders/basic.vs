#version 330
layout (location = 0) in vec3 vsiPosition;
layout (location = 1) in vec3 vsiNormal;
layout (location = 2) in vec2 vsiTexCoord;
out vec2 vsoTexCoord;

uniform float toto;

void main(void) {
  gl_Position = vec4(vsiPosition/*  * toto */, 1.0);
  vsoTexCoord = vsiTexCoord;
}
