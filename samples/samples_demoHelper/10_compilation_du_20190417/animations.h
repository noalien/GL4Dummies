/*!\file animations.h
 *
 * \brief Votre espace de liberté : c'est ici que vous pouvez ajouter
 * vos fonctions de transition et d'animation avant de les faire
 * référencées dans le tableau _animations du fichier \ref window.c
 *
 * Des squelettes d'animations et de transitions sont fournis pour
 * comprendre le fonctionnement de la bibliothèque. En bonus des
 * exemples dont un fondu en GLSL.
 *
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date May 05, 2014
 */
#ifndef _ANIMATIONS_H

#define _ANIMATIONS_H

#ifdef __cplusplus
extern "C" {
#endif

  extern void transition_vide(void (* a0)(int), void (* a1)(int), Uint32 t, Uint32 et, int state);
  extern void fondu(void (* a0)(int), void (* a1)(int), Uint32 t, Uint32 et, int state);
  extern void fondui(void (* a0)(int), void (* a1)(int), Uint32 t, Uint32 et, int state);
  extern void bidon00(int state);
  extern void bidon01(int state);
  extern void bidon02(int state);
  extern void animationsInit(void);

  extern void music_simple(int state);
  extern void music_fft(int state);
  extern void mix0105(int state);
  extern void earth(int state);

  /* dans transformations.c */
  extern void transformations(int state);

#ifdef __cplusplus
}
#endif

#endif
