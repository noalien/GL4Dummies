/*!\file primitives.c
 * \brief raster "maison" utilisant l'algo de tracé de droite Br'65.
 *
 * CE CODE A ÉTÉ EN GRANDE PARTIE RÉALISÉ LORS DE LA SÉANCE DE
 * COURS. IL RESTE DES CHOSES À IMPLÉMENTER (CORRIGER) OU À
 * OPTIMISER.
 *
 * ATTENTION CE CODE EST LE RÉSULTAT OBTENU SUITE À CE QUI A ÉTÉ FAIT
 * EN COURS, IL A ÉTÉ LÉGÈREMENT CORRIGÉ POUR ÊTRE FONCTIONNEL MAIS IL
 * N'Y A AUCUNE GARANTIE QUE TOUTES LES SITUATIONS AIENT ÉTÉ
 * CORRIGÉES.
 *
 * \author Farès BELHADJ, amsi@up8.edu
 * \date November 24, 2020.
 */

#include "moteur.h"
#include <assert.h>

/* bloc de fonctions locales (static) */
static inline void    abscisses(vertex_t * p0, vertex_t * p1, vertex_t * absc, int replace);
static inline void    drawHLine(vertex_t * vG, vertex_t * vD);
static inline void    interpolate(vertex_t * r, vertex_t * a, vertex_t * b, float fa, float fb, int s, int e);
static inline GLuint  rgba(GLubyte r, GLubyte g, GLubyte b, GLubyte a);
static inline GLubyte red(GLuint c);
static inline GLubyte green(GLuint c);
static inline GLubyte blue(GLuint c);
static inline GLubyte alpha(GLuint c);

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

/*!\brief fonction principale de ce fichier, elle dessine un triangle
 * rempli à l'écran en calculant l'ensemble des gradients
 * (interpolations bilinaires des attributs du sommet).
 */
void fillTriangle(triangle_t * t) {
  vertex_t * aG = NULL, * aD = NULL;
  int bas, median, haut, n, signe, i, h = gl4dpGetHeight();
  /* on rejette les triangles complètement out */
  if(t->state & PS_TOTALLY_OUT) return;
  /* "hack" pas terrible permettant de rejeter les triangles
   * partiellement out dont au moins un sommet est TOO_FAR (trop
   * éloigné). Voir le fichier transformations.c pour voir comment
   * améliorer ce traitement. */
  if(t->state & PS_PARTIALLY_OUT) {
    for(i = 0; i < 3; ++i)
      if(t->v[i].state & PS_TOO_FAR) return;
  }
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
    abscisses(&(t->v[haut]), &(t->v[bas]), aG, 1);
    abscisses(&(t->v[haut]), &(t->v[median]), aD, 1);
    abscisses(&(t->v[median]), &(t->v[bas]), &aD[t->v[haut].y - t->v[median].y], 0);
  } else { /* aG reçoit Ph->Pm puis Pm vers Pb, et aD reçoit Ph->Pb */
    abscisses(&(t->v[haut]), &(t->v[bas]), aD, 1);
    abscisses(&(t->v[haut]), &(t->v[median]), aG, 1);
    abscisses(&(t->v[median]), &(t->v[bas]), &aG[t->v[haut].y - t->v[median].y], 0);
  }
  /* printf pouvant être utile en cas de DEBUG */
  /* printf("signe: %d, haut = %d (%d, %d), median = %d (%d, %d), bas = %d (%d, %d)\n", signe, */
  /* 	 haut, t->v[haut].x, t->v[haut].y, median, t->v[median].x, t->v[median].y, bas, t->v[bas].x, t->v[bas].y); */
  for(i = 0; i < n; ++i) {
    if( aG[i].y >= 0 && aG[i].y < h &&
	( (aG[i].z >= 0 && aG[i].z <= 1) || (aD[i].z >= 0 && aD[i].z <= 1) ) )
      drawHLine(&aG[i], &aD[i]);
  }
  free(aG);
  free(aD);
}

/*!\brief utilise Br'65 pour determiner les abscisses des segments du
 * triangle à remplir (par \a drawHLine).
 */
void abscisses(vertex_t * p0, vertex_t * p1, vertex_t * absc, int replace) {
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
	interpolate(&absc[k], p0, p1, 1.0f - p, p, 0, 7);
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
	  interpolate(&absc[k], p0, p1, 1.0f - p, p, 0, 7);
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
      interpolate(&absc[k], p0, p1, 1.0f - p, p, 0, 7);
      ++k;
      if(delta < 0) {
	x += pasX;
	delta += incO;
      } else
	delta += incH;
    }
  }
}

