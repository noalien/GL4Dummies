#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

out float il;
uniform mat4 proj, model, view;
uniform vec4 lumpos;

void main() {
  vec4 mpos = model * vec4(pos, 1.0);
  vec3 Ld = normalize(mpos.xyz - lumpos.xyz);
  vec3 n = normalize(transpose(inverse(model)) * vec4(normal, 0.0)).xyz;
  il = clamp(dot(n, -Ld), 0.0, 1.0);
  gl_Position = proj * view * mpos;
}
