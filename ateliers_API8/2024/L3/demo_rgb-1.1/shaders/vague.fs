/*!\file basic.fs
 *
 * \brief fragment shader basique qui applique une couleur.
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date January 20 2017
 */
/* Version GLSL 3.30 */
#version 330
in vec4 mp;
in vec3 vsoNormal;
in vec3 vsoPos;
/* sortie du frament shader : une couleur */
out vec4 fragColor;

vec3 Lp = vec3(0.0, 0.0, 3.5);

uniform float temps;
uniform mat4 projectionMatrix, viewMatrix, modelMatrix;

const float PI = 3.1415;
const float freq = 3.0;

void main(void) {
  float x = freq * 2.0 * PI * (length(vsoPos.xy) - temps);
  float cx = cos(x);
  vec3 n = vsoNormal + vec3(cx * 0.5, 0.0, 0.0);
  n = normalize((transpose(inverse(modelMatrix)) * vec4(n, 0.0)).xyz);
  Lp.x += 1.0 * sin(temps);
  vec3 Ld = normalize(mp.xyz - Lp);
  float intensiteDeLumiereDiffuse = clamp(dot(n, -Ld), 0.0, 1.0);
  vec4 color = intensiteDeLumiereDiffuse * vec4(1.0);
  /* la couleur de sortie est le vsoColor linéairement interpolé au
     fragment (voir attribut flat pour désactiver l'interpolation). */
  fragColor = color;
}
