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
/* sortie du frament shader : une couleur */
out vec4 fragColor;

void main(void) {
  /* mettre la couleur de sortie à color */
  fragColor = color;
}
