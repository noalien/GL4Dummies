/*!\file primitives.c
 * \brief raster "maison" utilisant l'algo du tracé de droite Br'65.
 *
 * ATTENTION CE CODE EST LE RÉSULTAT OBTENU SUITE À CE QUI A ÉTÉ FAIT
 * EN COURS, IL A ÉTÉ LÉGÈREMENT CORRIGÉ ET COMPLÉTÉ POUR ÊTRE 
 * FONCTIONNEL MAIS IL N'Y A AUCUNE GARANTIE QUE TOUTES LES SITUATIONS 
 * AIENT ÉTÉ CORRIGÉES.
 *
 * \author Farès BELHADJ, amsi@up8.edu
 * \date November 24, 2020.
 */

#include "moteur.h"
#include <assert.h>

/* bloc de fonctions locales (static) */
static inline void    fillTriangle(surface_t * s, triangle_t * t);
static inline void    abscisses(surface_t * s, vertex_t * p0, vertex_t * p1, vertex_t * absc, int replace);
static inline void    drawHLine(surface_t * s, vertex_t * vG, vertex_t * vD);
static inline void    shading_none(surface_t * s, GLuint * pcolor, vertex_t * v);
static inline void    shading_only_tex(surface_t * s, GLuint * pcolor, vertex_t * v);
static inline void    shading_only_color_CM(surface_t * s, GLuint * pcolor, vertex_t * v);
static inline void    shading_only_color(surface_t * s, GLuint * pcolor, vertex_t * v);
static inline void    shading_all_CM(surface_t * s, GLuint * pcolor, vertex_t * v);
static inline void    shading_all(surface_t * s, GLuint * pcolor, vertex_t * v);
static inline void    interpolate(vertex_t * r, vertex_t * a, vertex_t * b, float fa, float fb, int s, int e);
static inline void    metainterpolate_none(vertex_t * r, vertex_t * a, vertex_t * b, float fa, float fb);
static inline void    metainterpolate_only_tex(vertex_t * r, vertex_t * a, vertex_t * b, float fa, float fb);
static inline void    metainterpolate_only_color(vertex_t * r, vertex_t * a, vertex_t * b, float fa, float fb);
static inline void    metainterpolate_all(vertex_t * r, vertex_t * a, vertex_t * b, float fa, float fb);
static inline GLuint  rgba(GLubyte r, GLubyte g, GLubyte b, GLubyte a);
static inline GLubyte red(GLuint c);
static inline GLubyte green(GLuint c);
static inline GLubyte blue(GLuint c);
static inline GLubyte alpha(GLuint c);
static        void    pquit(void); 

/*!\brief la texture courante à utiliser en cas de mapping de texture */
static GLuint * _tex = NULL;
/*!\brief la largeur de la texture courante à utiliser en cas de
 * mapping de texture */
static GLuint _texW = 0;
/*!\brief la hauteur de la texture courante à utiliser en cas de
 * mapping de texture */
static GLuint _texH = 0;
/*!\brief un buffer de depth pour faire le z-test */
static float * _depth = NULL;
/*!\brief flag pour savoir s'il faut ou non corriger l'interpolation
 * par rapport à la profondeur en cas de projection en
 * perspective */
static int _perpective_correction = 0;

/*!\brief transforme et rastérise l'ensemble des triangles de la
 * surface. */
