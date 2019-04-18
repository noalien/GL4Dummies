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
    GL4DF_SOBEL_MIX_MULT,
    GL4DF_OP_ADD,
    GL4DF_OP_SUB,
    GL4DF_OP_MULT,
    GL4DF_OP_DIV,
    GL4DF_OP_OVERLAY,
    GL4DF_CANNY_RESULT_RGB,
    GL4DF_CANNY_RESULT_INV_RGB,
    GL4DF_CANNY_RESULT_LUMINANCE,
    GL4DF_CANNY_RESULT_INV_LUMINANCE, /* par défault */
    GL4DF_CANNY_MIX_NONE,
    GL4DF_CANNY_MIX_ADD,
    GL4DF_CANNY_MIX_MULT,
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
  /*!\brief A documenter
   *\param version 0 pour cpu-original-like, 1 (et plus, plus tard ...) pour nouvelles
   *\todo A documenter
   */
  GL4DAPI void GL4DAPIENTRY gl4dfMCMDSetMDBUVersion(GLuint version);
  /*!\brief Permet de modifier la méthode de subdivision utilisée : 0
   * Triangle-Edge, 1 Diamond-Square.
   *\param method si 0 subdivision Triangle-Edge, si 1 subdivision
   * Diamond-Square. */
  GL4DAPI void GL4DAPIENTRY gl4dfMCMDSetSubdivisionMethod(GLuint method);
  /*!\brief Permet d'indiquer l'usage ou non d'une map (texture) pour
   * récupérer les valeurs locales de roughness.
   *
   * \param map_tex_id si 0, pas de roughness locale (la globale est
   * utilisée, voir \ref gl4dfMCMDSetNoiseH), sinon l'identifiant de
   * la texture servira de map de roughness. */
  GL4DAPI void GL4DAPIENTRY gl4dMCMDSetUseRoughnessMap(GLuint map_tex_id);
  /*!\brief Permet d'indiquer l'usage ou non d'une map (texture) pour
   * récupérer les valeurs locales d'interpolation.
   *
   * \param map_tex_id si 0, pas de valeurs locales d'interpolation
   * (la globale est utilisée, voir \ref gl4dfMCMDSetI), sinon
   * l'identifiant de la texture servira de map d'interpolation. */
  GL4DAPI void GL4DAPIENTRY gl4dMCMDSetUseInterpolationMap(GLuint map_tex_id);
  /*!\brief Permet d'indiquer l'usage ou non d'une map (texture) pour
   * récupérer les valeurs locales d'extrapolation.
   *
   * \param map_tex_id si 0, pas de valeurs locale d'extrapolation (la
   * globale est utilisée, voir \ref gl4dfMCMDSetIr), sinon
   * l'identifiant de la texture servira de map d'extrapolation. */
  GL4DAPI void GL4DAPIENTRY gl4dMCMDSetUseExtrapolationMap(GLuint map_tex_id);
  /*!\brief Permet d'indiquer l'usage ou non d'une map (texture) pour
   * récupérer les valeurs locales de noise scale.
   *
   * \param map_tex_id si 0, pas de valeurs locale de noise scale (la
   * globale est utilisée, voir \ref gl4dfMCMDSetNoiseS), sinon
   * l'identifiant de la texture servira de map de noise scale. */
  GL4DAPI void GL4DAPIENTRY gl4dMCMDSetUseNoiseScaleMap(GLuint map_tex_id);
  /*!\brief Permet d'indiquer l'usage ou non d'une map (texture) pour
   * récupérer les valeurs locales de noise translate.
   *
   * \param map_tex_id si 0, pas de valeurs locale de noise translate (la
   * globale est utilisée, voir \ref gl4dfMCMDSetNoiseS), sinon
   * l'identifiant de la texture servira de map de noise translate. */
  GL4DAPI void GL4DAPIENTRY gl4dMCMDSetUseNoiseTranslateMap(GLuint map_tex_id);
  /*!\brief Réalise un mélange entre deux textures (ou écran) en
   * entrée et l'écrit dans une texture ou à l'écran en
   * sortie. L'opération choisie pour le mélange est choisie à l'aide
   * de la fonction \ref gl4dfOpSetOp ; l'opération par défaut est
   * l'addition avec la valeur GL4DF_OP_ADD.
   *
   * \param in1 identifiant de la première texture en entrée (peut être 0 si écran).
   * \param in2 identifiant de la seconde texture en entrée (peut être 0 si écran).
   * \param out identifiant de la texture de sortie (peut être 0 si écran).
   * \param flipV effectuer un flip vertical si GL_TRUE.
   */
  GL4DAPI void GL4DAPIENTRY gl4dfOp(GLuint in1, GLuint in2, GLuint out, GLboolean flipV);
  /*!\brief Permet de choisir l'opération pour le mélange effectué
   * avec l'aide de la fonction \ref gl4dfOp ; l'opération par défaut
   * est l'addition avec la valeur GL4DF_OP_ADD.
   * \param op valeur correspondant à l'opération qui sera
   * effectuée. Peut être une parmi : GL4DF_OP_ADD (addition),
   * GL4DF_OP_SUB (soustraction), GL4DF_OP_MULT (multiplication),
   * GL4DF_OP_DIV (division), GL4DF_OP_OVERLAY (overlay).
   */
  GL4DAPI void GL4DAPIENTRY gl4dfOpSetOp(GL4DFenum op);

  /* Dans gl4dfCanny.c */
  /*!\brief Filtre 2D Canny (détection des contours) 
   *
   *\param in identifiant de texture source. Si 0, le framebuffer écran est pris à la place.
   *\param out identifiant de texture destination. Si 0, la sortie s'effectuera à l'écran.
   *\param flipV indique s'il est nécessaire d'effectuer un mirroir vertical du résultat.
   */
  GL4DAPI void GL4DAPIENTRY gl4dfCanny(GLuint in, GLuint out, GLboolean flipV);
  /* brief Indique la couleur multiplicative du Canny. Par défaut blanc ({1, 1, 1, 1}) est utilisé. 
   *
   * param vec4Color la couleur multiplicative (tableau de 4 flottants).
   */
  /* GL4DAPI void GL4DAPIENTRY gl4dfCannySetColor(GLfloat * vec4Color); */
  /*!\brief Fonction liée au filtre Canny. Méthode de calcul du résultat final du Canny
   *
   *\param mode indique les différents modes possibles. Plusieurs
   * choix sont disponibles:\n
   * - GL4DF_CANNY_RESULT_RGB : le résultat du Canny est laissé inchangé (chaque composante contien son Canny) ;\n
   * - GL4DF_CANNY_RESULT_INV_RGB : idem que la précédente avec une inversion (1.0 - s) ;\n
   * - GL4DF_CANNY_RESULT_LUMINANCE : le résultat est transformé en Luminance sur les 3 composantes ;\n
   * - GL4DF_CANNY_RESULT_INV_LUMINANCE : mode par défault, le résultat est l'inverse de la Luminance sur les 3 composantes.
   */
  GL4DAPI void GL4DAPIENTRY gl4dfCannySetResultMode(GL4DFenum mode);
  /*!\brief Fonction liée au filtre Canny. Méthode de combinaison du résultat final du Canny avec l'image d'origine
   *
   *\param mode indique les différents modes possibles. Plusieurs
   * choix sont disponibles:\n
   * - GL4DF_CANNY_MIX_NONE : le Canny est seul ;\n
   * - GL4DF_CANNY_MIX_ADD : le Canny est pondéré et additionné à l'image d'origine (utilise la fonction GLSL mix). Le facteur de pondération est fixé par \ref gl4dfCannySetMixFactor;\n
   * - GL4DF_CANNY_MIX_MULT : le Canny est multiplié par l'image d'origine.
   */
  GL4DAPI void GL4DAPIENTRY gl4dfCannySetMixMode(GL4DFenum mode);
  /*!\brief Fonction liée au filtre Canny. Modification du facteur de mix utilisé par le mode GL4DF_CANNY_MIX_ADD
   *
   *\param factor facteur de mélange compris entre 0 et 1
   *\see gl4dfCannySetMixMode
   */
  GL4DAPI void GL4DAPIENTRY gl4dfCannySetMixFactor(GLfloat factor);
  /*!\brief Fonction liée au filtre Canny. Modification des seuils :
   * bas (\a lowTh) et haut (\a highTh).
   *
   *\param lowTh seuil bas du filtre Canny
   *\param highTh seuil haut du filtre Canny
   */
  GL4DAPI void GL4DAPIENTRY gl4dfCannySetThresholds(GLfloat lowTh, GLfloat highTh);

#ifdef __cplusplus
}
#endif

#endif
