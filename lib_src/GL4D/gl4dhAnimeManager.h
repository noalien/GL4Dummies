/*!\file gl4dhAnimeManager.h
 *
 * \brief Bibliothèque de gestion des écrans d'animation et de mises à
 * jour en fonction du son.
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date April 29 2014
 */
#ifndef _GL4DH_ANIME_MANAGER_H

#define _GL4DH_ANIME_MANAGER_H

#include "gl4du.h"

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct GL4DHanime GL4DHanime;
  
  enum GL4DHstate {
    GL4DH_INIT,
    GL4DH_UPDATE_WITH_AUDIO,
    GL4DH_DRAW,
    GL4DH_FREE
  };
  
  struct GL4DHanime {
    Uint32 time;
    void (* first)      (int state);
    void (* last)       (int state);
    void (* transition) (void (* anim0)(int), void (* anim1)(int), Uint32 fullTime, Uint32 elapsedTime, int state);
  };
  
  GL4DAPI void   GL4DAPIENTRY gl4dhInit(GL4DHanime * animations, int w, int h, void (*callBeforeAllAnimationsInit)(void));
  GL4DAPI void   GL4DAPIENTRY gl4dhClean(void);
  GL4DAPI void   GL4DAPIENTRY gl4dhStartingSignal(void);
  GL4DAPI Uint32 GL4DAPIENTRY gl4dhGetTicks(void);
  GL4DAPI void   GL4DAPIENTRY gl4dhDraw(void);
  GL4DAPI void   GL4DAPIENTRY gl4dhUpdateWithAudio(void);

#ifdef __cplusplus
}
#endif

#endif