void transform_n_raster(surface_t * s, float * mvMat, float * projMat) {
  int i;
  /* la première fois allouer le depth buffer */
  if(_depth == NULL) {
    _depth = calloc(gl4dpGetWidth() * gl4dpGetHeight(), sizeof *_depth);
    assert(_depth);
    atexit(pquit);
  }
  /* si projMat[15] est à 1, c'est une projection orthogonale, pas
   * besoin de correction de perspective */
  _perpective_correction = projMat[15] == 1.0f ? 0 : 1;
  /* le viewport est fixe ; \todo peut devenir paramétrable ... */
  float viewport[] = { 0.0f, 0.0f, (float)gl4dpGetWidth(), (float)gl4dpGetHeight() };
  stransform(s, mvMat, projMat, viewport);
  /* mettre en place la texture qui sera utilisée pour mapper la surface */
  if(s->options & SO_USE_TEXTURE)
    setTexture(s->texId);
  for(i = 0; i < s->n; ++i) {
    /* si le triangle est déclaré CULL (par exemple en backface), le rejeter */
    if(s->t[i].state & PS_CULL ) continue;
    /* on rejette aussi les triangles complètement out */
    if(s->t[i].state & PS_TOTALLY_OUT) continue;
    /* "hack" pas terrible permettant de rejeter les triangles
     * partiellement out dont au moins un sommet est TOO_FAR (trop
     * éloigné). Voir le fichier transformations.c pour voir comment
     * améliorer ce traitement. */
    if( s->t[i].state & PS_PARTIALLY_OUT &&
	( (s->t[i].v[0].state & PS_TOO_FAR) ||
	  (s->t[i].v[1].state & PS_TOO_FAR) ||
	  (s->t[i].v[2].state & PS_TOO_FAR)    ) )
      continue;
    fillTriangle(s, &(s->t[i]));
  }
}

/*!\brief effacer le buffer de profondeur (à chaque frame) pour
 * réaliser le z-test */
void clearDepth(void) {
  if(_depth) {
    memset(_depth, 0, gl4dpGetWidth() * gl4dpGetHeight() * sizeof *_depth);
  }
}

/*!\brief met en place une texture pour être mappée sur la surface en cours */
void setTexture(GLuint screen) {
  GLuint oldId = gl4dpGetTextureId(); /* au cas où */
  gl4dpSetScreen(screen);
  _tex = gl4dpGetPixels();
  _texW = gl4dpGetWidth();
  _texH = gl4dpGetHeight();
  gl4dpSetScreen(oldId);
}


/*!\brief met à jour la fonction d'interpolation et de coloriage
 * (shadingfunc) de la surface en fonction de ses options */
void updatesfuncs(surface_t * s) {
  int t;
  if(s->options & SO_USE_TEXTURE) {
    s->interpolatefunc = (t = s->options & SO_COLOR_MATERIAL) ? metainterpolate_all : metainterpolate_only_tex;
    s->shadingfunc = (s->options & SO_USE_COLOR) ? (t ? shading_all_CM : shading_all) : shading_only_tex;
  } else  {
    s->interpolatefunc = (t = s->options & SO_COLOR_MATERIAL) ? metainterpolate_only_color : metainterpolate_none;
    s->shadingfunc = (s->options & SO_USE_COLOR) ? (t ? shading_only_color_CM : shading_only_color) : shading_none;;
  }
}

/*!\brief fonction principale de ce fichier, elle dessine un triangle
 * rempli à l'écran en calculant l'ensemble des gradients
 * (interpolations bilinaires des attributs du sommet).
 */
