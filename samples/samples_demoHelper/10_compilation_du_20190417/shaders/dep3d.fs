/*!\file dep3d.fs
 *
 * \brief rendu avec lumière directionnelle diffuse et couleur.
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr 
 * \date April 15 2016
 */
#version 330
uniform sampler2D tex;
in  vec3 vsoNormal;
in  vec2 vsoTexCoord;
out vec4 fragColor;

void main(void) {
  vec3 N = normalize(vsoNormal);
  vec3 L = normalize(vec3(-1, -1, -1)); /*vers le bas vers la gauche*/
  float diffuse = dot(N, -L);
  fragColor = vec4(texture(tex,vsoTexCoord).rgb * vec3(0.2 + diffuse), 1.0);
}
