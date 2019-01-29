/*!\file gl4dp.c
 *
 * \brief Primitives de dessin 2D sur texture GL.
 *
 * Nouvelle version, gère de multiple écrans.
 * A été étendue pour s'intégrer à la bibliothèque demoHelper.
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date March 14 2014
 * \todo faire des version inline ou macro des fonctions aussi
 * utilisées en interne pour plus de gain (exemple gl4dpPutPixel).
 * \todo améliorer gl4dpMap.
 */
#if defined(_MSC_VER)
#  define _USE_MATH_DEFINES
#endif
#include <math.h>
#include "gl4dp.h"
#include "gl4dg.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct screen_node_t screen_node_t;
struct screen_node_t {
  Uint32 * pixels;
  GLuint w, h, tId, isCPUToDate, isGPUToDate;
  struct screen_node_t * next;
};

static screen_node_t * _screen_list = NULL, ** _cur_screen = &_screen_list;
static void addScreen(GLuint w, GLuint h);
static void drawTex(GLuint tId, const GLfloat scale[2], const GLfloat translate[2]);
static void updateScreenFromGPU(void);

/*!\brief identifiant du programme GLSL */
static GLuint _pId = 0;

/*!\brief identifiant de la géométrie QUAD GL4Dummies */
static GLuint _quadId = 0;

/*!\brief La couleur en cours ; a modifier avec les fonctions setColor */
static Uint32 _cur_color = 0xFF0000FF;

/*!\brief revoie la \a _cur_color. */
Uint32 gl4dpGetColor(void) {
  return _cur_color;
}

/*!\brief met dans \a _cur_color la couleur passee en argument. */
void gl4dpSetColor(Uint32 color) {
  _cur_color = color;
}

/*!\brief initialise (ou réinitialise) l'écran aux dimensions \a w et
 * \a h, le lie à l'identifiant (OpenGL) de texture retourné.
 *
 * \param w la largeur de la fenêtre/texture GL.
 * \param h la hauteur de la fenêtre/texture GL.
 * \return l'identifiant (OpenGL) de la texture générée.
 */
GLuint gl4dpInitScreenWithDimensions(GLuint w, GLuint h) {
  const char * imvs =
    "<imvs>gl4dp_basic_with_transforms.vs</imvs>\
     #version 330\n					\
     layout (location = 0) in vec3 vsiPosition;\n	\
     layout (location = 1) in vec3 vsiNormal;\n		\
     layout (location = 2) in vec2 vsiTexCoord;\n	\
     uniform float rotation;\n				\
     uniform vec2 scale, translate, ptranslate;\n\
     out vec2 vsoTexCoord;\n			\
     void main(void) {\n						\
       mat2 r = mat2(cos(rotation), sin(rotation), -sin(rotation), cos(rotation));\n \
       vec2 v;\n							\
       v = r * (vsiPosition.xy - ptranslate) + ptranslate;\n		\
       gl_Position = vec4(v, 0.0, 1.0);\n				\
       vsoTexCoord = (vsiTexCoord / scale) - sign(scale) * translate;\n	\
     }\n";
  const char * imfs =
    "<imfs>gl4dp_basic.fs</imfs>\n\
     #version 330\n         \
     uniform sampler2D myTexture;\n     \
     in  vec2 vsoTexCoord;\n        \
     out vec4 fragColor;\n						\
     void main(void) {\n						\
       fragColor = texture(myTexture, vec2(vsoTexCoord.x, vsoTexCoord.y));\n \
     }";
  assert(w && h);
  addScreen(w, h);
  glBindTexture(GL_TEXTURE_2D, (*_cur_screen)->tId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); /* ICI */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  if(_pId == 0) {
    _quadId = gl4dgGenQuadf();
    _pId = gl4duCreateProgram(imvs, imfs, NULL);
  }
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  return (*_cur_screen)->tId;
}

/*!\brief initialise (ou réinitialise) l'écran aux dimensions du
 * viewport OpenGL, le lie à l'identifiant (OpenGL) de texture
 * retourné.
 *
 * Utilise \ref gl4dpInitScreenWithDimensions.
 *
 * \return l'identifiant (OpenGL) de la texture générée.
 * \see gl4dpInitScreenWithDimensions
 */
GLuint gl4dpInitScreen(void) {
  int vp[4];
  glGetIntegerv(GL_VIEWPORT, vp);
  return gl4dpInitScreenWithDimensions(vp[2], vp[3]);
}

/*!\brief ajoute un écran à la tête de liste d'écrans gérés.
 *
 * \param w la largeur de l'écran à créer.
 * \param h la hauteur de l'écran à créer.
 */