inline void fillTriangle(surface_t * s, triangle_t * t) {
  vertex_t * aG = NULL, * aD = NULL;
  int bas, median, haut, n, signe, i, h = gl4dpGetHeight();
  if(t->v[0].y < t->v[1].y) {
    if(t->v[0].y < t->v[2].y) {
      bas = 0;
      if(t->v[1].y < t->v[2].y) {
	median = 1;
	haut = 2;
      } else {
	median = 2;
	haut = 1;
      }
    } else {
      bas = 2;
      median = 0;
      haut = 1;
    }
  } else { /* p0 au dessus de p1 */
    if(t->v[1].y < t->v[2].y) {
      bas = 1;
      if(t->v[0].y < t->v[2].y) {
	median = 0;
	haut = 2;
      } else {
	median = 2;
	haut = 0;
      }
    } else {
      bas = 2;
      median = 1;
      haut = 0;
    }
  }
  n = t->v[haut].y - t->v[bas].y + 1;
  aG = malloc(n * sizeof *aG);
  assert(aG);
  aD = malloc(n * sizeof *aD);
  assert(aD);
  /* est-ce que Pm est à gauche (+) ou à droite (-) de la droite (Pb->Ph) ? */
  /*MAJ, idée TODO?, un produit vectoriel pourrait s'avérer mieux */
  if(t->v[haut].x == t->v[bas].x /*MAJ*/ || t->v[haut].y == t->v[bas].y) {
    /* eq de la droite x = t->v[haut].x; MAJ ou y = t->v[haut].y; */
    signe = (t->v[median].x > t->v[haut].x) ? -1 : 1;
  } else {
    /* eq ax + y + c = 0 */
    float a, c, x;
    a = (t->v[haut].y - t->v[bas].y) / (float)(t->v[bas].x - t->v[haut].x);
    c = -a * t->v[haut].x - t->v[haut].y;
    /*MAJ on trouve le x sur la droite au même y que le median et on compare */
    x = -(c + t->v[median].y) / a;
    signe = (t->v[median].x >= x) ? -1 : 1;
  }
  if(signe < 0) { /* aG reçoit Ph->Pb, et aD reçoit Ph->Pm puis Pm vers Pb */
    abscisses(s, &(t->v[haut]), &(t->v[bas]), aG, 1);
    abscisses(s, &(t->v[haut]), &(t->v[median]), aD, 1);
    abscisses(s, &(t->v[median]), &(t->v[bas]), &aD[t->v[haut].y - t->v[median].y], 0);
  } else { /* aG reçoit Ph->Pm puis Pm vers Pb, et aD reçoit Ph->Pb */
    abscisses(s, &(t->v[haut]), &(t->v[bas]), aD, 1);
    abscisses(s, &(t->v[haut]), &(t->v[median]), aG, 1);
    abscisses(s, &(t->v[median]), &(t->v[bas]), &aG[t->v[haut].y - t->v[median].y], 0);
  }
  /* printf pouvant être utile en cas de DEBUG */
  /* printf("signe: %d, haut = %d (%d, %d), median = %d (%d, %d), bas = %d (%d, %d)\n", signe, */
  /* 	 haut, t->v[haut].x, t->v[haut].y, median, t->v[median].x, t->v[median].y, bas, t->v[bas].x, t->v[bas].y); */
  for(i = 0; i < n; ++i) {
    if( aG[i].y >= 0 && aG[i].y < h &&
	( (aG[i].z >= 0 && aG[i].z <= 1) || (aD[i].z >= 0 && aD[i].z <= 1) ) )
      drawHLine(s, &aG[i], &aD[i]);
  }
  free(aG);
  free(aD);
}

/*!\brief utilise Br'65 pour determiner les abscisses des segments du
 * triangle à remplir (par \a drawHLine).
 */
