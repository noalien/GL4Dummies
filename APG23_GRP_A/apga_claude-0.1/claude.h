#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4du.h>

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

static inline GLuint rgba(GLubyte r, GLubyte g, GLubyte b, GLubyte a) {
  return (((GLuint)r) << R_SHIFT | ((GLuint)g) << G_SHIFT |  ((GLuint)b) << B_SHIFT | ((GLuint)a) << A_SHIFT );
}

static inline GLuint rgb(GLubyte r, GLubyte g, GLubyte b) {
  return rgba(r, g, b,255);
}

extern int claude_init(int argc, char ** argv, const char * title, int ww, int wh, int width, int height);
extern GLuint get_width(void);
extern GLuint get_height(void);
extern void clear_screen(void);
extern GLuint * get_pixels(void);
extern void update_screen(void);
