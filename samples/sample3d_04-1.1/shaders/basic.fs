/*!\file basic.fs
 *
 * \brief rendu avec lumière directionnelle diffuse et couleur mais
 * aussi renvoi d'identifiants d'objets passés comme uniform.
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr 
 * \date March 10 2017
 */
#version 330
uniform mat4 modelViewMatrix;
uniform vec4 couleur;
uniform int id, nb_mobiles;
in  vec3 vsoNormal;
in  vec3 vsoModPos;
layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 fragId;

void main(void) {
  vec3 N = normalize(vsoNormal);
  vec3 L = normalize(vec3(-1, -1, 0)); /*vers le bas vers la gauche*/
  float diffuse = dot(N, -L);
  fragColor = vec4((couleur.rgb * diffuse), couleur.a);
  fragId = vec4(float(id) / (float(nb_mobiles) + 1.0));
}