inline void abscisses(surface_t * s, vertex_t * p0, vertex_t * p1, vertex_t * absc, int replace) {
  int u = p1->x - p0->x, v = p1->y - p0->y, pasX = u < 0 ? -1 : 1, pasY = v < 0 ? -1 : 1;
  float dmax = sqrtf(u * u + v * v), p;
  u = abs(u); v = abs(v);
  if(u > v) { // 1er octan
    if(replace) {
      int objX = (u + 1) * pasX;
      int delta = u - 2 * v, incH = -2 * v, incO = 2 * u - 2 * v;
      for (int x = 0, y = 0, k = 0; x != objX; x += pasX) {
	absc[k].x = x + p0->x;
	absc[k].y = y + p0->y;
	p = sqrtf(x * x + y * y) / dmax;
	s->interpolatefunc(&absc[k], p0, p1, 1.0f - p, p);
	if(delta < 0) {
	  ++k;
	  y += pasY;
	  delta += incO;
	} else
	  delta += incH;
      }
    } else {
      int objX = (u + 1) * pasX;
      int delta = u - 2 * v, incH = -2 * v, incO = 2 * u - 2 * v;
      for (int x = 0, y = 0, k = 0, done = 0; x != objX; x += pasX) {
	if(!done) {
	  absc[k].x = x + p0->x;
	  absc[k].y = y + p0->y;
	  p = sqrtf(x * x + y * y) / dmax;
	  s->interpolatefunc(&absc[k], p0, p1, 1.0f - p, p);
	  done = 1;
	}
	if(delta < 0) {
	  ++k;
	  done = 0;
	  y += pasY;
	  delta += incO;
	} else
	  delta += incH;
      }
    }
  } else { // 2eme octan
    int objY = (v + 1) * pasY;
    int delta = v - 2 * u, incH = -2 * u, incO = 2 * v - 2 * u;
    for (int x = 0, y = 0, k = 0; y != objY; y += pasY) {
      absc[k].x = x + p0->x;
      absc[k].y = y + p0->y;
      p = sqrtf(x * x + y * y) / dmax;
      s->interpolatefunc(&absc[k], p0, p1, 1.0f - p, p);
      ++k;
      if(delta < 0) {
	x += pasX;
	delta += incO;
      } else
	delta += incH;
    }
  }
}

/*!\brief remplissage par droite horizontale entre deux abscisses */
inline void drawHLine(surface_t * s, vertex_t * vG, vertex_t * vD) {
  int w = gl4dpGetWidth(), x, yw = vG->y * w;
  GLuint * image = gl4dpGetPixels();
  float dmax = vD->x - vG->x, p, deltap;
  vertex_t v;
  /* il reste d'autres optims possibles */
  for(x = vG->x, p = 0.0f, deltap = 1.0f / dmax; x <= vD->x; ++x, p += deltap)
    if(x >= 0 && x < w) {
      s->interpolatefunc(&v, vG, vD, 1.0f - p, p);
      if(v.z < 0 || v.z > 1 || v.z < _depth[yw + x]) { continue; }
      s->shadingfunc(s, &image[yw + x], &v);
      _depth[yw + x] = v.z;
    }
}
/*!\brief aucune couleur n'est inscrite */
inline void shading_none(surface_t * s, GLuint * pcolor, vertex_t * v) {
  //vide pour l'instant, à prévoir le z-buffer
}

/*!\brief la couleur du pixel est tirée uniquement de la texture */
inline void shading_only_tex(surface_t * s, GLuint * pcolor, vertex_t * v) {
  int xt, yt, ct;
  GLubyte r, g, b, a;
  xt = (int)(v->texCoord.x * (_texW - EPSILON));
  if(xt < 0) {
    xt = xt % (-_texW);
    while(xt < 0) xt += _texW;
  } else
    xt = xt % _texW;
  yt = (int)(v->texCoord.y * (_texH - EPSILON));
  if(yt < 0) {
    yt = yt % (-_texH);
    while(yt < 0) yt += _texH;
  } else
    yt = yt % _texH;
  ct = yt * _texW + xt;
  *pcolor = _tex[yt * _texW + xt];
  r = (GLubyte)(  red(_tex[ct]) * v->li);
  g = (GLubyte)(green(_tex[ct]) * v->li);
  b = (GLubyte)( blue(_tex[ct]) * v->li);
  a = (GLubyte) alpha(_tex[ct]);
  *pcolor = rgba(r, g, b, a);
}

/*!\brief la couleur du pixel est tirée de la couleur interpolée */
inline void shading_only_color_CM(surface_t * s, GLuint * pcolor, vertex_t * v) {
  GLubyte r, g, b, a;
  r = (GLubyte)(v->li * v->icolor.x * (255 + EPSILON));
  g = (GLubyte)(v->li * v->icolor.y * (255 + EPSILON));
  b = (GLubyte)(v->li * v->icolor.z * (255 + EPSILON));
  a = (GLubyte)(v->icolor.w * (255 + EPSILON));
  *pcolor = rgba(r, g, b, a);
}

