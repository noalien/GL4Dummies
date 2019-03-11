/*!\file window.c
 * \brief GL4Dummies, exemple simple 2D avec GL4Dummies
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date February 04 2018
 */

/* inclusion des entêtes de fonctions de gestion de primitives simples
 * de dessin. La lettre p signifie aussi bien primitive que
 * pédagogique. */
#include <GL4D/gl4dp.h>

typedef struct point2d_t point2d_t;

struct point2d_t {
  int x, y;
  float r, g, b, s, t;
};

/* inclusion des entêtes de fonctions de création et de gestion de
 * fenêtres système ouvrant un contexte favorable à GL4dummies. Cette
 * partie est dépendante de la bibliothèque SDL2 */
#include <GL4D/gl4duw_SDL2.h>
#include <assert.h>
static int _w = 400;
static GLuint ecran0, ecran1;


/* modif : ajouts protos + fonction edist */
static int fillWithLine(point2d_t * a, point2d_t * b, point2d_t * Y, int gauche);
static void triangle(point2d_t * p1, point2d_t * p2, point2d_t * p3);
static void scanline(point2d_t * p1, point2d_t * p2);
static inline float edist(point2d_t * a, point2d_t * b);

static inline float edist(point2d_t * a, point2d_t * b) {
  float dx = a->x - b->x, dy = a->y - b->y;
  return sqrtf(dx * dx + dy * dy);
}

static int fillWithLine(point2d_t * a, point2d_t * b, point2d_t * Y, int gauche) {
  int u = b->x - a->x, v = b->y - a->y, i = 0, libre = 1;
  int pasX = (u < 0) ? -1 : 1, pasY = (v < 0) ? -1 : 1;
  int x, y, del, incH, incO;
  /* modif : ajout distance d entre a et b */
  float d = edist(a, b), ld, cld;
  if(abs(u) < abs(v)) { /* deuxieme octant */
    del = (incH = ((pasX * u) << 1)) - pasY * v;
    incO = incH - ((pasY * v) << 1);
    for(y = a->y, x = a->x; y != b->y; y += pasY, ++i) {
      Y[i].x = x; 
      Y[i].y = y; 
      /* modif : interpolation des couleurs */
      ld  = edist(&Y[i], a) / d;
      cld = (1.0f - ld);
      Y[i].r = a->r * cld + b->r * ld;
      Y[i].g = a->g * cld + b->g * ld;
      Y[i].b = a->b * cld + b->b * ld;
      Y[i].s = a->s * cld + b->s * ld;
      Y[i].t = a->t * cld + b->t * ld;
      if(del < 0) del += incH;
      else {
        del += incO;
        x += pasX;
      }
    }
    return i - 1;
  } else {  /* premier octant */
    del = (incH = ((pasY * v) << 1)) - pasX * u;
    incO = incH - ((pasX * u) << 1);
    for(x = a->x, y = a->y; x != b->x; x += pasX) {
      if(libre || (gauche && pasX < 0) || (!gauche && pasX > 0)) {
	Y[i].x = x; 
	Y[i].y = y;
	/* modif : interpolation des couleurs */
	ld  = edist(&Y[i], a) / d;
	cld = (1.0f - ld);
	Y[i].r = a->r * cld + b->r * ld;
	Y[i].g = a->g * cld + b->g * ld;
	Y[i].b = a->b * cld + b->b * ld;
	Y[i].s = a->s * cld + b->s * ld;
	Y[i].t = a->t * cld + b->t * ld;
	libre = 0;
      }
      if(del < 0) del += incH;
      else {
        del += incO;
        y += pasY;
	++i;
	libre = 1;
      }
    }
    return i; /* modif : le test libre ? i - 1 : i était faux */
  }
}



static void triangle(point2d_t * p1, point2d_t * p2, point2d_t * p3) {
  int n, i;
  point2d_t * a, * b, * c, * ptsG, * ptsD;
  if(p1->y > p2->y) {
    if(p1->y > p3->y) {
      a = p1;
      if(p2->y > p3->y) {
	b = p2;
	c = p3;
      } else {
	b = p3;
	c = p2;
      }
    } else {
      a = p3;
      b = p1;
      c = p2;
    }
  } else {
    if(p2->y > p3->y) {
      a = p2;
      if(p1->y > p3->y) {
	b = p1;
	c = p3;
      } else {
	b = p3;
	c = p1;
      }
    } else {
      a = p3;
      b = p2;
      c = p1;
    }
  }
  n = a->y - c->y + 1;
  ptsG = malloc(n * sizeof *ptsG);
  assert(ptsG);
  ptsD = malloc(n * sizeof *ptsD);
  assert(ptsD);
  if(b->x < c-> x) { // ab + bc à gauche
    int ret;
    ret = fillWithLine(a, b, ptsG, 1);
    fillWithLine(b, c, &ptsG[ret], 1);
    fillWithLine(a, c, ptsD, 0);
  } else { // ab + bc à droite
    int ret;
    ret = fillWithLine(a, b, ptsD, 0);
    fillWithLine(b, c, &ptsD[ret], 0);
    fillWithLine(a, c, ptsG, 1);
  }
  for(i = 0; i < n; ++i) {
    /* modif : utilisation des données pour scan */
    //assert(ptsG[i].y == ptsD[i].y);
    if(ptsG[i].y != ptsD[i].y) continue;
    if(ptsG[i].y < 0 || ptsG[i].y >= gl4dpGetHeight())
      continue;
    scanline(&ptsG[i], &ptsD[i]);
  }
  free(ptsG);
  free(ptsD);
}

