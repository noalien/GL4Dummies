#version 330

layout (location = 0) in vec2 vsiPosition;
layout (location = 1) in vec2 vsiTexCoord;
 
out vec2 vsoTexCoord;

void main(void) {
   gl_Position = vec4(vsiPosition, 0.0, 1.0);
   vsoTexCoord = vec2(vsiTexCoord.s, 1.0 - vsiTexCoord.t);
}