static void addScreen(GLuint w, GLuint h) {
  screen_node_t * newscr = NULL;
  assert(w && h);
  newscr = malloc(sizeof *newscr);
  assert(newscr);
  glGenTextures(1, &(newscr->tId));
  assert(newscr->tId);
  newscr->pixels = calloc(w * h, sizeof *(newscr->pixels));
  assert(newscr->pixels);
  newscr->w = w;
  newscr->h = h;
  newscr->isCPUToDate = 1;
  newscr->isGPUToDate = 0;
  newscr->next = _screen_list;
  _screen_list = newscr;
  _cur_screen = &_screen_list;
}

/*!\brief active comme écran courant l'écran dont l'id est passé en
 * argument.
 *
 * \param id identifiant de l'écran.
 */
int gl4dpSetScreen(GLuint id) {
  screen_node_t ** c = &_screen_list;
  while(*c && (*c)->tId != id)
    c = &((*c)->next);
  if(*c) {
    _cur_screen = c;
    return 1;
  }
  return 0;
}

/*!\brief supprime l'écran et se repositionne sur le premier */
void gl4dpDeleteScreen(void) {
  screen_node_t * to_delete;
  if(!*_cur_screen) return;
  if((*_cur_screen)->pixels != NULL)
    free((*_cur_screen)->pixels);
  if((*_cur_screen)->tId)
    glDeleteTextures(1, &((*_cur_screen)->tId));
  to_delete = *_cur_screen;
  *_cur_screen = (*_cur_screen)->next;
  free(to_delete);
  _cur_screen = &_screen_list;
}

/*!\brief retourne l'identifiant (OpenGL) de la texture liée à l'écran en cours.
 * \return l'identifiant (OpenGL) de la texture liée à cette bibliothèque.
 */
GLuint gl4dpGetTextureId(void) {
  return (*_cur_screen)->tId;
}

/*!\brief retourne la largeur de l'écran. */
GLuint gl4dpGetWidth(void) {
  return (*_cur_screen)->w;
}

/*!\brief retourne la hauteur de l'écran. */
GLuint gl4dpGetHeight(void) {
  return (*_cur_screen)->h;
}

/*!\brief Efface l'écran en mettant 0. */
void gl4dpClearScreen(void) {
  if(!(*_cur_screen)->isCPUToDate)
    updateScreenFromGPU();
  memset((*_cur_screen)->pixels, 0, (*_cur_screen)->w * (*_cur_screen)->h * sizeof *(*_cur_screen)->pixels);
  (*_cur_screen)->isGPUToDate = 0;
}

/*!\brief Efface l'écran en y mettant la valeur \a color. */
void gl4dpClearScreenWith(Uint32 color) {
  GLuint wh = (*_cur_screen)->w * (*_cur_screen)->h, i;
  if(!(*_cur_screen)->isCPUToDate)
    updateScreenFromGPU();
  for(i = 0; i < wh; i++)
    (*_cur_screen)->pixels[i] = color;
  (*_cur_screen)->isGPUToDate = 0;
}

/*!\brief renvoie la couleur à la coordonnée (x, y) */
Uint32 gl4dpGetPixel(int x, int y) {
  if(!(*_cur_screen)->isCPUToDate)
    updateScreenFromGPU();
  return (*_cur_screen)->pixels[y * (*_cur_screen)->w + x];
}

/*!\brief met la couleur en cours à la coordonnée (x, y) */
void gl4dpPutPixel(int x, int y) {
  if(!(*_cur_screen)->isCPUToDate)
    updateScreenFromGPU();
  (*_cur_screen)->pixels[y * (*_cur_screen)->w + x] = _cur_color;
  (*_cur_screen)->isGPUToDate = 0;
}

static void updateScreenFromGPU(void) {
  const GLfloat s[2] = {1.0, 1.0}, t[2] = {0.0, 0.0};
  glBindTexture(GL_TEXTURE_2D, (*_cur_screen)->tId);
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, (*_cur_screen)->pixels);
  (*_cur_screen)->isCPUToDate = (*_cur_screen)->isGPUToDate = 1;
  drawTex((*_cur_screen)->tId, s, t);
}

/*!\brief met à jour l'écran en envoyant la sous texture de dimensions
 * \a rect à GL. Si rect vaut NULL, met à jour toute la texture.
 *
 * \param rect le pointeur vers les quatre entiers positifs x,y (coin haut gauche du rectangle) et w,h (les dimensions du rectangle).
 */