/*!\brief la texture courante à utiliser en cas de mapping de texture */
static GLuint * _tex = NULL;
/*!\brief la largeur de la texture courante à utiliser en cas de
 * mapping de texture */
static GLuint _texW = 0;
/*!\brief la hauteur de la texture courante à utiliser en cas de
 * mapping de texture */
static GLuint _texH = 0;

/*!\brief met en place une texture pour être mappée sur la surface en cours */
void setTexture(GLuint screen) {
  GLuint oldId = gl4dpGetTextureId(); /* au cas où */
  gl4dpSetScreen(screen);
  _tex = gl4dpGetPixels();
  _texW = gl4dpGetWidth();
  _texH = gl4dpGetHeight();
  gl4dpSetScreen(oldId);
}

/*!\brief remplissage par droite horizontale entre deux abscisses */
void drawHLine(vertex_t * vG, vertex_t * vD) {
  int w = gl4dpGetWidth(), x, yw = vG->y * w, xt, yt, ct;
  GLuint * image = gl4dpGetPixels();
  float dmax = vD->x - vG->x, p, deltap;
  vertex_t v;
  GLubyte r, g, b, a;
  /* il reste d'autres optims possibles */
  for(x = vG->x, p = 0.0f, deltap = 1.0f / dmax; x <= vD->x; ++x, p += deltap)
    if(x >= 0 && x < w) {
      interpolate(&v, vG, vD, 1.0f - p, p, 0, 7);
      if(v.z < 0 || v.z > 1) { continue; }
      xt = (int)(v.texCoord.x * (_texW - EPSILON));
      if(xt < 0) {
      	xt = xt % (-_texW);
      	while(xt < 0) xt += _texW;
      } else
      	xt = xt % _texW;
      yt = (int)(v.texCoord.y * (_texH - EPSILON));
      if(yt < 0) {
      	yt = yt % (-_texH);
      	while(yt < 0) yt += _texH;
      } else
      	yt = yt % _texH;
      ct = yt * _texW + xt;
      /* utiliser simplement la couleur */
      /* r = (GLubyte)(v.icolor.x * (255 + EPSILON)); */
      /* g = (GLubyte)(v.icolor.y * (255 + EPSILON)); */
      /* b = (GLubyte)(v.icolor.z * (255 + EPSILON)); */
      /* a = (GLubyte)(v.icolor.w * (255 + EPSILON)); */
      /* multiplier la texture par la couleur */
      r = (GLubyte)((  red(_tex[ct]) + EPSILON) * v.icolor.x);
      g = (GLubyte)((green(_tex[ct]) + EPSILON) * v.icolor.y);
      b = (GLubyte)(( blue(_tex[ct]) + EPSILON) * v.icolor.z);
      a = (GLubyte)((alpha(_tex[ct]) + EPSILON) * v.icolor.w);
      image[yw + x] = rgba(r, g, b, a);
    }
}

/*!\brief interpolation de plusieurs floattants (entre \a s et \a e)
 * de la structure vertex_t en utilisant \a a et \a b, les
 * facteurs \a fa et \a fb, le tout dans \a r 
 * \todo désactiver la correction en cas de projection orthogonale
 * (mat[15] == 1 au lieu de mat[15] == 0 pour la
 * perspective). Solution efficace, utiliser un pointeur de
 * fonction. */
inline void interpolate(vertex_t * r, vertex_t * a, vertex_t * b, float fa, float fb, int s, int e) {
  int i;
  float * pr = (float *)&(r->texCoord);
  float * pa = (float *)&(a->texCoord);
  float * pb = (float *)&(b->texCoord);
  /* Correction de l'interpolation par rapport à la perspective, le z
   * joue un rôle dans les distances, il est nécessaire de le
   * réintégrer en modifiant les facteurs de proportion.
   * lien utile : https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/perspective-correct-interpolation-vertex-attributes 
   *
   * TODO : désactiver la correction en cas de projection orthogonale
      //(mat[15] == 1 au lieu de mat[15] == 0 pour la
      //perspective). Solution efficace, utiliser un pointeur de
      //fonction.
   */
  float z = 1.0f / (fa / a->zmod + fb / b->zmod);
  fa = z * fa / a->zmod; fb = z * fb / b->zmod;
  for(i = s; i <= e; ++i)
    pr[i] = fa * pa[i] + fb * pb[i];
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
