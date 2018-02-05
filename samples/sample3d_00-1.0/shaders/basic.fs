/*!\file basic.fs
 *
 * \brief fragment shader basique qui applique une texture et
 * une couleur.
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date January 20 2017
 */
/* Version GLSL 3.30 */
#version 330
/* texture 2 dimensions */
uniform sampler2D myTexture;
/* Le fragment shader est directement en relatrion avec les sorties du vertex shader */
in  vec2 vsoTexCoord;
in  vec3 vsoColor;
/* sortie du frament shader : une couleur */
out vec4 fragColor;

void main(void) {
  /* mélange entre la texture et la couleur.*/
  fragColor = vec4(texture(myTexture, vsoTexCoord).rgb + vsoColor, 1.0);
}