void gl4dpUpdateScreen(GLint * rect) {
  const GLfloat s[2] = {1.0, 1.0}, t[2] = {0.0, 0.0};
  if(!(*_cur_screen)->isCPUToDate) {
  updateScreenFromGPU();
  return;
  }
  glBindTexture(GL_TEXTURE_2D, (*_cur_screen)->tId);
  if(!(*_cur_screen)->isGPUToDate) {
    if(rect == NULL)
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (*_cur_screen)->w, (*_cur_screen)->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, (*_cur_screen)->pixels);
    else {
      glPixelStorei(GL_UNPACK_ROW_LENGTH, (*_cur_screen)->w);
      glTexSubImage2D(GL_TEXTURE_2D, 0, rect[0], rect[1], rect[2], rect[3], GL_RGBA, GL_UNSIGNED_BYTE, &(*_cur_screen)->pixels[rect[0] + rect[1] * (*_cur_screen)->w]);
      glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    }
  }
  (*_cur_screen)->isCPUToDate = (*_cur_screen)->isGPUToDate = 1;
  drawTex((*_cur_screen)->tId, s, t);
}

/*!\brief dessine un rectangle
 * \a rect en utilisant la couleur en cours.
 *
 * \param rect le pointeur vers les quatre entiers positifs x,y (coin haut gauche du rectangle) et w,h (les dimensions du rectangle).
 */
void gl4dpRect(GLint * rect) {
  GLint x, y, yw, mx = rect[0] + rect[2], my = rect[1] + rect[3];
  if(!(*_cur_screen)->isCPUToDate)
    updateScreenFromGPU();
  for(y = rect[1]; y < my; y++) {
    yw = y * (*_cur_screen)->w;
    for(x = rect[0]; x < mx; x++)
      (*_cur_screen)->pixels[x + yw] = _cur_color;
  }
  (*_cur_screen)->isGPUToDate = 0;
}

/*!\brief dessine un segment (x0, y0) -> (x1, y1) de couleur \a
 * _cur_color en utilisant l'algorithme de Bresenham'65.
 */
void gl4dpLine(int x0, int y0, int x1, int y1) {
  int u = x1 - x0, v = y1 - y0;
  int pasX = (u < 0) ? -1 : 1, pasY = (v < 0) ? -1 : 1;
  int x, y, del, incH, incO;
  if(!(*_cur_screen)->isCPUToDate)
    updateScreenFromGPU();
  if(abs(u) < abs(v)) { /* deuxieme octant */
    del = (incH = ((pasX * u) << 1)) - pasY * v;
    incO = incH - ((pasY * v) << 1);
    for(y = y0, x = x0; y != y1; y += pasY) {
      if(IN_SCREEN(x, y))
  gl4dpPutPixel(x, y);
      if(del < 0) del += incH;
      else {
  del += incO;
  x += pasX;
      }
    }
  } else {  /* premier octant */
    del = (incH = ((pasY * v) << 1)) - pasX * u;
    incO = incH - ((pasX * u) << 1);
    for(x = x0, y = y0; x != x1; x += pasX) {
      if(IN_SCREEN(x, y))
  gl4dpPutPixel(x, y);
      if(del < 0) del += incH;
      else {
  del += incO;
  y += pasY;
      }
    }
  }
  (*_cur_screen)->isGPUToDate = 0;
}

/*!\brief dessine un cercle centré en (x0, y0) de rayon r en
 * utilisant l'algorithme de Bresenham'77.
 */
void gl4dpCircle(int x0, int y0, int r) {
  int x, y, del, incH, incO, t = M_SQRT1_2 * r + 1;
  if(!(*_cur_screen)->isCPUToDate)
    updateScreenFromGPU();
  del = 3 - (r << 1);
  incH = 6;
  incO = 10 - (r << 2);
  for(x = 0, y = r; x <= t; x++, incH += 4, incO += 4) {
    if(IN_SCREEN(x0 + x, y0 + y)) gl4dpPutPixel(x0 + x, y0 + y);
    if(IN_SCREEN(x0 + x, y0 - y)) gl4dpPutPixel(x0 + x, y0 - y);
    if(IN_SCREEN(x0 - x, y0 + y)) gl4dpPutPixel(x0 - x, y0 + y);
    if(IN_SCREEN(x0 - x, y0 - y)) gl4dpPutPixel(x0 - x, y0 - y);
    if(IN_SCREEN(x0 + y, y0 + x)) gl4dpPutPixel(x0 + y, y0 + x);
    if(IN_SCREEN(x0 + y, y0 - x)) gl4dpPutPixel(x0 + y, y0 - x);
    if(IN_SCREEN(x0 - y, y0 + x)) gl4dpPutPixel(x0 - y, y0 + x);
    if(IN_SCREEN(x0 - y, y0 - x)) gl4dpPutPixel(x0 - y, y0 - x);
    if(del < 0) del += incH;
    else {
      y--;
      incO += 4;
      del += incO;
    }
  }
  (*_cur_screen)->isGPUToDate = 0;
}

