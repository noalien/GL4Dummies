/* \author Farès BELHADJ */
/* Version GLSL 3.30     */
#version 330
in vec4 mp;
in vec3 vsoNormal;
in vec3 vsoPos;
/* sortie du frament shader : une couleur */
out vec4 fragColor;

vec3 Lp = vec3(0.0, 3.0, -4.5);

uniform float temps;
uniform mat4  projectionMatrix, viewMatrix, modelMatrix;

const float PI = 3.1415;
const float freq = 3.0;

void main(void) {
  float x = freq * 2.0 * PI * length(vsoPos.xy);
  float cx = cos(x);
  vec3 n = normalize((transpose(inverse(modelMatrix)) * vec4(vsoNormal, 0.0)).xyz);
  vec3 Ld = normalize(mp.xyz - Lp);
  float intensiteDeLumiereDiffuse = clamp(dot(n, -Ld), 0.0, 1.0);
  vec4 color = intensiteDeLumiereDiffuse * vec4(1.0);
  fragColor = color;
}
