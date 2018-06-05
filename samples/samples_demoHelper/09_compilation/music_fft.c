/*!\file window.c
 *
 * \brief Utilisation SDL2_Mixer et fftw pour l'affichage des
 * fréquences du son.
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date May 13 2017
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4dp.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL_mixer.h>
#include <fftw3.h>
#include <GL4D/gl4dh.h>
#include "audioHelper.h"

/* Prototypes des fonctions statiques contenues dans ce fichier C */
static void init(void);
static void audio(void);
static void draw(void);
static void quit(void);

/*!\brief nombre d'échantillons du signal sonore */
#define ECHANTILLONS 1024
/*!\brief amplitude des échantillons du signal sonore */
static Sint16 _hauteurs[ECHANTILLONS];
/*!\brief id du screen à créer */
static GLuint _screen = 0;


/*!\brief données entrées/sorties pour la lib fftw */
static fftw_complex * _in4fftw = NULL, * _out4fftw = NULL;
/*!\brief donnée à précalculée utile à la lib fftw */
static fftw_plan _plan4fftw = NULL;

static void init(void) {
  /* préparation des conteneurs de données pour la lib FFTW */
  _in4fftw   = fftw_malloc(ECHANTILLONS *  sizeof *_in4fftw);
  memset(_in4fftw, 0, ECHANTILLONS *  sizeof *_in4fftw);
  assert(_in4fftw);
  _out4fftw  = fftw_malloc(ECHANTILLONS * sizeof *_out4fftw);
  assert(_out4fftw);
  _plan4fftw = fftw_plan_dft_1d(ECHANTILLONS, _in4fftw, _out4fftw, FFTW_FORWARD, FFTW_ESTIMATE);
  assert(_plan4fftw);
  /* préparation GL */
  _screen = gl4dpInitScreen();
}

/*!\brief dessine dans le contexte OpenGL actif */
static void draw(void) {
  int i;
  gl4dpSetColor(RGB(255, 255, 255));
  gl4dpSetScreen(_screen);
  gl4dpClearScreenWith(RGB(255, 0, 0));
  for(i = 0; i < ECHANTILLONS; ++i) {
    int x0, y0;
    x0 = (i * (gl4dpGetWidth() - 1)) / (ECHANTILLONS - 1);
    y0 = _hauteurs[i];
    gl4dpPutPixel(x0, y0);
  }
  gl4dpUpdateScreen(NULL);
}


/*!\brief appelée quand l'audio est joué et met dans \a stream les
 * données audio de longueur \a len */
static void audio(void) {
  if(_plan4fftw) {
    int i, j, l = MIN(ahGetAudioStreamLength() >> 1, ECHANTILLONS);
    Sint16 *d = (Sint16 *)ahGetAudioStream();
    for(i = 0; i < l; i++)
      _in4fftw[i][0] = d[i] / ((1 << 15) - 1.0);
    fftw_execute(_plan4fftw);
    for(i = 0; i < l >> 2; i++) {
      _hauteurs[4 * i] = (int)(sqrt(_out4fftw[i][0] * _out4fftw[i][0] + _out4fftw[i][1] * _out4fftw[i][1]) * exp(2.0 * i / (double)(l / 4.0)));
      for(j = 1; j < 4; j++)
	_hauteurs[4 * i + j] = MIN(_hauteurs[4 * i], 255);
    }
  }
}

/*!\brief Cette fonction est appelée au moment de sortir du programme
 *  (atexit), elle libère les données audio, la fenêtre SDL \ref _win
 *  et le contexte OpenGL \ref _oglContext.
 */
static void quit(void) {
  if(_screen) {
    gl4dpSetScreen(_screen);
    gl4dpDeleteScreen();
    _screen = 0;
  }
  if(_plan4fftw) {
    fftw_destroy_plan(_plan4fftw);
    _plan4fftw = NULL;
  }
  if(_in4fftw) {
    fftw_free(_in4fftw); 
    _in4fftw = NULL;
  }
  if(_out4fftw) {
    fftw_free(_out4fftw); 
    _out4fftw = NULL;
  }
}

void music_fft(int state) {
  switch(state) {
  case GL4DH_INIT:
    init();
    return;
  case GL4DH_FREE:
    quit();
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    audio();
    return;
  default: /* GL4DH_DRAW */
    draw();
    return;
  }
}
