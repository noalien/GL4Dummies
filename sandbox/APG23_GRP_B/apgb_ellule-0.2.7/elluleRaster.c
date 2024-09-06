#include "elluleRaster.h"
#include <stdlib.h>
#include <assert.h>

static int _tw = 1, _th = 1;
static uint32_t _rouge = 0xFF0000FF, * _texels = &_rouge; /* si pas de texture, on utilise un pixel rouge pour alerter */

static enum rendering_opt_t _rendering_options = EL_COLOR; /* seule la couleur est activée par défaut */

static inline void _useTexture(texture_t * t);
static inline void _hline(vertex_t * aG, vertex_t * aD);
static inline void _abscisses(vertex_t * p0, vertex_t * p1, vertex_t * absc, int replace);
static inline void _perspcorrect(vertex_t * p0, vertex_t * p1, float * zmod, float * p0w, float * p1w);
static inline void _perspcorrectlight(vertex_t * p0, vertex_t * p1, float * p0w, float * p1w);
static inline void _interpolate(vertex_t * res, vertex_t * p0, float pp0, vertex_t * p1, float pp1);

void elFillTriangle(surface_t * s, triangle_t * t) {
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
  /* if(n <= 0 || n > 10000) return; */

  _useTexture(s->tex);

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

void _useTexture(texture_t * t) {
  _tw = t ? t->w : 1;
  _th = t ? t->h : 1;
  _texels = t ? t->rgba : &_rouge;
}

texture_t * elGenTexture(const char * filename) {
  SDL_Surface * s = SDL_LoadBMP(filename);
  assert(s);
  SDL_Surface * d = SDL_CreateRGBSurface(0, s->w, s->h, 32, R_MASK, G_MASK, B_MASK, A_MASK);
  SDL_BlitSurface(s, NULL, d, NULL);
  SDL_FreeSurface(s);
  texture_t * t = malloc(sizeof *t);
  t->w = d->w;
  t->h = d->h;
  t->rgba = malloc(t->w * t->h * sizeof *(t->rgba));
  assert(t->rgba);
  memcpy(t->rgba, d->pixels, t->w * t->h * sizeof *(t->rgba));
  SDL_FreeSurface(d);
  return t;
}

void elFreeTexture(texture_t * t) {
  free(t->rgba);
  free(t);
}

void elEnable(enum rendering_opt_t opt) {
  _rendering_options |= opt;
}

void elDisable(enum rendering_opt_t opt) {
  _rendering_options &= ~opt;
}

int elIsEnabled(enum rendering_opt_t opt) {
  return _rendering_options & opt;
}

void _abscisses(vertex_t * p0, vertex_t * p1, vertex_t * absc, int replace) {
  int u = p1->iCoord[0] - p0->iCoord[0], v = p1->iCoord[1] - p0->iCoord[1], pasX = u < 0 ? -1 : 1, pasY = v < 0 ? -1 : 1;
  u = abs(u); v = abs(v);
  float d = sqrt(u * u + v * v), pp0, pp1;
  if(d == 0.0f) { /* points confondus */
    *absc = *p0;
    return;
  }
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
	_interpolate(&absc[k], p0, pp0, p1, pp1);
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
	  _interpolate(&absc[k], p0, pp0, p1, pp1);
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
      _interpolate(&absc[k], p0, pp0, p1, pp1);
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
  float d, dx, paD, paG, depth, * depths, r, g, b, a;
  if(y < 0 || y >= h) /* pas besoin de dessiner */
    return;
  /* x le plus à gauche, x le plus à droite */
  x0 = x0 < 0 ? 0 : x0;
  x1 = x1 >= w ? w - 1 : x1;
  if(x0 >= w || x1 < 0) /* pas besoin de dessiner */
    return;
  /* ATTENTION ICI (peut-être aussi plus haut) */
  d = x1 - x0 + EL_EPSILON;
  uint32_t * p = elGetPixels();
  depths = elGetDepths();
  
  for(int x = x0, yw = y * w; x <= x1; ++x) {
    dx = x - x0 + EL_EPSILON;
    paD = dx / d; paG = 1.0f - paD;
    _perspcorrectlight(aG, aD, &paG, &paD);
    depth = paG * aG->mvppos[2] + paD * aD->mvppos[2];
    if(depths[yw + x] > depth) continue; /* sinon ce z passe */
    /* on l'écrit */
    depths[yw + x] = depth;
    
    /* couleurs */
    r  = _max(_min(paG * aG->color[0] + paD * aD->color[0], 1.0f), 0.0f);
    g  = _max(_min(paG * aG->color[1] + paD * aD->color[1], 1.0f), 0.0f);
    b  = _max(_min(paG * aG->color[2] + paD * aD->color[2], 1.0f), 0.0f);
    a  = _max(_min(paG * aG->color[3] + paD * aD->color[3], 1.0f), 0.0f);
    if(_rendering_options & EL_TEXTURE) {
      int s, t;
      /* coordonnées de texture */
      s  = (int)((paG * aG->texCoord[0] + paD * aD->texCoord[0]) * (_tw - 1.0f));
      t  = (int)((paG * aG->texCoord[1] + paD * aD->texCoord[1]) * (_th - 1.0f));
      /* on fait un repeat */
      s = s % _tw; while(s < 0) s += _tw;
      t = t % _th; while(t < 0) t += _th;
      /* on fait le choix de la multiplication texture par couleur */
      uint32_t tex = _texels[t * _tw + s];
      r *=   _red(tex) / 255.0f;
      g *= _green(tex) / 255.0f;
      b *=  _blue(tex) / 255.0f;
      a *= _alpha(tex) / 255.0f;
    }
    if(_rendering_options & EL_ALPHA) {
      uint32_t old = p[yw + x];
      float ca = 1.0f - a;
      r = a * r + ca * (_red(old) / 255.0f);
      g = a * g + ca * (_green(old) / 255.0f);
      b = a * b + ca * (_blue(old) / 255.0f);
    }
    p[yw + x] = _rgba( (uint8_t)(r * (256.0f - EL_EPSILON)),
		       (uint8_t)(g * (256.0f - EL_EPSILON)),
		       (uint8_t)(b * (256.0f - EL_EPSILON)),
		       255 );
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

/*!\brief interpolation */
void _interpolate(vertex_t * res, vertex_t * p0, float pp0, vertex_t * p1, float pp1) {
  if(_rendering_options & EL_SHADING) {
    res->il          = pp0 * p0->il          + pp1 * p1->il;
    res->mvppos[2]   = pp0 * p0->mvppos[2]   + pp1 * p1->mvppos[2];
    res->texCoord[0] = pp0 * p0->texCoord[0] + pp1 * p1->texCoord[0];
    res->texCoord[1] = pp0 * p0->texCoord[1] + pp1 * p1->texCoord[1];
    res->color[3]    = pp0 * p0->color[3] + pp1 * p1->color[3];
    pp0 *= res->il; pp1 *= res->il; /* atténuation de rgb en fonction de il */
  } else {
    res->mvppos[2]   = pp0 * p0->mvppos[2]   + pp1 * p1->mvppos[2];
    res->texCoord[0] = pp0 * p0->texCoord[0] + pp1 * p1->texCoord[0];
    res->texCoord[1] = pp0 * p0->texCoord[1] + pp1 * p1->texCoord[1];
    res->color[3]    = pp0 * p0->color[3] + pp1 * p1->color[3];
  }
  
  res->color[0]    = pp0 * p0->color[0] + pp1 * p1->color[0];
  res->color[1]    = pp0 * p0->color[1] + pp1 * p1->color[1];
  res->color[2]    = pp0 * p0->color[2] + pp1 * p1->color[2];
}
