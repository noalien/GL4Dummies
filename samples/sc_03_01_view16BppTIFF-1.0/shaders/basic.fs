/*!\file basic.fs
 *
 * \brief fragment shader <<basique>> qui applique une texture où
 * seule la composante rouge est renseignée. L'application se fait à
 * l'aide d'une LUT stockée dans une autre texture.  
 *
 * \author Farès BELHADJ, amsi@up8.edu 
 * \date July 13 2020
 */
/* Version GLSL 3.30 */
#version 330
/* texture 2 dimensions (l'image chargée) */
uniform sampler2D tex;
/* texture 1 dimension pour la LUT */
uniform sampler1D lut;
/* utiliser ou non la LUT */
uniform int useLUT;
/* normaliser ou non */
uniform int normalize;
/* valeurs min et max pour normaliser */
uniform float minv, maxv;
/* Coordonnées de texture interpolées depuis le vertex shader */
in vec2 vsoTexCoord;
/* sortie du frament shader : une couleur */
out vec4 fragColor;

void main(void) {
  float intensite;
  if(normalize != 0)
    intensite = (texture(tex, vsoTexCoord).r - minv) / (maxv - minv);
  else
    intensite = texture(tex, vsoTexCoord).r;
  if(useLUT == 0)
    fragColor = vec4(intensite);
  else
    fragColor = texture(lut, intensite);
}
