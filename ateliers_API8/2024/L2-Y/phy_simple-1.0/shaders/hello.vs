#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 proj;
uniform mat4 model;
uniform mat4 view;

/* incidence de la lumière pour algorithme Gouraud */
out float il;
out vec2 vsoTexCoord;

void main() {
  /* une lumière directionnelle constante */
  const vec3 Ld = vec3(0.0, -0.5, -0.866);
  vec4 n = vec4(normal, 0.0);
  n = transpose(inverse(model)) * n;
  /* le produit scalaire permet de calculer l'incidence de la lumière */
  il = clamp(dot(normalize(n.xyz), -Ld), 0.0, 1.0);
  gl_Position = proj * view * model * vec4(pos, 1.0);
  /* passer la coordonnée de texture au fragment shader */
  vsoTexCoord = vec2(texCoord.x, 1.0 - texCoord.y);
}