/*!\brief la couleur du pixel est tirée de la couleur diffuse de la
 * surface */
inline void shading_only_color(surface_t * s, GLuint * pcolor, vertex_t * v) {
  GLubyte r, g, b, a;
  r = (GLubyte)(v->li * s->dcolor.x * (255 + EPSILON));
  g = (GLubyte)(v->li * s->dcolor.y * (255 + EPSILON));
  b = (GLubyte)(v->li * s->dcolor.z * (255 + EPSILON));
  a = (GLubyte)(s->dcolor.w * (255 + EPSILON));
  *pcolor = rgba(r, g, b, a);
}

/*!\brief la couleur du pixel est le produit de la couleur interpolée
 * et de la texture */
inline void shading_all_CM(surface_t * s, GLuint * pcolor, vertex_t * v) {
  GLubyte r, g, b, a;
  int xt, yt, ct;
  xt = (int)(v->texCoord.x * (_texW - EPSILON));
  if(xt < 0) {
    xt = xt % (-_texW);
    while(xt < 0) xt += _texW;
  } else
    xt = xt % _texW;
  yt = (int)(v->texCoord.y * (_texH - EPSILON));
  if(yt < 0) {
    yt = yt % (-_texH);
    while(yt < 0) yt += _texH;
  } else
    yt = yt % _texH;
  ct = yt * _texW + xt;
  r = (GLubyte)((  red(_tex[ct]) + EPSILON) * v->li * v->icolor.x);
  g = (GLubyte)((green(_tex[ct]) + EPSILON) * v->li * v->icolor.y);
  b = (GLubyte)(( blue(_tex[ct]) + EPSILON) * v->li * v->icolor.z);
  a = (GLubyte)((alpha(_tex[ct]) + EPSILON) * v->icolor.w);
  *pcolor = rgba(r, g, b, a);
}

/*!\brief la couleur du pixel est le produit de la couleur diffuse
 * de la surface et de la texture */
inline void shading_all(surface_t * s, GLuint * pcolor, vertex_t * v) {
  GLubyte r, g, b, a;
  int xt, yt, ct;
  xt = (int)(v->texCoord.x * (_texW - EPSILON));
  if(xt < 0) {
    xt = xt % (-_texW);
    while(xt < 0) xt += _texW;
  } else
    xt = xt % _texW;
  yt = (int)(v->texCoord.y * (_texH - EPSILON));
  if(yt < 0) {
    yt = yt % (-_texH);
    while(yt < 0) yt += _texH;
  } else
    yt = yt % _texH;
  ct = yt * _texW + xt;
  r = (GLubyte)((  red(_tex[ct]) + EPSILON) * v->li * s->dcolor.x);
  g = (GLubyte)((green(_tex[ct]) + EPSILON) * v->li * s->dcolor.y);
  b = (GLubyte)(( blue(_tex[ct]) + EPSILON) * v->li * s->dcolor.z);
  a = (GLubyte)((alpha(_tex[ct]) + EPSILON) * s->dcolor.w);
  *pcolor = rgba(r, g, b, a);
}

/*!\brief interpolation de plusieurs floattants (entre \a s et \a e)
 * de la structure vertex_t en utilisant \a a et \a b, les
 * facteurs \a fa et \a fb, le tout dans \a r 
 * \todo un pointeur de fonction pour éviter un test s'il faut
 * un _perpective_correction != 0 ??? */