/*!\brief dessine un segment horizontal (x0, y) -> (x1, y).
 */
void gl4dpHLine(int x0, int x1, int y) {
  int x, pasX = (x1 - x0) < 0 ? -1 : 1, x1pp;
  if(!(*_cur_screen)->isCPUToDate)
    updateScreenFromGPU();
  y  = MIN(MAX(0, y), ((int)gl4dpGetHeight()) - 1);
  x0 = MIN(MAX(0, x0), ((int)gl4dpGetWidth()) - 1);
  x1 = MIN(MAX(0, x1), ((int)gl4dpGetWidth()) - 1);
  x1pp = x1 + pasX;
  for(x = x0; x != x1pp; x += pasX)
    gl4dpPutPixel(x, y);
  (*_cur_screen)->isGPUToDate = 0;
}

/*!\brief dessine un cercle plein centré en (x0, y0) de rayon r
 * en utilisant l'algorithme de Bresenham'77.
 *
 * \see hLine
 */
void gl4dpFilledCircle(int x0, int y0, int r) {
  int x, y, del, incH, incO, t = M_SQRT1_2 * r + 1;
  if(!(*_cur_screen)->isCPUToDate)
    updateScreenFromGPU();
  del = 3 - (r << 1);
  incH = 6;
  incO = 10 - (r << 2);
  for(x = 0, y = r; x <= t; x++, incH += 4, incO += 4) {
    gl4dpHLine(x0 + x, x0 - x, y0 + y);
    gl4dpHLine(x0 + x, x0 - x, y0 - y);
    gl4dpHLine(x0 + y, x0 - y, y0 + x);
    gl4dpHLine(x0 + y, x0 - y, y0 - x);
    if(del < 0) del += incH;
    else {
      y--;
      incO += 4;
      del += incO;
    }
  }
  (*_cur_screen)->isGPUToDate = 0;
}

/*!\brief convertie une surface SDL en un tableau de luminances
 * comprises entre 0 et 1 (L = 0.299 * R + 0.587 * G + 0.114 * B). Le
 * repère des y est remis vers le haut pour un usage GL.
 *
 * \param s La surface SDL à convertir
 * \return le pointeur vers le tableau de luminances (à libérer avec free).
 */
GLfloat * gl4dpSDLSurfaceToLuminanceMap(SDL_Surface * s) {
  int x, y, yw, wh = s->w * s->h;
  Uint32 * p;
  Uint8 r, g, b;
  SDL_Surface * d = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ABGR8888, 0);
  GLfloat * wm = malloc(wh * sizeof *wm);
  assert(wm);
  for(y = d->h -1, p = d->pixels; y >= 0; y--) {
    yw = y * d->w;
    for(x = 0; x < d->w; x++) {
      SDL_GetRGB(*p++, d->format, &r, &g, &b);
      wm[yw + x] = (0.299f * r) / 255.0f + (0.587f * g) / 255.0f + (0.114f * b) / 255.0f;
    }
  }
  SDL_FreeSurface(d);
  return wm;
}

/*!\brief copie la surface SDL vers l'écran en cours ; peut
 * zoomer/dézoomer/inverser et translater en utilisant \a scale et \a
 * translate.
 */
void gl4dpCopyFromSDLSurfaceWithTransforms(SDL_Surface * s, const GLfloat scale[2], const GLfloat translate[2]) {
  GLint ofbo, fboTId;
  GLuint fbo, id = 0;
  const GLfloat s0[2] = {1.0, 1.0}, t0[2] = {0.0, 0.0};
  SDL_Surface * d = SDL_CreateRGBSurface(0, s->w, s->h, 32, R_MASK, G_MASK, B_MASK, A_MASK);
  SDL_BlitSurface(s, NULL, d, NULL);
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, d->w, d->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, d->pixels);
  SDL_FreeSurface(d);
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &ofbo);
  if(ofbo) {
    /* RECUPERER L'ID DE LA DERNIERE TEXTURE ATTACHEE AU FRAMEBUFFER */
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &fboTId);
  } else {
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  }
  /* ATTACHER LA TEXTURE ECRAN POUR RENDRE DESSUS */
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, (*_cur_screen)->tId,  0);
  drawTex(id, scale, translate);
  if(ofbo) {
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,  fboTId,  0);
  } else {
    glDeleteFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
  glDeleteTextures(1, &id);
  (*_cur_screen)->isCPUToDate = 0;

  drawTex((*_cur_screen)->tId, s0, t0);
}

