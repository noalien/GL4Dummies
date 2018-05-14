/*!\file effet.fs
 *
 * \brief rendu avec effet sur texture 2D.
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr 
 * \date May 14 2018
 */
#version 330
uniform sampler2D tex;
uniform float temps;
in  vec2 vsoTexCoord;
out vec4 fragColor;

void main(void) {
  vec2 vecteur = vsoTexCoord - vec2(0.5);
  float distance = length(vecteur);
  float angle = atan(vecteur.y, vecteur.x);
  /* c'est la ligne qui suit qui change tout ! */
  angle +=  0.001 * temps / (1.0 + distance);
  vec2 tc = vec2(0.5) + vec2(distance * cos(angle), distance * sin(angle));
  fragColor = texture(tex, tc);
}
