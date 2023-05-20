#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 proj, modView;

uniform sampler2D tex;

out vec2 tCoord;

void main() {
  vec3 p = pos + vec3(0.0, texture(tex, texCoord).r / 4.0, 0.0);
  gl_Position = proj * modView * vec4(p, 1.0);
  tCoord = texCoord;
}
