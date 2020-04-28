/*!\file basic.fs
 *
 * \brief fragment shader basique qui applique une couleur à chaque
 * fragment de l'image.
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date March 11, 2018
 */
/* Version GLSL 3.30 */
#version 330
in vec2 vsoTexCoord;

uniform sampler2D tex;

/* sortie du frament shader : une couleur */
out vec4 fragColor;

void main(void) {
  /* mettre du jaune en sortie */
  fragColor = texture(tex, vsoTexCoord);//vec4(0, vsoTexCoord, 1);
}
