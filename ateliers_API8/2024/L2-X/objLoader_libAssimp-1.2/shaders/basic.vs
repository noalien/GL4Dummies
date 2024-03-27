#version 330

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform int sky;


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
  if(sky != 0)
    vsoTexCoord = 2.0 * vec2(vsiTexCoord.x, vsiTexCoord.y);
  else
    vsoTexCoord = vec2(vsiTexCoord.x, 1.0 - vsiTexCoord.y);
}
