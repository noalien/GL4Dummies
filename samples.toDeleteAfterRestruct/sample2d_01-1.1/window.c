/*!\file window.c
 *
 * \brief début de code réalisé en cours ayant pour but de construire
 * un diagramme de voronoi. Exercice à finir pour le 5 mars 2018.
 *
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date February 21 2018
 */

/* inclusion des entêtes de fonctions de gestion de primitives simples
 * de dessin. La lettre p signifie aussi bien primitive que
 * pédagogique. */
#include <GL4D/gl4dp.h>

/* inclusion des entêtes de fonctions de création et de gestion de
 * fenêtres système ouvrant un contexte favorable à GL4dummies. Cette
 * partie est dépendante de la bibliothèque SDL2 */
#include <GL4D/gl4duw_SDL2.h>
#include <assert.h>

static int _nbCells = 20000;
static GLuint _s1 = 0, _s2 = 0;


typedef struct site_t site_t;

struct site_t {
  int x, y;
  Uint32 color;
  int alive;
};

static site_t * diag = NULL;
static int nbCells, radius;

int circleNOM(int x0, int y0, int r) {
  int x, y, del, incH, incO, t = M_SQRT1_2 * r + 1;
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
      if(IN_SCREEN(x0 + x, y0 + y)) gl4dpPutPixel(x0 + x, y0 + y);
      if(IN_SCREEN(x0 + x, y0 - y)) gl4dpPutPixel(x0 + x, y0 - y);
      if(IN_SCREEN(x0 - x, y0 + y)) gl4dpPutPixel(x0 - x, y0 + y);
      if(IN_SCREEN(x0 - x, y0 - y)) gl4dpPutPixel(x0 - x, y0 - y);
      if(IN_SCREEN(x0 + y, y0 + x)) gl4dpPutPixel(x0 + y, y0 + x);
      if(IN_SCREEN(x0 + y, y0 - x)) gl4dpPutPixel(x0 + y, y0 - x);
      if(IN_SCREEN(x0 - y, y0 + x)) gl4dpPutPixel(x0 - y, y0 + x);
      if(IN_SCREEN(x0 - y, y0 - x)) gl4dpPutPixel(x0 - y, y0 - x);
      incO += 4;
      del += incO;
    }
  }
  return 1;
}

static void diagFree(void) {
  if(diag) {
    free(diag);
    diag = NULL;
  }  
}

static void diagInit(int n) {
  int x, y, i, w, h;
  //Uint8 r, g, b;
  nbCells = n;
  if(diag)
    diagFree();
  else
    atexit(diagFree);
  radius = 1;
  diag = malloc(nbCells * sizeof *diag);
  assert(diag);
  gl4dpSetScreen(_s1);
  gl4dpClearScreen();
  w = gl4dpGetWidth();
  h = gl4dpGetHeight();
  for(i = 0; i < nbCells; i++) {
    gl4dpSetScreen(_s1);
    do {
      x = (int)(gl4dmURand() * w);
      y = (int)(gl4dmURand() * h);
    } while(gl4dpGetPixel(x, y));
    //r = gl4dmURand() * 256;
    //g = gl4dmURand() * 256;
    //b = gl4dmURand() * 256;
    diag[i].x = x;
    diag[i].y = y;
    gl4dpSetScreen(_s2);
    diag[i].color = gl4dpGetPixel(x, h - y - 1);//RGB(r,g,b);
    diag[i].alive = 1;
    gl4dpSetScreen(_s1);
    gl4dpSetColor(diag[i].color);
    gl4dpPutPixel(x, y);
  }
  gl4dpSetScreen(_s1);
}

static int diagGrow(void) {
  /* fait grossir toutes les cellules de 1, et retourne une valeur qui
   * dit si il y en a au moins une qui a grandi */
  int i, fini = 1;
  for(i = 0; i < nbCells; i++) {
    if(!diag[i].alive) continue;
    gl4dpSetColor(diag[i].color);
    if(circleNOM(diag[i].x, diag[i].y, radius))
      fini = 0;
    else
      diag[i].alive = 0;
  }
  //radius++;
  return !fini;
}


/*!\brief appelée au moment de sortir du programme (atexit), elle
 *  libère les éléments utilisés par GL4Dummies.*/
static void quitte(void) {
  gl4duClean(GL4DU_ALL);
}

/*!\brief fonction appelée à chaque draw par la gl4duwMainLoop. */
static void dessin(void) {
  if(!diagGrow()) {
    gl4dpUpdateScreen(NULL);
    diagInit(_nbCells);
  }
  gl4dpUpdateScreen(NULL);
}

/*!\brief créé la fenêtre, un screen 2D effacé en noir et lance une
 *  boucle infinie.*/
int main(int argc, char ** argv) {
  SDL_Surface * s;
  /* tentative de création d'une fenêtre pour GL4Dummies */
  if(!gl4duwCreateWindow(argc, argv, /* args du programme */
			 "GL4Dummies' Hello Pixels", /* titre */
			 10, 10, 800, 600, /* x,y, largeur, heuteur */
			 GL4DW_SHOWN) /* état visible */) {
    /* ici si échec de la création souvent lié à un problème d'absence
     * de contexte graphique ou d'impossibilité d'ouverture d'un
     * contexte OpenGL (au moins 3.2) */
    return 1;
  }
  /* ajoute la fonction quitte à la pile des choses à faire en sortant
   * du programme */
  atexit(quitte);
  /* chargement d'une image avec SDL2 */
  s = SDL_LoadBMP("images/img.bmp");
  if(!s)
    exit(1);
  /* création d'un screen GL4Dummies (texture dans laquelle nous
   * pouvons dessiner) aux dimensions de l'image SDL */
  _s1 = gl4dpInitScreenWithDimensions(s->w, s->h);
  /* création d'un screen GL4Dummies (texture dans laquelle nous
   * pouvons dessiner) aux dimensions de l'image SDL */
  _s2 = gl4dpInitScreenWithDimensions(s->w, s->h);
  gl4dpCopyFromSDLSurface(s);
  SDL_FreeSurface(s);
  /* remettre _s1 comme écran courant */
  gl4dpSetScreen(_s1);
  /* met en place une fonction de display au sein de la boucle
   * event-simu-draw gl4duwMainLoop */
  gl4duwDisplayFunc(dessin);
  diagInit(_nbCells);
  /* boucle infinie pour éviter que le programme ne s'arrête et ferme
   * la fenêtre */
  gl4duwMainLoop();
  return 0;
}
