/*!\file gl4df.h
 *
 * \brief The GL4Dummies filters : filtres de rendu de la bibliothèque
 * GL4Dummies.
 *
 * \author Farès BELHADJ amsi@ai.univ-paris8.fr
 * \date April 14, 2016
 * 
 */
#ifndef _GL4DF_H
#define _GL4DF_H

#include <GL4D/gl4dummies.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef enum GL4DFenum GL4DFenum;
  enum GL4DFenum {
    GL4DF_SOBEL_RESULT_RGB = 0,
    GL4DF_SOBEL_RESULT_INV_RGB,
    GL4DF_SOBEL_RESULT_LUMINANCE,
    GL4DF_SOBEL_RESULT_INV_LUMINANCE, /* par défault */
    GL4DF_SOBEL_MIX_NONE,
    GL4DF_SOBEL_MIX_ADD,
    GL4DF_SOBEL_MIX_MULT
  };

  /* Dans gl4dfBlur.c */
  /*!\brief Filtre 2D de flou Gaussien 
   *
   *\param in identifiant de texture source. Si 0, le framebuffer écran est pris à la place.
   *\param out identifiant de texture destination. Si 0, la sortie s'effectuera à l'écran.
   *\param radius rayon du masque de convolution Gaussien à appliquer (de 0 à 128).
   *\param nb_iterations le nombre d'itérations de flou.
   *\param weight identifiant de texture (niveaux de gris) à utiliser pour pondérer le rayon de flou. Si 0, aucune pondération n'est appliquée.
   *\param flipV indique s'il est nécessaire d'effectuer un mirroir vertical du résultat.
   */
  GL4DAPI void GL4DAPIENTRY gl4dfBlur(GLuint in, GLuint out, GLuint radius, GLuint nb_iterations, GLuint weight, GLboolean flipV);
  /* Dans gl4dfSobel.c */
  /*!\brief Filtre 2D Sobel (détection des contours) 
   *
   *\param in identifiant de texture source. Si 0, le framebuffer écran est pris à la place.
   *\param out identifiant de texture destination. Si 0, la sortie s'effectuera à l'écran.
   *\param flipV indique s'il est nécessaire d'effectuer un mirroir vertical du résultat.
   */
  GL4DAPI void GL4DAPIENTRY gl4dfSobel(GLuint in, GLuint out, GLboolean flipV);
  /*!\brief Fonction liée au filtre Sobel. Méthode de calcul du résultat final du Sobel
   *
   *\param mode indique les différents modes possibles. Plusieurs
   * choix sont disponibles:\n
   * - GL4DF_SOBEL_RESULT_RGB : le résultat du Sobel est laissé inchangé (chaque composante contien son Sobel) ;\n
   * - GL4DF_SOBEL_RESULT_INV_RGB : idem que la précédente avec une inversion (1.0 - s) ;\n
   * - GL4DF_SOBEL_RESULT_LUMINANCE : le résultat est transformé en Luminance sur les 3 composantes ;\n
   * - GL4DF_SOBEL_RESULT_INV_LUMINANCE : mode par défault, le résultat est l'inverse de la Luminance sur les 3 composantes.
   */
  GL4DAPI void GL4DAPIENTRY gl4dfSobelSetResultMode(GL4DFenum mode);
  /*!\brief Fonction liée au filtre Sobel. Méthode de combinaison du résultat final du Sobel avec l'image d'origine
   *
   *\param mode indique les différents modes possibles. Plusieurs
   * choix sont disponibles:\n
   * - GL4DF_SOBEL_MIX_NONE : le Sobel est seul ;\n
   * - GL4DF_SOBEL_MIX_ADD : le Sobel est pondéré et additionné à l'image d'origine (utilise la fonction GLSL mix). Le facteur de pondération est fixé par \ref gl4dfSobelSetMixFactor;\n
   * - GL4DF_SOBEL_MIX_MULT : le Sobel est multiplié par l'image d'origine.
   */
  GL4DAPI void GL4DAPIENTRY gl4dfSobelSetMixMode(GL4DFenum mode);
  /*!\brief Fonction liée au filtre Sobel. Modification du facteur de mix utilisé par le mode GL4DF_SOBEL_MIX_ADD
   *
   *\param factor facteur de mélange compris entre 0 et 1
   *\see gl4dfSobelSetMixMode
   */
  GL4DAPI void GL4DAPIENTRY gl4dfSobelSetMixFactor(GLfloat factor);

#ifdef __cplusplus
}
#endif

#endif
