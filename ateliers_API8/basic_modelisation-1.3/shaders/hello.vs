#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

out float il;
uniform mat4 proj, modelView;

void main() {
  vec3 Ld = normalize(vec3(0.0, -0.2, -1.0));
  vec3 n = normalize(transpose(inverse(modelView)) * vec4(normal, 0.0)).xyz;
  il = clamp(dot(n, -Ld), 0.0, 1.0);
  gl_Position = proj * modelView * vec4(pos, 1.0);
}
