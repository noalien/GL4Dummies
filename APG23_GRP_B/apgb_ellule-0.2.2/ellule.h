#ifndef ELLULE_H
#define ELLULE_H

/* TODO : utiliser les type standards à taille fixe */

#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4du.h>
#include <stdint.h>

#include "elluleMaths.h"

#if SDL_BYTEORDER == SDL_BIGENDIAN 
/*!\brief décalage pour la composante rouge */
#  define R_SHIFT 24
/*!\brief décalage pour la composante verte */
#  define G_SHIFT 16
/*!\brief décalage pour la composante bleue */
#  define B_SHIFT  8
/*!\brief décalage pour la composante alpha */
#  define A_SHIFT  0
/*!\brief masque pour la composante rouge */
#  define R_MASK 0xff000000
/*!\brief masque pour la composante verte */
#  define G_MASK 0x00ff0000
/*!\brief masque pour la composante bleue */
#  define B_MASK 0x0000ff00
/*!\brief masque pour la composante alpha */
#  define A_MASK 0x000000ff
#else
/*!\brief décalage pour la composante rouge */
#  define R_SHIFT  0
/*!\brief décalage pour la composante verte */
#  define G_SHIFT  8
/*!\brief décalage pour la composante bleue */
#  define B_SHIFT 16
/*!\brief décalage pour la composante alpha */
#  define A_SHIFT 24
/*!\brief masque pour la composante rouge */
#  define R_MASK 0x000000ff
/*!\brief masque pour la composante verte */
#  define G_MASK 0x0000ff00
/*!\brief masque pour la composante bleue */
#  define B_MASK 0x00ff0000
/*!\brief masque pour la composante alpha */
#  define A_MASK 0xff000000
#endif

#define EL_EPSILON 0.00001f

#ifdef __cplusplus
extern "C" {
#endif
  
  static inline uint32_t _rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return (((uint32_t)r) << R_SHIFT | ((uint32_t)g) << G_SHIFT |  ((uint32_t)b) << B_SHIFT | ((uint32_t)a) << A_SHIFT );
  }
  
  static inline uint32_t _rgb(uint8_t r, uint8_t g, uint8_t b) {
    return _rgba(r, g, b,255);
  }
  
  static inline uint8_t _extract_comp(uint32_t coul, int shift) {
    return (uint8_t)((((uint32_t)(coul)) >> (shift)) & 0xFF);
  }

  static inline uint8_t _red(uint32_t coul) {
    return _extract_comp(coul, R_SHIFT);
  }

  static inline uint8_t _green(uint32_t coul) {
    return _extract_comp(coul, G_SHIFT);
  }

  static inline uint8_t _blue(uint32_t coul) {
    return _extract_comp(coul, B_SHIFT);
  }

  static inline uint8_t _alpha(uint32_t coul) {
    return _extract_comp(coul, A_SHIFT);
  }

  extern int        elInit(int argc, char ** argv, const char * title, int ww /* window width */, int wh /* window height */,
			   int width, int height);
  extern uint32_t   elGetWidth(void);
  extern uint32_t   elGetHeight(void);
  extern void       elClear(void);
  extern float    * elGetDepths(void);
  extern uint32_t * elGetPixels(void);
  extern void       elUpdate(void);

#ifdef __cplusplus
}
#endif
  
#endif
