/*!\file basic.fs
 *
 * \brief fragment shader basique qui applique une couleur.
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date January 20 2017
 */
/* Version GLSL 3.30 */
#version 330
/* Le fragment shader est directement en relation avec les sorties du vertex shader */
in  vec4 vsoColor;
/* sortie du frament shader : une couleur */
out vec4 fragColor;

void main(void) {
  /* la couleur de sortie est le vsoColor linéairement interpolé au
     fragment (voir attribut flat pour désactiver l'interpolation). */
  fragColor = vsoColor;
}