static void scanline(point2d_t * p1, point2d_t * p2) {
  int x0, x1, w = gl4dpGetWidth(), x;
  point2d_t * g, * d;
  float dist, dmax, S, T;
  unsigned char R, G, B;
  Uint32 coul;
  assert(p1->y == p2->y);
  if(p1->x < p2->x) {
    g = p1; d = p2;
  } else {
    g = p2; d = p1;
  }
  if(g->y < 0 || g->y >= w)
    return;
  dmax = d->x - g->x;
  dmax = dmax == 0.0f ? 1.0f : dmax;
  x0 = MAX(g->x, 0);
  x1 = MIN(d->x, w - 1);
  for(x = x0; x <= x1; ++x) {
    dist = (x - g->x) / dmax;
    /*R = (unsigned char)(255.0f * (dist * d->r + (1.0f - dist) * g->r));
    G = (unsigned char)(255.0f * (dist * d->g + (1.0f - dist) * g->g));
    B = (unsigned char)(255.0f * (dist * d->b + (1.0f - dist) * g->b));
    gl4dpSetColor(RGB(R, G, B));*/
    S = dist * d->s + (1.0f - dist) * g->s;
    T = dist * d->t + (1.0f - dist) * g->t;
    gl4dpSetScreen(ecran1);
    coul = gl4dpGetPixel(S * (gl4dpGetWidth() - 1), (1.0 - T) * (gl4dpGetHeight() - 1));
    gl4dpSetScreen(ecran0);
    gl4dpSetColor(coul);
    gl4dpPutPixel(x, g->y);
  }
}

void dessin(void) {
  point2d_t a, b;
  static point2d_t c = {100, 500, 0, 0, 1, 0.5f, 1.0f};
  static point2d_t d = {400, 250, 1, 0, 0, 0, 0};
  static point2d_t e = {10, 100, 0, 1, 0, 1.0f, 0};
  gl4dpClearScreen();
  /*int y;
  for(y = 0; y < 256; ++y) {
    gl4dpSetColor(RGB(0, 255, 255 - y));
    gl4dpPutPixel(0, y);
    a.x = 0; a.y = y;
    a.r = 0.0f; a.g = 1.0f; a.b = (255.0f - y) / 255.0f;
    gl4dpSetColor(RGB(255, 255 - y, 0));
    gl4dpPutPixel(_w - 1, y);
    b.x = _w - 1; b.y = y;
    b.r = 1.0f; b.g = (255.0f - y) / 255.0f; b.b = 0.0f; 
    scanline(&a, &b);
    }*/
  triangle(&c, &d, &e);
  c.x += (rand()%7) - 3;
  c.y += (rand()%7) - 3;
  d.x += (rand()%7) - 3;
  d.y += (rand()%7) - 3;
  e.x += (rand()%7) - 3;
  e.y += (rand()%7) - 3;
  gl4dpUpdateScreen(NULL);

}

/*!\brief créé la fenêtre, un screen 2D effacé en noir et lance une
 *  boucle infinie.*/
int main(int argc, char ** argv) {
  SDL_Surface * s;
  /* tentative de création d'une fenêtre pour GL4Dummies */
  if(!gl4duwCreateWindow(argc, argv, /* args du programme */
			 "GL4Dummies' Hello World", /* titre */
			 10, 10, 1024, 768, /* x,y, largeur, heuteur */
			 GL4DW_SHOWN) /* état visible */) {
    /* ici si échec de la création souvent lié à un problème d'absence
     * de contexte graphique ou d'impossibilité d'ouverture d'un
     * contexte OpenGL (au moins 3.2) */
    return 1;
  }
  s = SDL_LoadBMP("img.bmp");
  assert(s);
  /* création d'un screen GL4Dummies (texture dans laquelle nous
   * pouvons dessiner) aux dimensions de la fenêtre */
  ecran0 = gl4dpInitScreenWithDimensions(_w, 256);
  ecran1 = gl4dpInitScreenWithDimensions(s->w, s->h);
  gl4dpCopyFromSDLSurface(s);
  gl4dpSetScreen(ecran0);
  /* effacement du screen en cours en utilisant la couleur par défaut,
   * le noir */
  //gl4dpClearScreen();

  gl4duwDisplayFunc(dessin);
  /* boucle infinie pour éviter que le programme ne s'arrête et ferme
   * la fenêtre */
  gl4duwMainLoop();
  return 0;
}
