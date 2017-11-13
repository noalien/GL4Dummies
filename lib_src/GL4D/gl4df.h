/*!\file gl4df.h
 *
 * \brief The GL4Dummies filters : filtres de rendu de la bibliothèque
 * GL4Dummies.
 *
 * \author Farès BELHADJ amsi@ai.univ-paris8.fr
 * \date April 14, 2016
 *
 * \todo écrire le code pour : GL4D/gl4dfMedian.c
 * GL4D/gl4dfScattering.c GL4D/gl4dfFocus.c GL4D/gl4dfConversion.c
 * GL4D/gl4dfMedia.c GL4D/gl4dfFractalPainting.c GL4D/gl4dfHatching.c
 * GL4D/gl4dfSegmentation.c GL4D/gl4dfOpticalFlow.c
 *
 * \todo en l'état, ses fonctionnalités ne sont pas designées pour
 * être ni thread-safe ni fonctionnant avec plusieurs contextes
 * OpenGL.
 */
#ifndef _GL4DF_H
#define _GL4DF_H

#include "gl4dummies.h"

#ifdef __cplusplus
extern "C" {
#endif

  enum GL4DFenum {
    GL4DF_SOBEL_RESULT_RGB = 0,
    GL4DF_SOBEL_RESULT_INV_RGB,
    GL4DF_SOBEL_RESULT_LUMINANCE,
    GL4DF_SOBEL_RESULT_INV_LUMINANCE, /* par défault */
    GL4DF_SOBEL_MIX_NONE,
    GL4DF_SOBEL_MIX_ADD,
    GL4DF_SOBEL_MIX_MULT
  };
  typedef enum GL4DFenum GL4DFenum;
  /* Dans gl4dConversion.c */
  /*!\brief Envoie le framebuffer actif (ou l'écran) vers une texture.
   *
   *\param out pointeur vers la variable contenant l'identifiant de
   * texture dans laquelle le framebuffer (ou l'écran) sera copié. Si
   * cette identifiant (*out) est nul (0), copie le framebuffer (ou
   * l'écran) vers une nouvelle texture dont l'identifiant sera écrit
   * dans \a out et devra être libéré avec glDeleteTextures. Cette
   * nouvelle texture aura les dimensions (largeur et hauteur) du
   * viewport courant et le filtre utilisé est celui choisi à l'aide
   * de \ref gl4dfConvSetFilter.
   */
  GL4DAPI void GL4DAPIENTRY gl4dfConvFrame2Tex(GLuint * out);
  /*!\brief Envoie une texture identifiée par \a in vers le framebuffer actif (ou écran).
   *
   *\param in identifiant OpenGL de la texture à copier dans le framebuffer (ou l'écran).
   */
  GL4DAPI void GL4DAPIENTRY gl4dfConvTex2Frame(GLuint in);
  /*!\brief Indique le filtre à utiliser en cas d'interpolation du
   * Blit. Le filtre par défaut est GL_NEAREST.
   *
   *\param filter est le filtre d'interpolation (GL_LINEAR ou GL_NEAREST).
   */
  GL4DAPI void GL4DAPIENTRY gl4dfConvSetFilter(GLenum filter);
  /*!\brief Copie la texture identifiée par \a in dans \a out. Le mode
   * d'interpolation (par défaut GL_NEAREST) est à renseigner à l'aide
   * de la fonction \ref gl4dfConvSetFilter.
   *
   *\param in identifiant GL de la texture en entrée.
   *\param out identifiant GL de la texture en sortie.
   *\param flipV indique s'il est nécessaire d'effectuer un mirroir vertical lors de la copie.
   *\see gl4dfConvSetFilter
   */
  GL4DAPI void GL4DAPIENTRY gl4dfConvTex2Tex(GLuint in, GLuint out, GLboolean flipV);
  /* Dans gl4dfBlur.c */
  /*!\brief Indique la composante R (=0), G (=1), B (=2) ou A (=3) à
   * utiliser dans la weight map.
   *
   *\param weightMapComponent le numéro de la composante : 0, 1, 2 ou
   *3. La valeur par défaut est 0.
   */
  GL4DAPI void GL4DAPIENTRY gl4dfBlurSetWeightMapComponent(GLuint weightMapComponent);
  /*!\brief Indique la translation de la valeur appliquée depuis la weight map.
   *
   *\param weightMapTranslate la translation à appliquer.
   */
  GL4DAPI void GL4DAPIENTRY gl4dfBlurSetWeightMapTranslate(GLfloat weightMapTranslate);
  /*!\brief Indique le scale de la valeur appliquée depuis la weight map.
   *
   *\param weightMapScale le scale à appliquer.
   */
  GL4DAPI void GL4DAPIENTRY gl4dfBlurSetWeightMapScale(GLfloat weightMapScale);
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
  /* Dans gl4dfMedian.c */
  /*!\brief Filtre 2D médian 3x3.
   *
   *\param in identifiant de texture source. Si 0, le framebuffer écran est pris à la place.
   *\param out identifiant de texture destination. Si 0, la sortie s'effectuera à l'écran.
   *\param nb_iterations le nombre d'itérations de filtre médian.
   *\param flipV indique s'il est nécessaire d'effectuer un mirroir vertical du résultat.
   */
  GL4DAPI void GL4DAPIENTRY gl4dfMedian(GLuint in, GLuint out, GLuint nb_iterations, GLboolean flipV);
  /* Dans gl4dfScattering.c */
  /*!\brief Filtre 2D de mélange de pixels (éparpillement) 
   *
   *\param in identifiant de texture source. Si 0, le framebuffer écran est pris à la place.
   *\param out identifiant de texture destination. Si 0, la sortie s'effectuera à l'écran.
   *\param radius rayon de l'éparpillement autour de chaque pixel.
   *\param displacementmap identifiant de texture (niveaux de gris) à
   * utiliser pour le suivi du bruit (éviter le showerdoor effect). Si
   * 0, aucune texture de deplacement n'est utilisée.
   *\param weightmap identifiant de texture (niveaux de gris) à utiliser pour pondérer le rayon d'éparpillement. Si 0, aucune pondération n'est appliquée.
   *\param flipV indique s'il est nécessaire d'effectuer un mirroir vertical du résultat.
   */
  GL4DAPI void GL4DAPIENTRY gl4dfScattering(GLuint in, GLuint out, GLuint radius, GLuint displacementmap, GLuint weightmap, GLboolean flipV);
  /*!\brief Force le changement de la carte de mélange (éparpillement) utilisée. */
  GL4DAPI void GL4DAPIENTRY gl4dfScatteringChange(void);
  /* Dans gl4dfSobel.c */
  /*!\brief Filtre 2D Sobel (détection des contours) 
   *
   *\param in identifiant de texture source. Si 0, le framebuffer écran est pris à la place.
   *\param out identifiant de texture destination. Si 0, la sortie s'effectuera à l'écran.
   *\param flipV indique s'il est nécessaire d'effectuer un mirroir vertical du résultat.
   */
  GL4DAPI void GL4DAPIENTRY gl4dfSobel(GLuint in, GLuint out, GLboolean flipV);
  /* brief Indique la couleur multiplicative du Sobel. Par défaut blanc ({1, 1, 1, 1}) est utilisé. 
   *
   * param vec4Color la couleur multiplicative (tableau de 4 flottants).
   */
  /* GL4DAPI void GL4DAPIENTRY gl4dfSobelSetColor(GLfloat * vec4Color); */
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

  /*!\brief A documenter
   *
   *\todo A documenter
   */
  GL4DAPI void GL4DAPIENTRY gl4dfMCMD(GLuint in, GLuint out, GLboolean flipV);
  /*!\brief A documenter
   *
   *\todo A documenter
   */
  GL4DAPI void GL4DAPIENTRY gl4dfMCMDDimensions(GLuint, GLuint);
  /*!\brief A documenter
   *
   *\todo A documenter
   */
  GL4DAPI void GL4DAPIENTRY gl4dfMCMDTakeColor(GLboolean mcmd_take_color);
  /*!\brief A documenter
   *
   *\todo A documenter
   */
  GL4DAPI void GL4DAPIENTRY gl4dfMCMDChangeSeed(GLboolean change_seed);
  /*!\brief A documenter
   *
   *\todo A documenter
   */
  GL4DAPI void GL4DAPIENTRY gl4dfMCMDSetSkeletonize(GLboolean skeletonize);
  /*!\brief A documenter
   *
   * Ir > 0 extrapolation vers le bas, Ir < 0 extrapolation vers le
   * haut. Ir nul extrapolation en maintenant la même altitude. |Ir| < 1
   * croissance ou décroissance lente. |Ir| < 1 croissance ou
   * décroissance rapide. |Ir| égale à 1 croissance ou décroissance
   * linéaire.
   *\todo A documenter
   */
  GL4DAPI void GL4DAPIENTRY gl4dfMCMDSetIr(const GLfloat * mcmd_Ir);
  /*!\brief A documenter
   *
   *\todo A documenter
   */
  GL4DAPI void GL4DAPIENTRY gl4dfMCMDSetI(const GLfloat * mcmd_I);
  /*!\brief A documenter
   *
   *\todo A documenter
   */
  GL4DAPI void GL4DAPIENTRY gl4dfMCMDSetNoiseH(const GLfloat * mcmd_noise_H);
  /*!\brief A documenter
   *
   *\todo A documenter
   */
  GL4DAPI void GL4DAPIENTRY gl4dfMCMDSetNoiseS(const GLfloat * mcmd_noise_S);
  /*!\brief A documenter
   *
   *\todo A documenter
   */
  GL4DAPI void GL4DAPIENTRY gl4dfMCMDSetNoiseT(const GLfloat * mcmd_noise_T);
  /*!\brief A documenter
   *
   *\todo A documenter
   */
  GL4DAPI void GL4DAPIENTRY gl4dfMCMDSetNoisePhaseChange(const GLfloat * mcmd_noise_phase_change);
  /*!\brief A documenter
   *
   *\todo A documenter
   */
  GL4DAPI void GL4DAPIENTRY gl4dfMCMDSetSkeletonizeRandThreshold(GLfloat rand_threshold);

#ifdef __cplusplus
}
#endif

#endif
