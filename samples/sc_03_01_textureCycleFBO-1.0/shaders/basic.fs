/*!\file basic.fs
 *
 * \brief fragment shader basique qui applique une texture en se
 * décalant d'un texel. Comme cela est fait de manière cyclique, nous
 * obtenons une translation de la texture.
 * \author Farès BELHADJ, amsi@up8.edu
 * \date July 15 2020
 */
/* Version GLSL 3.30 */
#version 330
/* texture 2 dimensions */
uniform sampler2D tex;
/* les pas pour passer au texel suivant ; soit en x soit en y */
uniform vec2 step;
/* Le fragment shader est directement en relation avec les sorties du vertex shader */
in vec2 vsoTexCoord;
/* sortie du frament shader : une couleur */
out vec4 fragColor;

void main(void) {
  fragColor = texture(tex, vsoTexCoord - vec2(step.x, 0));
}
