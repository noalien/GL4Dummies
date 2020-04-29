/*!\file basic.fs
 *
 * \brief fragment shader basique qui applique juste une texture.
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date February 19 2018
 */
/* Version GLSL 3.30 */
#version 330
/* texture 2 dimensions */
uniform sampler2D tex;
/* Le fragment shader est directement en relation avec les sorties du vertex shader */
in vec2 vsoTexCoord;
/* sortie du frament shader : une couleur */
out vec4 fragColor;

void main(void) {
  fragColor = texture(tex, vsoTexCoord);
}
