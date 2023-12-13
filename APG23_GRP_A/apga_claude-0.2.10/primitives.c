#include "primitives.h"
#include "claude.h"
#include <stdlib.h>
#include <assert.h>

static uint32_t * _texels = NULL;
static int _tw = 0, _th = 0;

static inline void _hline(vertex_t * p0, vertex_t * p1);
static inline void _abscisses(const vertex_t * p0, const vertex_t * p1, vertex_t * absc, int replace);
static inline void _persp_correct(const vertex_t * p0, const vertex_t * p1, vertex_t * res, float * w0, float * w1);
static inline void _persp_correct2(const vertex_t * p0, const vertex_t * p1, float * w0, float * w1);
static inline void _interpolate(const vertex_t * p0, const vertex_t * p1, vertex_t * res, float w0, float w1);

void fill_triangle(const triangle_t * t) {
  int bas, median, haut, i;
  if(t->v[0].ye < t->v[1].ye) {
    if(t->v[0].ye < t->v[2].ye) {
      bas = 0;
      if(t->v[1].ye < t->v[2].ye) {
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
    if(t->v[1].ye < t->v[2].ye) {
      bas = 1;
      if(t->v[0].ye < t->v[2].ye) {
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
  int signe, n = t->v[haut].ye - t->v[bas].ye + 1;
  /* if(n == 1) return; */
  vertex_t * aG = malloc(n * sizeof *aG);
  assert(aG);
  vertex_t * aD = malloc(n * sizeof *aD);
  assert(aD);

  /* est-ce que Pm est à gauche (+) ou à droite (-) de la droite (Pb->Ph) ? */
  /* idée TODO?, un produit vectoriel pourrait s'avérer mieux */
  if(t->v[haut].xe == t->v[bas].xe || t->v[haut].ye == t->v[bas].ye) {
    /* eq de la droite x = t->v[haut].xe; ou y = t->v[haut].ye; */
    signe = (t->v[median].xe > t->v[haut].xe) ? -1 : 1;
  } else {
    /* eq ax + y + c = 0 */
    float a, c, x;
    a = (t->v[haut].ye - t->v[bas].ye) / (float)(t->v[bas].xe - t->v[haut].xe);
    c = -a * t->v[haut].xe - t->v[haut].ye;
    /* on cherche le x sur la DROITE au même y que le median et on compare */
    x = -(c + t->v[median].ye) / a;
    signe = (t->v[median].xe >= x) ? -1 : 1;
  }
  if(signe < 0) { /* aG reçoit Ph->Pb, et aD reçoit Ph->Pm puis Pm vers Pb */
    _abscisses(&(t->v[haut]), &(t->v[bas]), aG, 1);
    _abscisses(&(t->v[haut]), &(t->v[median]), aD, 1);
    _abscisses(&(t->v[median]), &(t->v[bas]), &aD[t->v[haut].ye - t->v[median].ye], 0);
  } else { /* aG reçoit Ph->Pm puis Pm vers Pb, et aD reçoit Ph->Pb */
    _abscisses(&(t->v[haut]), &(t->v[bas]), aD, 1);
    _abscisses(&(t->v[haut]), &(t->v[median]), aG, 1);
    _abscisses(&(t->v[median]), &(t->v[bas]), &aG[t->v[haut].ye - t->v[median].ye], 0);
  }
  int h = get_height();
  for(i = 0; i < n; ++i) {
    if( aG[i].ye >= 0 && aG[i].ye < h )
      _hline(&aG[i], &aD[i]);
  }
  free(aG);
  free(aD);  
}

texture_t * gen_texture(const char * file) {
  SDL_Surface * s = SDL_LoadBMP(file);
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

void apply_texture(texture_t * t) {
  _tw = t->w;
  _th = t->h;
  _texels = t->rgba;
}

void free_texture(texture_t * t) {
  free(t->rgba);
  free(t);
}

void _abscisses(const vertex_t * p0, const vertex_t * p1, vertex_t * absc, int replace) {
  int dx, dy;
  int u = p1->xe - p0->xe, v = p1->ye - p0->ye, pasX = u < 0 ? -1 : 1, pasY = v < 0 ? -1 : 1;
  float d = sqrt(u * u + v * v), w = 0.0f, cw = 1.0f;
  if(d == 0.0f) { /* TODO : points confondus, solution bof */
    *absc = *p0;
    return;
  }
  u = abs(u); v = abs(v);
  if(u > v) { // 1er octan
    if(replace) {
      int objX = (u + 1) * pasX;
      int delta = u - 2 * v, incH = -2 * v, incO = 2 * u - 2 * v;
      for (int x = 0, y = 0, k = 0; x != objX; x += pasX) {
	absc[k].xe = x + p0->xe;
	absc[k].ye = y + p0->ye;
	dx = absc[k].xe - p0->xe;
	dy = absc[k].ye - p0->ye;
	w = sqrt(dx * dx + dy * dy) / d;
	cw = 1.0f - w;
	_persp_correct(p0, p1, &(absc[k]), &cw, &w);
	_interpolate(p0, p1, &(absc[k]), cw, w);
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
	  absc[k].xe = x + p0->xe;
	  absc[k].ye = y + p0->ye;
	  dx = absc[k].xe - p0->xe;
	  dy = absc[k].ye - p0->ye;
	  w = sqrt(dx * dx + dy * dy) / d;
	  cw = 1.0f - w;
	  _persp_correct(p0, p1, &(absc[k]), &cw, &w);
	  _interpolate(p0, p1, &(absc[k]), cw, w);
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
      absc[k].xe = x + p0->xe;
      absc[k].ye = y + p0->ye;
      dx = absc[k].xe - p0->xe;
      dy = absc[k].ye - p0->ye;
      w = sqrt(dx * dx + dy * dy) / d;
      cw = 1.0f - w;
      _persp_correct(p0, p1, &(absc[k]), &cw, &w);
      _interpolate(p0, p1, &(absc[k]), cw, w);
      ++k;
      if(delta < 0) {
	x += pasX;
        delta += incO;
      } else
        delta += incH;
    }
  }
}

/* droite horizontale ATTENTION p0->xe <= p1->xe */
void _hline(vertex_t * p0, vertex_t * p1) {
  int x0 = p0->xe, x1 = p1->xe, y = p0->ye; /* y = p1->ye est possible aussi */
  int w = get_width(), h = get_height();
  if(y < 0 || y >= h) /* pas besoin de dessiner */
    return;
  /* TODO : test pas fou, et pas le meilleur endroit */
  if( (p0->d < 0.0f && p1->d < 0.0f) ||
      (p0->d > 1.0f && p1->d > 1.0f) ||
      isnan(p0->d) || isinf(p0->d)   ||
      isnan(p1->d) || isinf(p1->d)     ) {
    return;
  }
  /* x le plus à gauche, x le plus à droite */
  x0 = x0 < 0 ? 0 : x0;
  x1 = x1 >= w ? w - 1 : x1;
  if(x0 >= w || x1 < 0) /* pas besoin de dessiner */
    return;
  float d = p1->xe - p0->xe;
  if(d == 0.0f) d = CL_EPSILON;
  float ww = 0.0f, cww = 1.0f;
  uint32_t * p = get_pixels();
  float * depths = get_depths();
  
  /* voir si mieux avec une sorte de memset */
  for(int x = x0, yw = y * w; x <= x1; ++x) {
    float il, alpha;
    uint8_t r, g, b;
    int s, t;
    uint32_t tex;
    ww = (x - x0) / d; cww = 1.0f - ww;
    _persp_correct2(p0, p1, &cww, &ww);
    
    float depth = (ww * p1->d + cww * p0->d);
    /* le test de profondeur */
    if(depth < depths[yw + x]) continue;
    depths[yw + x] = depth; /* maj */
    il = (ww * p1->il + cww * p0->il); /* interpolation de l'incidence de la lumière */
    r = il * (ww * p1->r + cww * p0->r) * (256.0f - CL_EPSILON);
    g = il * (ww * p1->g + cww * p0->g) * (256.0f - CL_EPSILON);
    b = il * (ww * p1->b + cww * p0->b) * (256.0f - CL_EPSILON);
    alpha = (ww * p1->a + cww * p0->a);
    s = (ww * p1->s + cww * p0->s) * (_tw - CL_EPSILON);
    t = (ww * p1->t + cww * p0->t) * (_th - CL_EPSILON);
    /* on fait un clamp */
    if(s < 0) s = 0; if(s > _tw - 1) s = _tw - 1;
    if(t < 0) t = 0; if(t > _th - 1) t = _th - 1;
    tex = _texels[t * _tw + s];
    /* on fait le choix de la multiplication */
    r *= red(tex)   / 255.0f;
    g *= green(tex) / 255.0f;
    b *= blue(tex)  / 255.0f;
    /* r = depth * (256.0f - CL_EPSILON); */
    /* g = depth * (256.0f - CL_EPSILON); */
    /* b = depth * (256.0f - CL_EPSILON); */
    if(alpha < 1.0f) {
      uint8_t or, og, ob;
      or = red(p[yw + x]);
      og = green(p[yw + x]);
      ob = blue(p[yw + x]);
      p[yw + x] = rgb(r * alpha + or * (1.0f - alpha), g * alpha + og * (1.0f - alpha), b * alpha + ob * (1.0f - alpha));
    } else 
      p[yw + x] = rgb(r, g, b);
  }  
}

void _persp_correct(const vertex_t * p0, const vertex_t * p1, vertex_t * res, float * w0, float * w1) { 
  /* TODO : Attention, block de correction de
     perspective. Mettre en place un booléen pour tester si
     nécessaire (projection perspective */
  /* Correction de l'interpolation par rapport à la perspective, le z
   * joue un rôle dans les distances, il est nécessaire de le
   * réintégrer en modifiant les facteurs de proportion.
   * lien utile : https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/perspective-correct-interpolation-vertex-attributes 
   */ 
  float z = 1.0f / (*w0 / p0->zmod + *w1 / p1->zmod);
  res->zmod = *w0 * p0->zmod + *w1 * p1->zmod;
  *w0 = z * *w0 / p0->zmod; *w1 = z * *w1 / p1->zmod;
}

void _persp_correct2(const vertex_t * p0, const vertex_t * p1, float * w0, float * w1) { 
  /* TODO : Attention, block de correction de
     perspective. Mettre en place un booléen pour tester si
     nécessaire (projection perspective */
  /* Correction de l'interpolation par rapport à la perspective, le z
   * joue un rôle dans les distances, il est nécessaire de le
   * réintégrer en modifiant les facteurs de proportion.
   * lien utile : https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/perspective-correct-interpolation-vertex-attributes 
   */ 
  float z = 1.0f / (*w0 / p0->zmod + *w1 / p1->zmod);
  *w0 = z * *w0 / p0->zmod; *w1 = z * *w1 / p1->zmod;
}

void _interpolate(const vertex_t * p0, const vertex_t * p1, vertex_t * res, float w0, float w1) {
  res->r = w1 * p1->r + w0 * p0->r;
  res->g = w1 * p1->g + w0 * p0->g;
  res->b = w1 * p1->b + w0 * p0->b;
  res->a = w1 * p1->a + w0 * p0->a;
  res->s = w1 * p1->s + w0 * p0->s;
  res->t = w1 * p1->t + w0 * p0->t;
  res->d = w1 * p1->d + w0 * p0->d;
  res->il = w1 * p1->il + w0 * p0->il; /* interpolation de l'incidence de la lumière */
}
