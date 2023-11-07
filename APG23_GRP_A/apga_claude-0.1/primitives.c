#include "primitives.h"
#include "claude.h"
#include <stdlib.h>
#include <assert.h>


static inline void _hline(vertex_t * p0, vertex_t * p1);
static inline void _abscisses(vertex_t * p0, vertex_t * p1, vertex_t * absc, int replace);

void fill_triangle(triangle_t * t) {
  int bas, median, haut, i;
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
  int signe, n = t->v[haut].y - t->v[bas].y + 1;
  vertex_t * aG = malloc(n * sizeof *aG);
  assert(aG);
  vertex_t * aD = malloc(n * sizeof *aD);
  assert(aD);

  /* est-ce que Pm est à gauche (+) ou à droite (-) de la droite (Pb->Ph) ? */
  /* idée TODO?, un produit vectoriel pourrait s'avérer mieux */
  if(t->v[haut].x == t->v[bas].x || t->v[haut].y == t->v[bas].y) {
    /* eq de la droite x = t->v[haut].x; ou y = t->v[haut].y; */
    signe = (t->v[median].x > t->v[haut].x) ? -1 : 1;
  } else {
    /* eq ax + y + c = 0 */
    float a, c, x;
    a = (t->v[haut].y - t->v[bas].y) / (float)(t->v[bas].x - t->v[haut].x);
    c = -a * t->v[haut].x - t->v[haut].y;
    /* on cherche le x sur la DROITE au même y que le median et on compare */
    x = -(c + t->v[median].y) / a;
    signe = (t->v[median].x >= x) ? -1 : 1;
  }
  if(signe < 0) { /* aG reçoit Ph->Pb, et aD reçoit Ph->Pm puis Pm vers Pb */
    _abscisses(&(t->v[haut]), &(t->v[bas]), aG, 1);
    _abscisses(&(t->v[haut]), &(t->v[median]), aD, 1);
    _abscisses(&(t->v[median]), &(t->v[bas]), &aD[t->v[haut].y - t->v[median].y], 0);
  } else { /* aG reçoit Ph->Pm puis Pm vers Pb, et aD reçoit Ph->Pb */
    _abscisses(&(t->v[haut]), &(t->v[bas]), aD, 1);
    _abscisses(&(t->v[haut]), &(t->v[median]), aG, 1);
    _abscisses(&(t->v[median]), &(t->v[bas]), &aG[t->v[haut].y - t->v[median].y], 0);
  }
  int h = get_height();
  for(i = 0; i < n; ++i) {
    if( aG[i].y >= 0 && aG[i].y < h )
      _hline(&aG[i], &aD[i]);
  }
  free(aG);
  free(aD);  
}

void _abscisses(vertex_t * p0, vertex_t * p1, vertex_t * absc, int replace) {
  int dx, dy;
  int u = p1->x - p0->x, v = p1->y - p0->y, pasX = u < 0 ? -1 : 1, pasY = v < 0 ? -1 : 1;
  float d = sqrt(u * u + v * v), w = 0.0f, cw = 1.0f;
  u = abs(u); v = abs(v);
  if(u > v) { // 1er octan
    if(replace) {
      int objX = (u + 1) * pasX;
      int delta = u - 2 * v, incH = -2 * v, incO = 2 * u - 2 * v;
      for (int x = 0, y = 0, k = 0; x != objX; x += pasX) {
	absc[k].x = x + p0->x;
	absc[k].y = y + p0->y;
	dx = absc[k].x - p0->x;
	dy = absc[k].y - p0->y;
	w = sqrt(dx * dx + dy * dy) / d;
	cw = 1.0f - w;
	absc[k].r = w * p1->r + cw * p0->r;
	absc[k].g = w * p1->g + cw * p0->g;
	absc[k].b = w * p1->b + cw * p0->b;
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
	  dx = absc[k].x - p0->x;
	  dy = absc[k].y - p0->y;
	  w = sqrt(dx * dx + dy * dy) / d;
	  cw = 1.0f - w;
	  absc[k].r = w * p1->r + cw * p0->r;
	  absc[k].g = w * p1->g + cw * p0->g;
	  absc[k].b = w * p1->b + cw * p0->b;
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
  } else { // 2nd octan
    int objY = (v + 1) * pasY;
    int delta = v - 2 * u, incH = -2 * u, incO = 2 * v - 2 * u;
    for (int x = 0, y = 0, k = 0; y != objY; y += pasY) {
      absc[k].x = x + p0->x;
      absc[k].y = y + p0->y;
      dx = absc[k].x - p0->x;
      dy = absc[k].y - p0->y;
      w = sqrt(dx * dx + dy * dy) / d;
      cw = 1.0f - w;
      absc[k].r = w * p1->r + cw * p0->r;
      absc[k].g = w * p1->g + cw * p0->g;
      absc[k].b = w * p1->b + cw * p0->b;
      ++k;
      if(delta < 0) {
	x += pasX;
        delta += incO;
      } else
        delta += incH;
    }
  }
}

/* droite horizontale ATTENTION p0->x <= p1->x */
void _hline(vertex_t * p0, vertex_t * p1) {
  int x0 = p0->x, x1 = p1->x, y = p0->y; /* y = p1->y est possible aussi */
  int w = get_width(), h = get_height();
  if(y < 0 || y >= h) /* pas besoin de dessiner */
    return;
  /* x le plus à gauche, x le plus à droite */
  x0 = x0 < 0 ? 0 : x0;
  x1 = x1 >= w ? w - 1 : x1;
  if(x0 >= w || x1 < 0) /* pas besoin de dessiner */
    return;
  float d = p1->x - p0->x;
  float ww = 0.0f, cww = 1.0f;
  GLuint * p = get_pixels();
  /* voir si mieux avec une sorte de memset */
  for(int x = x0, yw = y * w; x <= x1; ++x) {
    GLubyte r, g, b;
    ww = (x - x0) / d; cww = 1.0f - ww;
    r = (ww * p1->r + cww * p0->r) * 255.999f;
    g = (ww * p1->g + cww * p0->g) * 255.999f;
    b = (ww * p1->b + cww * p0->b) * 255.999f;
    p[yw + x] = rgb(r, g, b);
  }  
}
