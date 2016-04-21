/*!\file gl4dg.h
 *
 * \brief The GL4Dummies Geometry
 *
 * \author Fares BELHADJ amsi@ai.univ-paris8.fr
 * \date February 22, 2016
 */

#ifndef _GL4DG_H
#define _GL4DG_H

#include "gl4dummies.h"

#ifdef __cplusplus
extern "C" {
#endif
  /*!\brief Initialise les structures nécessaire au stockage des
   * géométries proposées par GL4Dummies. Cette fonction est appelée
   * par la fonction \ref gl4duInit, utilisez donc cette dernière. 
   *
   * Chaque géométrie générée via les fonctions gl4dgGenXXXXXX peut
   * être libérée si nécessaire par gl4dgDelete en lui passant
   * l'identifiant de l'objet-géométrie obtenu. Sinon utiliser, en fin
   * de programme, \ref gl4dgClean ou \ref gl4duClean avec l'argument GL4DU_GEOMETRY ou
   * GL4DU_ALL.  
   *\see gl4duInit 
   *\see gl4duClean avec l'argument GL4DU_GEOMETRY ou GL4DU_ALL
   *\see gl4dgClean
   */
  GL4DAPI void      GL4DAPIENTRY gl4dgInit(void);
  /*!\brief Libère les structures nécessaire au stockage des
   * géométries proposées par GL4Dummies en supprimant aussi tous les
   * objets produits. Cette fonction est appelée par la fonction \ref
   * gl4duClean avec l'argument GL4DU_GEOMETRY ou GL4DU_ALL, utilisez
   * donc cette dernière.
   *
   *\see gl4duClean avec l'argument GL4DU_GEOMETRY ou GL4DU_ALL
   *\see gl4dgInit
   */
  GL4DAPI void      GL4DAPIENTRY gl4dgClean(void);
  /*!\brief Modifie le niveau d'optimisation de certaines des
   * géométries à générer (exemple le torus, la grid et la sphère).
   * 
   * \param level trois niveaux sont possibles : 0 fabriquer surtout
   * des triangles, 1 (par défaut) tenter de faire des
   * triangle_strips, 2 essayer de tout faire en une seule strip.
   */
  GL4DAPI void      GL4DAPIENTRY gl4dgSetGeometryOptimizationLevel(GLuint level);
  /*!\brief Renvoie l'identifiant du Vertex Array Object correspondant
   * à l'objet-géométrie référencé par \a id.
   * 
   * \param id la référence de l'objet-géométrie généré par cette bibliothèque.
   * \return l'identifiant du Vertex Array Object correspondant à
   * l'objet-géométrie référencé par \a id.
   */
  GL4DAPI GLuint    GL4DAPIENTRY gl4dgGetVAO(GLuint id);
  /*!\brief Génère un objet-géométrie de type Quad (plan vertical en
   * z=0) et renvoie son identifiant (référence).
   * 
   * La géométrie est décrite par 4 sommets reliés par un triangle_strip.
   *
   * \return l'identifiant (référence) de l'objet-géométrie généré.
   * \see gl4dgDraw pour le dessiner.
   * \see gl4dgDraw pour le détruire.
   */
  GL4DAPI GLuint    GL4DAPIENTRY gl4dgGenQuadf(void);
  /*!\brief Génère un objet-géométrie de type cube et renvoie son identifiant (référence).
   * 
   * Ce cube est composé de 6 plans meshés par des triangle_strips. Le normales sont aux plans.
   *
   * \return l'identifiant (référence) de l'objet-géométrie généré.
   * \see gl4dgDraw pour le dessiner.
   * \see gl4dgDraw pour le détruire.
   */
  GL4DAPI GLuint    GL4DAPIENTRY gl4dgGenCubef(void);
  /*!\brief Génère un objet-géométrie de type sphère et renvoie son identifiant (référence).
   * 
   * Cette sphère est obtenue par transformation des coordonnées polaires en coordonnées cartésiennes.
   *
   * \param slices le nombre de longitudes de la sphère.
   * \param stacks le nombre de latitudes de la sphère.
   * \return l'identifiant (référence) de l'objet-géométrie généré.
   * \see gl4dgDraw pour le dessiner.
   * \see gl4dgDraw pour le détruire.
   * \see gl4dgSetGeometryOptimizationLevel
   */
  GL4DAPI GLuint    GL4DAPIENTRY gl4dgGenSpheref(GLuint slices, GLuint stacks);
  /*!\brief Génère un objet-géométrie de type cone et renvoie son identifiant (référence).
   * 
   * Le cone est meshé à l'aide d'un triangle_strip, sa base est faite d'un triangle_fan.
   *
   * \param slices le nombre de longitudes du cone.
   * \param base indique s'il y a ou pas une base au cone.
   * \return l'identifiant (référence) de l'objet-géométrie généré.
   * \see gl4dgDraw pour le dessiner.
   * \see gl4dgDraw pour le détruire.
   */
  GL4DAPI GLuint    GL4DAPIENTRY gl4dgGenConef(GLuint slices, GLboolean base);
  /*!\brief Génère un objet-géométrie de type fan-cone et renvoie son identifiant (référence).
   * 
   * Le cone et sa base sont meshés par triangle_fan.
   *
   * \param slices le nombre de longitudes du cone.
   * \param base indique s'il y a ou pas une base au cone.
   * \return l'identifiant (référence) de l'objet-géométrie généré.
   * \see gl4dgDraw pour le dessiner.
   * \see gl4dgDraw pour le détruire.
   */
  GL4DAPI GLuint    GL4DAPIENTRY gl4dgGenFanConef(GLuint slices, GLboolean base);
  /*!\brief Génère un objet-géométrie de type cylindre et renvoie son identifiant (référence).
   * 
   * Le cylindre est meshé à l'aide d'un triangle_strip, ses bases sont faites de triangle_fans.
   *
   * \param slices le nombre de longitudes du cylindre.
   * \param base indique s'il y a ou pas deux bases au cylindre (top et bas).
   * \return l'identifiant (référence) de l'objet-géométrie généré.
   * \see gl4dgDraw pour le dessiner.
   * \see gl4dgDraw pour le détruire.
   */
  GL4DAPI GLuint    GL4DAPIENTRY gl4dgGenCylinderf(GLuint slices, GLboolean base);
  /*!\brief Génère un objet-géométrie de type disque et renvoie son identifiant (référence).
   * 
   * Le disque est composé d'un triangle_fan, il est en y=0 et est tourné vers le haut.
   *
   * \param slices le nombre de longitudes du disque.
   * \return l'identifiant (référence) de l'objet-géométrie généré.
   * \see gl4dgDraw pour le dessiner.
   * \see gl4dgDraw pour le détruire.
   */
  GL4DAPI GLuint    GL4DAPIENTRY gl4dgGenDiskf(GLuint slices);
  /*!\brief Génère un objet-géométrie de type tore et renvoie son identifiant (référence).
   * 
   * \param slices le nombre de longitudes du tore.
   * \param stacks le nombre de "latitudes" (subdivisions en coupe verticale) du tore.
   * \param radius le rayon d'une section verticale du tore (eq. rayon du pneu).
   * \return l'identifiant (référence) de l'objet-géométrie généré.
   * \see gl4dgDraw pour le dessiner.
   * \see gl4dgDraw pour le détruire.
   * \see gl4dgSetGeometryOptimizationLevel
   */
  GL4DAPI GLuint    GL4DAPIENTRY gl4dgGenTorusf(GLuint slices, GLuint stacks, GLfloat radius);
  /*!\brief Génère un objet-géométrie de type grille de sommets et renvoie son identifiant (référence).
   * 
   * La grille est couchée en y=0 et est orientée vers le haut
   *
   * \param width la largeur de la grille.
   * \param height la hauteur de la grille.
   * \return l'identifiant (référence) de l'objet-géométrie généré.
   * \see gl4dgDraw pour le dessiner.
   * \see gl4dgDraw pour le détruire.
   * \see gl4dgSetGeometryOptimizationLevel
   */
  GL4DAPI GLuint    GL4DAPIENTRY gl4dgGenGrid2df(GLuint width, GLuint height);
  /*!\brief Génère un objet-géométrie de type grille-heightmap et renvoie son identifiant (référence).
   * 
   * La grille est couchée en y=0 et est orientée vers le haut
   *
   * \param width la largeur de la grille.
   * \param height la hauteur de la grille.
   * \param heightmap matrice de mêmes dimensions que la grille et
   * servant de carte de hauteurs permettant de déplacer les sommets
   * de la grille (les normales sont calculées et lissées en fonction
   * de la heightmap). Les valeurs doivent être données entre 0 et 1
   * afin d'obtenir une grille dont les y varient entre -1 et +1.
   * \return l'identifiant (référence) de l'objet-géométrie généré.
   * \see gl4dgDraw pour le dessiner.
   * \see gl4dgDraw pour le détruire.
   * \see gl4dgSetGeometryOptimizationLevel
   */
  GL4DAPI GLuint    GL4DAPIENTRY gl4dgGenGrid2dFromHeightMapf(GLuint width, GLuint height, GLfloat * heightmap);
  /*!\brief Dessine un objet-géométrie dont l'identifiant (référence)
   * est passé en argument.
   * 
   * \param id identifiant de l'objet à dessiner.
   */
  GL4DAPI void      GL4DAPIENTRY gl4dgDraw(GLuint id);
  /*!\brief Détruit un objet-géométrie dont l'identifiant (référence)
   * est passé en argument.
   * 
   * \param id identifiant de l'objet à supprimer.
   */
  GL4DAPI void      GL4DAPIENTRY gl4dgDelete(GLuint id);
  
#ifdef __cplusplus
}
#endif

#endif
