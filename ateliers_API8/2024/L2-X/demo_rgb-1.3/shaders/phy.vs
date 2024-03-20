#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

/* j'invente une sortie pour mon vertex shader, c'est le calcul
   d'intensité de lumière, lié à l'incidence du vecteur lumière sur
   chaque vertex (sommet) */
out float il;

out vec2 vsoTexCoord;

const vec3 Ld = vec3(0.0, -0.7071, -0.7071);

void main() {
  vec4 n = transpose(inverse(model)) * vec4(normal, 0.0);
  gl_Position = proj * view * model * vec4(pos, 1.0);
  /* vec3(1.0) = vec3(1.0, 1.0, 1.0) = vec3(vec2(1.0), 1.0) */
  il = clamp(dot(normalize(n.xyz), -Ld), 0.0, 1.0);
  vsoTexCoord = 4.0 * texCoord;
}
