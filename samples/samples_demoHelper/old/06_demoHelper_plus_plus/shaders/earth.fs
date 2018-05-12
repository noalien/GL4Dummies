/*!\file basic.fs
 *
 * \brief rendu texturé avec lumière positionnelle diffuse et spéculaire
 * (optionnelle).
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr 
 * \date April 15 2016
 */
#version 330
uniform vec4 lumPos;
uniform mat4 modelViewMatrix;
uniform int specular;
uniform sampler2D tex0, tex1;
in  vec3 vsoNormal;
in  vec2 vsoTexCoord;
in  vec3 vsoModPos;
out vec4 fragColor;

void main(void) {
  float diffuse = 0, spec = 0;
  vec4 color = texture(tex0, vsoTexCoord);
  vec3 N = normalize(vsoNormal);
  vec3 L = normalize(vsoModPos - lumPos.xyz);
  diffuse = dot(N, -L);
  if(specular != 0) {
    vec3 R = reflect(L, N);
    vec3 V = normalize(-vsoModPos);
    spec = (0.3 + 0.7 * texture(tex1, vsoTexCoord).r) * pow(max(0, dot(R, V)), 10);
  }
  color *= diffuse;
  fragColor = spec + color;
}
