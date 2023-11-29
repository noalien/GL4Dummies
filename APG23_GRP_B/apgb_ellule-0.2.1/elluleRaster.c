#include "elluleRaster.h"
#include <stdlib.h>
#include <assert.h>

static int _tw = 0, _th = 0;
static uint32_t * _texels = NULL;

static inline void _hline(vertex_t * aG, vertex_t * aD);
static inline void _abscisses(vertex_t * p0, vertex_t * p1, vertex_t * absc, int replace);
static inline void _perspcorrect(vertex_t * p0, vertex_t * p1, float * zmod, float * p0w, float * p1w);
static inline void _perspcorrectlight(vertex_t * p0, vertex_t * p1, float * p0w, float * p1w);

void elFillTriangle(triangle_t * t) {
  int bas, median, haut, i;
  if(t->v[0].iCoord[1] < t->v[1].iCoord[1]) {
    if(t->v[0].iCoord[1] < t->v[2].iCoord[1]) {
      bas = 0;
      if(t->v[1].iCoord[1] < t->v[2].iCoord[1]) {
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
    if(t->v[1].iCoord[1] < t->v[2].iCoord[1]) {
      bas = 1;
      if(t->v[0].iCoord[1] < t->v[2].iCoord[1]) {
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
  int signe, n = t->v[haut].iCoord[1] - t->v[bas].iCoord[1] + 1;
  /* TODO : gérer ce problème */
  if(n <= 0 || n > 10000) return;
  vertex_t * aG = malloc(n * sizeof *aG);
  assert(aG);
  vertex_t * aD = malloc(n * sizeof *aD);
  assert(aD);

  /* est-ce que Pm est à gauche (+) ou à droite (-) de la droite (Pb->Ph) ? */
  /* idée TODO?, un produit vectoriel pourrait s'avérer mieux */
  if(t->v[haut].iCoord[0] == t->v[bas].iCoord[0] || t->v[haut].iCoord[1] == t->v[bas].iCoord[1]) {
    /* eq de la droite x = t->v[haut].iCoord[0]; ou y = t->v[haut].iCoord[1]; */
    signe = (t->v[median].iCoord[0] > t->v[haut].iCoord[0]) ? -1 : 1;
  } else {
    /* eq ax + y + c = 0 */
    float a, c, x;
    a = (t->v[haut].iCoord[1] - t->v[bas].iCoord[1]) / (float)(t->v[bas].iCoord[0] - t->v[haut].iCoord[0]);
    c = -a * t->v[haut].iCoord[0] - t->v[haut].iCoord[1];
    /* on cherche le x sur la DROITE au même y que le median et on compare */
    x = -(c + t->v[median].iCoord[1]) / a;
    signe = (t->v[median].iCoord[0] >= x) ? -1 : 1;
  }
  if(signe < 0) { /* aG reçoit Ph->Pb, et aD reçoit Ph->Pm puis Pm vers Pb */
    _abscisses(&(t->v[haut]), &(t->v[bas]), aG, 1);
    _abscisses(&(t->v[haut]), &(t->v[median]), aD, 1);
    _abscisses(&(t->v[median]), &(t->v[bas]), &aD[t->v[haut].iCoord[1] - t->v[median].iCoord[1]], 0);
  } else { /* aG reçoit Ph->Pm puis Pm vers Pb, et aD reçoit Ph->Pb */
    _abscisses(&(t->v[haut]), &(t->v[bas]), aD, 1);
    _abscisses(&(t->v[haut]), &(t->v[median]), aG, 1);
    _abscisses(&(t->v[median]), &(t->v[bas]), &aG[t->v[haut].iCoord[1] - t->v[median].iCoord[1]], 0);
  }
  int h = elGetHeight();
  for(i = 0; i < n; ++i) {
    if( aG[i].iCoord[1] >= 0 && aG[i].iCoord[1] < h )
      _hline(&aG[i], &aD[i]);
  }
  free(aG);
  free(aD);  
}

/* pour libérer la texture dans _texels */
static void _quit(void) {
  if(_texels) {
    free(_texels);
    _texels = NULL;
  }
}

void elUseTexture(const char * filename) {
  SDL_Surface * s = SDL_LoadBMP(filename);
  assert(s);
  /* assert(s->format->BytesPerPixel == 4);
     plus besoin, on convertit la surface au
     format souhaité */
  SDL_Surface * d = SDL_CreateRGBSurface(0, s->w, s->h, 32, R_MASK, G_MASK, B_MASK, A_MASK);
  SDL_BlitSurface(s, NULL, d, NULL);
  SDL_FreeSurface(s);
  _tw = d->w;
  _th = d->h;
  if(_texels)
    free(_texels);
  else
    atexit(_quit);
  _texels = malloc(_tw * _th * sizeof *_texels);
  assert(_texels);
  memcpy(_texels, d->pixels, _tw * _th * sizeof *_texels);
  SDL_FreeSurface(d);
}

void _abscisses(vertex_t * p0, vertex_t * p1, vertex_t * absc, int replace) {
  int u = p1->iCoord[0] - p0->iCoord[0], v = p1->iCoord[1] - p0->iCoord[1], pasX = u < 0 ? -1 : 1, pasY = v < 0 ? -1 : 1;
  u = abs(u); v = abs(v);
  float d = sqrt(u * u + v * v), pp0, pp1;
  if(u > v) { // 1er octan
    if(replace) {
      int objX = (u + 1) * pasX;
      int delta = u - 2 * v, incH = -2 * v, incO = 2 * u - 2 * v;
      for (int x = 0, y = 0, k = 0; x != objX; x += pasX) {
	absc[k].iCoord[0] = x + p0->iCoord[0];
	absc[k].iCoord[1] = y + p0->iCoord[1];
	pp1 = sqrt(x * x + y * y) / d;
	pp0 = (1.0f - pp1);
	_perspcorrect(p0, p1, &(absc[k].mvpos[2]), &pp0, &pp1);
	absc[k].color[0] = pp0 * p0->color[0] + pp1 * p1->color[0];
	absc[k].color[1] = pp0 * p0->color[1] + pp1 * p1->color[1];
	absc[k].color[2] = pp0 * p0->color[2] + pp1 * p1->color[2];
	absc[k].color[3] = pp0 * p0->color[3] + pp1 * p1->color[3];
	absc[k].texCoord[0] = pp0 * p0->texCoord[0] + pp1 * p1->texCoord[0];
	absc[k].texCoord[1] = pp0 * p0->texCoord[1] + pp1 * p1->texCoord[1];
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
	  absc[k].iCoord[0] = x + p0->iCoord[0];
	  absc[k].iCoord[1] = y + p0->iCoord[1];
	  pp1 = sqrt(x * x + y * y) / d;
	  pp0 = (1.0f - pp1);
	  _perspcorrect(p0, p1, &(absc[k].mvpos[2]), &pp0, &pp1);
	  absc[k].color[0] = pp0 * p0->color[0] + pp1 * p1->color[0];
	  absc[k].color[1] = pp0 * p0->color[1] + pp1 * p1->color[1];
	  absc[k].color[2] = pp0 * p0->color[2] + pp1 * p1->color[2];
	  absc[k].color[3] = pp0 * p0->color[3] + pp1 * p1->color[3];
	  absc[k].texCoord[0] = pp0 * p0->texCoord[0] + pp1 * p1->texCoord[0];
	  absc[k].texCoord[1] = pp0 * p0->texCoord[1] + pp1 * p1->texCoord[1];
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
      absc[k].iCoord[0] = x + p0->iCoord[0];
      absc[k].iCoord[1] = y + p0->iCoord[1];
      pp1 = sqrt(x * x + y * y) / d;
      pp0 = (1.0f - pp1);
      _perspcorrect(p0, p1, &(absc[k].mvpos[2]), &pp0, &pp1);
      absc[k].color[0] = pp0 * p0->color[0] + pp1 * p1->color[0];
      absc[k].color[1] = pp0 * p0->color[1] + pp1 * p1->color[1];
      absc[k].color[2] = pp0 * p0->color[2] + pp1 * p1->color[2];
      absc[k].color[3] = pp0 * p0->color[3] + pp1 * p1->color[3];
      absc[k].texCoord[0] = pp0 * p0->texCoord[0] + pp1 * p1->texCoord[0];
      absc[k].texCoord[1] = pp0 * p0->texCoord[1] + pp1 * p1->texCoord[1];
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
  int x0 = aG->iCoord[0], x1 = aD->iCoord[0], y = aG->iCoord[1];
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
    _perspcorrectlight(aG, aD, &paG, &paD);
    r = (uint8_t)((paG * aG->color[0] + paD * aD->color[0]) * (256.0f - EL_EPSILON));
    g = (uint8_t)((paG * aG->color[1] + paD * aD->color[1]) * (256.0f - EL_EPSILON));
    b = (uint8_t)((paG * aG->color[2] + paD * aD->color[2]) * (256.0f - EL_EPSILON));
    a = (uint8_t)((paG * aG->color[3] + paD * aD->color[3]) * (256.0f - EL_EPSILON));
    s = (uint8_t)((paG * aG->texCoord[0] + paD * aD->texCoord[0]) * (_tw - EL_EPSILON));
    t = (uint8_t)((paG * aG->texCoord[1] + paD * aD->texCoord[1]) * (_th - EL_EPSILON));
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

/*!\brief correction de la perspective à l'aide du z-mod-view */
void _perspcorrect(vertex_t * p0, vertex_t * p1, float * zmod, float * p0w, float * p1w) {
  /* TODO : Attention, block de correction de
     perspective. Mettre en place un booléen pour tester si
     nécessaire (projection perspective) */
  /* Correction de l'interpolation par rapport à la perspective,
   * le z joue un rôle dans les distances, il est nécessaire de
   * le réintégrer en modifiant les facteurs de proportion.
   * lien utile :
   * https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/perspective-correct-interpolation-vertex-attributes
   */
  float z = 1.0f / (*p0w / p0->mvpos[2] + *p1w / p1->mvpos[2]);
  *zmod = *p0w * p0->mvpos[2] + *p1w * p1->mvpos[2];
  *p0w = z * *p0w / p0->mvpos[2];
  *p1w = z * *p1w / p1->mvpos[2];
}

/*!\brief version light de \ref _perspcorrect, pas besoin de maj de z-mod-view */
void _perspcorrectlight(vertex_t * p0, vertex_t * p1, float * p0w, float * p1w) {
  float z = 1.0f / (*p0w / p0->mvpos[2] + *p1w / p1->mvpos[2]);
  *p0w = z * *p0w / p0->mvpos[2];
  *p1w = z * *p1w / p1->mvpos[2];
}