/*!\brief copie la surface SDL vers l'écran en cours en prenant 100% de l'écran.
 */
void gl4dpCopyFromSDLSurface(SDL_Surface * s) {
  const GLfloat s1[2] = {1.0, 1.0}, t1[2] = {0.0, 0.0};
  gl4dpCopyFromSDLSurfaceWithTransforms(s, s1, t1);
}

/*!\brief plaque un écran sur un autre écran.
 *
 * \param dstSId identifiant de l'écran destination (sur lequel on plaque).
 * \param srcSId identifiant de l'écran source (qui sera plaqué).
 * \param pRect rectangle représentant la position (dans dstSId) à laquelle nous allons plaquer la source srcSId.
 * \param tRect reclangle représentant la quantité de surface copiée depuis srcSId - c'est les coordonnées de textures.
 * \param rotation angle de rotation en radians appliqué lors du placage. Le rectangle pRect tourne autour de son centre.
 */
void gl4dpMap(GLuint dstSId, GLuint srcSId, const GLfloat pRect[4], const GLfloat tRect[4], GLfloat rotation) {
  GLint ofbo, fboTId;
  GLuint fbo, vao, buffer;
  GLfloat s[2] = {1.0, 1.0}, t[2] = {0.0, 0.0}, pt[2];
  GLfloat data[] = {
    /* 4 coordonnées de sommets */
    2.0f * pRect[0] - 1.0f, 2.0f * pRect[1] - 1.0f, 0.0f,
    2.0f * pRect[2] - 1.0f, 2.0f * pRect[1] - 1.0f, 0.0f,
    2.0f * pRect[0] - 1.0f, 2.0f * pRect[3] - 1.0f, 0.0f,
    2.0f * pRect[2] - 1.0f, 2.0f * pRect[3] - 1.0f, 0.0f,
    /* 1 normales par sommet */
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    /* 2 coordonnées de texture par sommet */
    tRect[0], tRect[1], tRect[2], tRect[1],
    tRect[0], tRect[3], tRect[2], tRect[3]
  };
  pt[0] = (pRect[0] + pRect[2]) - 1.0f;
  pt[1] = (pRect[1] + pRect[3]) - 1.0f;
  gl4dpSetScreen(srcSId);
  gl4dpUpdateScreen(NULL);
  gl4dpSetScreen(dstSId);
  gl4dpUpdateScreen(NULL);
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glGenBuffers(1, &buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof data, data, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (const void *)((4 * 3) * sizeof *data));
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (const void *)((8 * 3) * sizeof *data));

  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &ofbo);
  if(ofbo) {
    /* RECUPERER L'ID DE LA DERNIERE TEXTURE ATTACHEE AU FRAMEBUFFER */
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &fboTId);
  } else {
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  }
  /* ATTACHER LA TEXTURE ECRAN POUR RENDRE DESSUS */
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, (*_cur_screen)->tId,  0);

  glDisable(GL_DEPTH);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glUseProgram(_pId);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, srcSId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glUniform2fv(glGetUniformLocation(_pId, "scale"), 1, s);
  glUniform2fv(glGetUniformLocation(_pId, "translate"), 1, t);
  glUniform2fv(glGetUniformLocation(_pId, "ptranslate"), 1, pt);
  glUniform1f(glGetUniformLocation(_pId, "rotation"), rotation);
  glUniform1i(glGetUniformLocation(_pId, "myTexture"), 0);
  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glDisable(GL_BLEND);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  if(ofbo) {
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,  fboTId,  0);
  } else {
    glDeleteFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
  if(vao)
    glDeleteVertexArrays(1, &vao);
  if(buffer)
    glDeleteBuffers(1, &buffer);

  (*_cur_screen)->isCPUToDate = 0;
}

static void drawTex(GLuint tId, const GLfloat scale[2], const GLfloat translate[2]) {
  const GLfloat t[2] = {0.0f, 0.0f};
  glDisable(GL_DEPTH);
  glUseProgram(_pId);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tId);
  glUniform2fv(glGetUniformLocation(_pId, "scale"), 1, scale);
  glUniform2fv(glGetUniformLocation(_pId, "translate"), 1, translate);
  glUniform1i (glGetUniformLocation(_pId, "myTexture"), 0);
  glUniform2fv(glGetUniformLocation(_pId, "ptranslate"), 1, t);
  glUniform1f (glGetUniformLocation(_pId, "rotation"), 0);
  gl4dgDraw(_quadId);
}
