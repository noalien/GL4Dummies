#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

/* j'invente une sortie pour mon vertex shader, c'est le calcul
   d'intensité de lumière diffuse, lié à l'incidence du vecteur
   lumière sur chaque vertex (sommet) */
out float il;

out vec2 vsoTexCoord;

const vec4 Lp = vec4(0.0, 0.0, -3.0, 1.0);

void main() {
  vec4 ni = transpose(inverse(view * model)) * vec4(normal, 0.0);
  vec4 vi = view * model * vec4(pos, 1.0);
  gl_Position = proj * vi;
  vec3 Ld = (vi - Lp).xyz;
  Ld = normalize(Ld);
  il = clamp(dot(normalize(ni.xyz), -Ld), 0.0, 1.0);
  il = 0.3 + 0.7 * il; /* 30% d'ambiant */
  vsoTexCoord = 4.0 * texCoord;
}
