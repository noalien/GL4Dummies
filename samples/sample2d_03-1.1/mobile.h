/*!\file mobile.h
 *
 * \brief Bibliothèque de gestion de mobiles
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr 
 * \date Februry 23 2016
 */
#ifndef MOBILE_H

#define MOBILE_H

#ifdef __cplusplus
extern "C" {
#endif
  
  extern void mobile2texture(float * f);
  extern void mobileInit(int n, int w, int h);
  extern void mobileMove(void);
  
#ifdef __cplusplus
}
#endif
#endif
