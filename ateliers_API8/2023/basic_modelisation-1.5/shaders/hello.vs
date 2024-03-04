#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

out vec4 mpos;
out vec3 mnormal;
uniform mat4 proj, model, view;

void main() {
  mpos = model * vec4(pos, 1.0);
  mnormal = normalize(transpose(inverse(model)) * vec4(normal, 0.0)).xyz;
  gl_Position = proj * view * mpos;
}
