#include "elluleRaster.h"
#include <stdlib.h>
#include <assert.h>

static int _tw = 0, _th = 0;
static uint32_t * _texels = NULL;

static inline void _hline(vertex_t * aG, vertex_t * aD);
static inline void _abscisses(vertex_t * p0, vertex_t * p1, vertex_t * absc, int replace);

void elFillTriangle(triangle_t * t) {
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
  int h = elGetHeight();
  for(i = 0; i < n; ++i) {
    if( aG[i].y >= 0 && aG[i].y < h )
      _hline(&aG[i], &aD[i]);
  }
  free(aG);
  free(aD);  
}

void elUseTexture(const char * filename) {
  SDL_Surface * s = SDL_LoadBMP(filename);
  if(s == NULL) {
    if(_texels != NULL) {
      free(_texels);
      _texels = NULL;
    }
    return;
  }
  assert(s->format->BytesPerPixel == 4);
  _tw = s->w;
  _th = s->h;
  _texels = malloc(_tw * _th * sizeof *_texels);
  assert(_texels);
  memcpy(_texels, s->pixels, _tw * _th * sizeof *_texels);
  SDL_FreeSurface(s);
}


void _abscisses(vertex_t * p0, vertex_t * p1, vertex_t * absc, int replace) {
  int u = p1->x - p0->x, v = p1->y - p0->y, pasX = u < 0 ? -1 : 1, pasY = v < 0 ? -1 : 1;
  u = abs(u); v = abs(v);
  float d = sqrt(u * u + v * v), pp1;
  if(u > v) { // 1er octan
    if(replace) {
      int objX = (u + 1) * pasX;
      int delta = u - 2 * v, incH = -2 * v, incO = 2 * u - 2 * v;
      for (int x = 0, y = 0, k = 0; x != objX; x += pasX) {
	absc[k].x = x + p0->x;
	absc[k].y = y + p0->y;
	pp1 = sqrt(x * x + y * y) / d;
	absc[k].r = (1.0f - pp1) * p0->r + pp1 * p1->r;
	absc[k].g = (1.0f - pp1) * p0->g + pp1 * p1->g;
	absc[k].b = (1.0f - pp1) * p0->b + pp1 * p1->b;
	absc[k].a = (1.0f - pp1) * p0->a + pp1 * p1->a;
	absc[k].s = (1.0f - pp1) * p0->s + pp1 * p1->s;
	absc[k].t = (1.0f - pp1) * p0->t + pp1 * p1->t;
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
	  pp1 = sqrt(x * x + y * y) / d;
	  absc[k].r = (1.0f - pp1) * p0->r + pp1 * p1->r;
	  absc[k].g = (1.0f - pp1) * p0->g + pp1 * p1->g;
	  absc[k].b = (1.0f - pp1) * p0->b + pp1 * p1->b;
	  absc[k].a = (1.0f - pp1) * p0->a + pp1 * p1->a;
	  absc[k].s = (1.0f - pp1) * p0->s + pp1 * p1->s;
	  absc[k].t = (1.0f - pp1) * p0->t + pp1 * p1->t;
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
      pp1 = sqrt(x * x + y * y) / d;
      absc[k].r = (1.0f - pp1) * p0->r + pp1 * p1->r;
      absc[k].g = (1.0f - pp1) * p0->g + pp1 * p1->g;
      absc[k].b = (1.0f - pp1) * p0->b + pp1 * p1->b;
      absc[k].a = (1.0f - pp1) * p0->a + pp1 * p1->a;
      absc[k].s = (1.0f - pp1) * p0->s + pp1 * p1->s;
      absc[k].t = (1.0f - pp1) * p0->t + pp1 * p1->t;
      ++k;
      if(delta < 0) {
	x += pasX;
        delta += incO;
      } else
        delta += incH;
    }
  }
}

/* droite horizontale ATTENTION x0 <= x1 */
void _hline(vertex_t * aG, vertex_t * aD) {
  int x0 = aG->x, x1 = aD->x, y = aG->y;
  int w = elGetWidth(), h = elGetHeight();
  float d, dx, paD, paG;
  if(y < 0 || y >= h) /* pas besoin de dessiner */
    return;
  /* x le plus à gauche, x le plus à droite */
  x0 = x0 < 0 ? 0 : x0;
  x1 = x1 >= w ? w - 1 : x1;
  if(x0 >= w || x1 < 0) /* pas besoin de dessiner */
    return;
  d = x1 - x0;
  GLuint * p = elGetPixels();
  /* voir si mieux avec une sorte de memset */
  for(int x = x0, yw = y * w; x <= x1; ++x) {
    uint8_t r, g, b, a;
    int s, t;
    dx = x - x0;
    paD = dx / d; paG = 1.0f - paD;
    r = (uint8_t)((paG * aG->r + paD * aD->r) * (256.0f - EL_EPSILON));
    g = (uint8_t)((paG * aG->g + paD * aD->g) * (256.0f - EL_EPSILON));
    b = (uint8_t)((paG * aG->b + paD * aD->b) * (256.0f - EL_EPSILON));
    a = (uint8_t)((paG * aG->a + paD * aD->a) * (256.0f - EL_EPSILON));
    s = (uint8_t)((paG * aG->s + paD * aD->s) * (_tw - EL_EPSILON));
    t = (uint8_t)((paG * aG->t + paD * aD->t) * (_th - EL_EPSILON));
    /* on fait un repeat */
    s = s % _tw; while(s < 0) s += _tw;
    t = t % _th; while(t < 0) t += _th;
    /* on fait le choix de la multiplication texture par couleur */
    uint32_t tex = _texels[t * _tw + s];
    r *= _red(tex)    / 255.0f;
    g *= _green(tex)  / 255.0f;
    b *= _blue(tex)   / 255.0f;
    a *= _alpha(tex)  / 255.0f;
    p[yw + x] = _rgba(r, g, b, a);
  }
}
