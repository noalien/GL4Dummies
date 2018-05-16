/*!\file mobile.h
 *
 * \brief Bibliothèque de gestion de mobiles
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr 
 * \date March 10 2017
 */
#ifndef MOBILE_H

#define MOBILE_H

#ifdef __cplusplus
extern "C" {
#endif

  extern void mobileInit(int n, GLfloat width, GLfloat height);
  extern void mobileSetFreeze(GLuint id, GLboolean freeze);
  extern void mobileGetCoords(GLuint id, GLfloat * coords);
  extern void mobileSetCoords(GLuint id, GLfloat * coords);
  extern void mobileMove(void);
  extern void mobileDraw(GLuint obj);

#ifdef __cplusplus
}
#endif
#endif
