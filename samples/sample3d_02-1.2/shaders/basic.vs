#version 330

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform sampler2D ebump;
uniform int bumpMap;
layout (location = 0) in vec3 vsiPosition;
layout (location = 1) in vec3 vsiNormal;
layout (location = 2) in vec2 vsiTexCoord;
 
out vec3 vsoNormal;
out vec3 vsoPosition;
out vec3 vsoModPos;
out vec2 vsoTexCoord;

void main(void) {
  vec3 bpos = bumpMap != 0 ? vsiPosition + 0.03 * texture(ebump, vsiTexCoord).r * vsiPosition : vsiPosition;
  vec4 mp = modelViewMatrix * vec4(bpos, 1.0);
  vsoNormal = (transpose(inverse(modelViewMatrix))  * vec4(vsiNormal, 0.0)).xyz;
  vsoPosition = bpos;
  vsoModPos = mp.xyz;
  vsoTexCoord = vec2(vsiTexCoord.x, 1.0 - vsiTexCoord.y);
  gl_Position = projectionMatrix * mp;
}
