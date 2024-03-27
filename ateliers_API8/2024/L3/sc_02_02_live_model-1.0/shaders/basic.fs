/* \author Farès BELHADJ */
/* Version GLSL 3.30     */
#version 330
in vec4 mp;
in vec3 vsoNormal;
/* sortie du frament shader : une couleur */
out vec4 fragColor;

const vec3 Lp = vec3(0.0, 10.0, 10.0);
const float shininess = 100.0;

uniform float temps;
uniform mat4  projectionMatrix, viewMatrix, modelMatrix;

void main(void) {
  vec3 n = normalize((transpose(inverse(modelMatrix)) * vec4(vsoNormal, 0.0)).xyz);
  /* on bouge un peu la lumière positionnelle */
  vec3 nLp = Lp + vec3(0.0, 0.0, 3.0 * sin(temps));
  vec3 Ld = normalize(mp.xyz - nLp);
  vec3 R = reflect(Ld, n);
  vec3 V = -normalize((viewMatrix * mp).xyz);
  float intensiteDuSpeculaire = pow(clamp(dot(V, R), 0.0, 1.0), shininess);
  float intensiteDeLumiereDiffuse = clamp(dot(n, -Ld), 0.0, 1.0);
  vec4 dcolor = intensiteDeLumiereDiffuse * vec4(0.5, 0.3, 0.0, 1.0);
  vec4 scolor = intensiteDuSpeculaire * vec4(1.0);
  fragColor = dcolor + scolor;
}
