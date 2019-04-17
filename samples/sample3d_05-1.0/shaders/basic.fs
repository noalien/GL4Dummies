/*!\file basic.fs
 *
 * \brief rendu avec lumière positionnelle diffuse et couleur mais
 * aussi renvoi d'identifiants d'objets passés comme uniform. Aussi,
 * calcul de l'ombre selon une shadow map (smTex) calculée par une
 * précédente passe.
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr 
 * \date March 10 2017
 */
#version 330 core
uniform vec4 couleur, lumpos;
uniform int id, nb_mobiles;
uniform sampler2D smTex;
in  vec4 vsoNormal;
in  vec4 vsoMVPos;
in  vec4 vsoSMCoord;
layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 fragId;

void main(void) {
  if(id == 2) { /* lumière positionnelle */
    fragColor = vec4(1, 1, 0.5, 1);
  } else {
    vec3 N = normalize(vsoNormal.xyz);
    vec3 L = normalize(vsoMVPos.xyz - lumpos.xyz);
    vec3 projCoords = vsoSMCoord.xyz / vsoSMCoord.w;
    float diffuse = dot(N, -L);
    if(id != 1) {
      if(diffuse < 0.3)
	diffuse = 0.1;
      else if(diffuse < 0.6)
	diffuse = 0.5;
      else if(diffuse < 0.9)
	diffuse = 0.75;
      else
	diffuse = 1.0;
    }
    if(texture(smTex, projCoords.xy).r  <  projCoords.z)
      diffuse *= 0.0; 
    fragColor = vec4((couleur.rgb * diffuse), couleur.a);
  }
  fragId = vec4(float(id) / (float(nb_mobiles) + 2.0));
}
