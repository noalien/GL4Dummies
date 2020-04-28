/*!\file basic.fs
 *
 * \brief fragment shader basique qui applique une couleur à chaque
 * fragment de l'image.
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date March 11, 2018
 */
/* Version GLSL 3.30 */
#version 330
/* entrée du Fragment Shader, color envoyée depuis le Vertex Shader */
in vec4 color;
in vec2 vsoTexCoord;
/* sortie du frament shader : une couleur */
out vec4 fragColor;

void main(void) {
/*   if( (vsoTexCoord.x - 0.5) * (vsoTexCoord.x - 0.5) + */
/*       (vsoTexCoord.y - 0.5) * (vsoTexCoord.y - 0.5) < 0.25 ) */
/*     fragColor = color; */
  float d = length(vsoTexCoord - vec2(0.5));
  if( d < 0.5 )
    fragColor = d * color;
  else
    discard;
}
