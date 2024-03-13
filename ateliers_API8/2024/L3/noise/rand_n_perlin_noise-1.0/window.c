/*!\file window.c
 * \brief bruit 1D et 2D, bruit de perlin et leur visualisation
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date March 3 2017
 */
#include "rand.h"
#include "ImprovedNoise.h"
#include <GL4D/gl4dp.h>
#include <GL4D/gl4duw_SDL2.h>
#define NO 240
#define W 1024
#define H 480
static int _o1d[NO], _o2d[H * H / 4], _ntirages = 10, _pause = 1;
static double _zoom = 1.0, _temps = 0.01;

static void draw(void) {
  int i, j, c, max, pas = W / NO;
  /* effacer l'écran */
  gl4dpClearScreen();
  /* DESSINER EN 1D LES OCCURENCES D'UNE SUITE DE BRUIT (donc 1D) sur NO individus */
  /* couleur de dessin : blanc */
  gl4dpSetColor(RGB(255, 255, 255));
  max = test1(_o1d, NO, NO * _ntirages);
  for(i = 0; i < NO - 1; i++) {
    gl4dpLine(pas * i,       H / 2 + H * (_o1d[i] / (max + 1.0)) / 2, 
	      pas * (i + 1), H / 2 + H * (_o1d[i + 1] / (max + 1.0)) / 2);
  }

  /* DESSINER LE BRUIT PARAMETRIQUE 1D sur NO individus */
  /* couleur de dessin : rouge */
  gl4dpSetColor(RGB(255, 0, 0));
  max = test2(_o1d, NO);
  for(i = 0; i < NO - 1; i++) {
    gl4dpLine(pas * i,       H / 2 + H * (_o1d[i] / (max + 1.0)) / 2, 
	      pas * (i + 1), H / 2 + H * (_o1d[i + 1] / (max + 1.0)) / 2);
  }

  /* DESSINER EN 2D LES OCCURENCES D'UNE SUITE DE BRUIT (donc 1D) sur (H/2)*(H/2) individus */
  max = test3(_o2d, H / 2, H * H * _ntirages / 4);
  for(i = 0; i < H / 2; i++) {
    for(j = 0; j < H / 2; j++) {
      c = 256 * (_o2d[i * (H / 2) + j] / (max + 1.0));
      gl4dpSetColor(RGB(c, c, c));
      gl4dpPutPixel(i, j);
    }
  }

  /* DESSINER EN 2D LE BRUIT PARAMETRIQUE 2D sur (H/2)*(H/2) individus */
  max = test4(_o2d, H / 2);
  for(i = 0; i < H / 2; i++) {
    for(j = 0; j < H / 2; j++) {
      c = 256 * (_o2d[i * (H / 2) + j] / (max + 1.0));
      gl4dpSetColor(RGB(c, c, c));
      gl4dpPutPixel(i + H / 2, j);
    }
  }

  /* DESSINER EN 2D LE BRUIT de Perlin */
  for(i = 0; i < H / 2; i++) {
    for(j = 0; j < H / 2; j++) {
      c = 255 * noise(j / _zoom, i / _zoom, _temps);
      gl4dpSetColor(RGB(c, c, c));
      gl4dpPutPixel(i + H, j);
    }
  }
  if(!_pause)
    _temps += 0.01;
  gl4dpUpdateScreen(NULL);
}

static void key(int keycode) {
  switch(keycode) {
  case ' ':
    _pause = !_pause;
    break;
  case SDLK_LEFT:
    _zoom -= 0.05;
    break;
  case SDLK_RIGHT:
    _zoom += 0.05;
    break;
  case SDLK_UP:
    if(_ntirages < 1000000)
      _ntirages *= 10;
    break;
  case SDLK_DOWN:
    if(_ntirages > 1)
      _ntirages /= 10;
    break;
  default:
    break;
  }
}
/*!\brief appelée au moment de sortir du programme (atexit), elle
 *  libère les éléments utilisés.*/
static void quit(void) {
  /* nettoyage des éléments utilisés par la bibliothèque GL4Dummies */
  gl4duClean(GL4DU_ALL);
}
/*!\brief créé la fenêtre, un screen 2D, place la fonction display et
 * appelle la boucle principale.*/
int main(int argc, char ** argv) {
  /* fenêtre positionnée en (10, 10), ayant une dimension de (512, 512) et un titre "GL4D-Primitives" */
  if(!gl4duwCreateWindow(argc, argv, "GL4D-Primitives", 10, 10, W, H, SDL_WINDOW_SHOWN))
    return 1;
  gl4dpInitScreen();
  /* couleur de dessin : blanc */
  gl4dpSetColor(RGB(255, 255, 255));
  atexit(quit);
  gl4duwDisplayFunc(draw);
  gl4duwKeyDownFunc(key);
  gl4duwMainLoop();
  return 0;
}