inline void interpolate(vertex_t * r, vertex_t * a, vertex_t * b, float fa, float fb, int s, int e) {
  int i;
  float * pr = (float *)&(r->texCoord);
  float * pa = (float *)&(a->texCoord);
  float * pb = (float *)&(b->texCoord);
  /* Correction de l'interpolation par rapport à la perspective, le z
   * joue un rôle dans les distances, il est nécessaire de le
   * réintégrer en modifiant les facteurs de proportion.
   * lien utile : https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/perspective-correct-interpolation-vertex-attributes 
   */
  if(_perpective_correction) {
    float z = 1.0f / (fa / a->zmod + fb / b->zmod);
    fa = z * fa / a->zmod; fb = z * fb / b->zmod;
  }
  for(i = s; i <= e; ++i)
    pr[i] = fa * pa[i] + fb * pb[i];
}

/*!\brief meta-fonction pour appeler \a interpolate, demande
 * uniquement l'interpolation des z */
inline void metainterpolate_none(vertex_t * r, vertex_t * a, vertex_t * b, float fa, float fb) {
  interpolate(r, a, b, fa, fb, 6, 8);
}

/*!\brief meta-fonction pour appeler \a interpolate, demande
 * uniquement l'interpolation des coord. de texture et les z */
inline void metainterpolate_only_tex(vertex_t * r, vertex_t * a, vertex_t * b, float fa, float fb) {
  interpolate(r, a, b, fa, fb, 0, 1);
  interpolate(r, a, b, fa, fb, 6, 8);
}

/*!\brief meta-fonction pour appeler \a interpolate, demande
 * uniquement l'interpolation des couleurs et les z */
inline void metainterpolate_only_color(vertex_t * r, vertex_t * a, vertex_t * b, float fa, float fb) {
  interpolate(r, a, b, fa, fb, 2, 8);
}

/*!\brief meta-fonction pour appeler \a interpolate, demande
 * l'interpolation de l'ensemble des attributs */
inline void metainterpolate_all(vertex_t * r, vertex_t * a, vertex_t * b, float fa, float fb) {
  interpolate(r, a, b, fa, fb, 0, 8);
}

GLuint rgba(GLubyte r, GLubyte g, GLubyte b, GLubyte a) {
  return RGBA(r, g, b, a);
}

GLubyte red(GLuint c) {
  return RED(c);
}

GLubyte green(GLuint c) {
  return GREEN(c);
}

GLubyte blue(GLuint c) {
  return BLUE(c);
}

GLubyte alpha(GLuint c) {
  return ALPHA(c);
}


/*!\brief tracé de droite dans grille écran selon l'algorithme Br'65.
 *
 * \deprecated ne gère pas le sommet en général (dans l'espace 3D).
 */
void drawLine(int x0, int y0, int x1, int y1, GLuint color) {
  int u = x1 - x0, v = y1 - y0, pasX = u < 0 ? -1 : 1, pasY = v < 0 ? -1 : 1;
  int w = gl4dpGetWidth();
  GLuint * image = gl4dpGetPixels();
  u = abs(u); v = abs(v);
  if(u > v) { // 1er octan
    int objX = (u + 1) * pasX;
    int delta = u - 2 * v, incH = -2 * v, incO = 2 * u - 2 * v;
    for (int x = 0, y = 0; x != objX; x += pasX) {
      if(IN_SCREEN(x + x0, y0 + y))
	image[(y0 + y) * w + x + x0] = color;
      if(delta < 0) {
	y += pasY;
	delta += incO;
      } else
	delta += incH;
    }
  } else { // 2eme octan
    int objY = (v + 1) * pasY;
    int delta = v - 2 * u, incH = -2 * u, incO = 2 * v - 2 * u;
    for (int x = 0, y = 0; y != objY; y += pasY) {
      if(IN_SCREEN(x + x0, y0 + y))
	image[(y0 + y) * w + x + x0] = color;
      if(delta < 0) {
	x += pasX;
	delta += incO;
      } else
	delta += incH;
    }
  }
}

/*!\brief au moment de quitter le programme désallouer la mémoire
 * utilisée pour _depth */
void pquit(void) {
  if(_depth) {
    free(_depth);
    _depth = NULL;
  }
}
