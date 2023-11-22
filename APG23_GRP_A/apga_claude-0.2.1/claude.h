/*!\file claude.h
 * \brief bibliothèque de rastérisation développée par le groupe A du
 * cours de L2 "Algo pour la programmation graphique" (licence
 * informatique, Univ. Paris 8) en collaboration avec l'enseignant
 * (Farès Belhadj).
 * \date d'octobre à fin novembre 2023
 */

#ifndef CLAUDE_H
#define CLAUDE_H

#include <stdint.h>
#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4du.h>

#include "primitives.h"

/* nouvel arrivant */
#include "claude_math.h"

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

#define CL_EPSILON 0.00001f

#ifdef __cplusplus
extern "C" {
#endif

  static inline uint32_t rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return (((uint32_t)r) << R_SHIFT | ((uint32_t)g) << G_SHIFT |  ((uint32_t)b) << B_SHIFT | ((uint32_t)a) << A_SHIFT );
  }
  
  static inline uint32_t rgb(uint8_t r, uint8_t g, uint8_t b) {
    return rgba(r, g, b,255);
  }
  
  static inline uint8_t extract_comp(uint32_t coul, int shift) {
    return (uint8_t)((((uint32_t)(coul)) >> (shift)) & 0xFF);
  }

  static inline uint8_t red(uint32_t coul) {
    return extract_comp(coul, R_SHIFT);
  }
  
  static inline uint8_t green(uint32_t coul) {
    return extract_comp(coul, G_SHIFT);
  }
  
  static inline uint8_t blue(uint32_t coul) {
    return extract_comp(coul, B_SHIFT);
  }
  
  /* dans claude.c */
  extern int        claude_init(int argc, char ** argv, const char * title, int ww, int wh, int width, int height);
  extern void       claude_clear(void);
  extern uint32_t   get_width(void);
  extern uint32_t   get_height(void);
  extern void       clear_screen(void);
  extern uint32_t * get_pixels(void);
  extern float *    get_depths(void);
  extern void       update_screen(void);
  /* dans transformations.c */
  extern void claude_apply_transforms(const mat4 M, const mat4 V, const mat4 P, const surface_t * s, surface_t * sp);
  extern void claude_draw(surface_t * sp, const int * viewport);


#ifdef __cplusplus
}
#endif
  
#endif /* du #ifndef CLAUDE_H */
