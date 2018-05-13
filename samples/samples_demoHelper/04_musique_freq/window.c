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

/* Prototypes des fonctions statiques contenues dans ce fichier C */
static void init(const char * filename);
static void draw(void);
static void initAudio(const char * filename);
static void quit(void);

/*!\brief nombre d'échantillons du signal sonore */
#define ECHANTILLONS 1024
/*!\brief amplitude des échantillons du signal sonore */
static Sint16 _hauteurs[ECHANTILLONS];
/*!\brief dimensions de la fenêtre */
static int _wW = ECHANTILLONS, _wH = 256;
/*!\brief id du screen à créer */
static GLuint _screen = 0;


/*!\brief pointeur vers la musique chargée par SDL_Mixer */
static Mix_Music * _mmusic = NULL;
/*!\brief données entrées/sorties pour la lib fftw */
static fftw_complex * _in4fftw = NULL, * _out4fftw = NULL;
/*!\brief donnée à précalculée utile à la lib fftw */
static fftw_plan _plan4fftw = NULL;

/*!\brief La fonction principale créé la fenêtre d'affichage,
 * initialise GL et les données, affecte les fonctions d'événements et
 * lance la boucle principale d'affichage.*/
int main(int argc, char ** argv) {
  if(argc != 2) {
    fprintf(stderr, "usage: %s <audio_file>\n", argv[0]);
    return 2;
  }
  if(!gl4duwCreateWindow(argc, argv, "GL4Dummies", 10, 10, 
			 _wW, _wH, GL4DW_SHOWN))
    return 1;
  init(argv[1]);
  atexit(quit);
  gl4duwDisplayFunc(draw);
  gl4duwMainLoop();
  return 0;
}

static void init(const char * filename) {
  /* préparation des conteneurs de données pour la lib FFTW */
  _in4fftw   = fftw_malloc(ECHANTILLONS *  sizeof *_in4fftw);
  memset(_in4fftw, 0, ECHANTILLONS *  sizeof *_in4fftw);
  assert(_in4fftw);
  _out4fftw  = fftw_malloc(ECHANTILLONS * sizeof *_out4fftw);
  assert(_out4fftw);
  _plan4fftw = fftw_plan_dft_1d(ECHANTILLONS, _in4fftw, _out4fftw, FFTW_FORWARD, FFTW_ESTIMATE);
  assert(_plan4fftw);
  /* préparation GL */
  glViewport(0, 0, _wW, _wH);
  _screen = gl4dpInitScreen();
  /* chargement de l'audio */
  initAudio(filename);
}

/*!\brief dessine dans le contexte OpenGL actif */
static void draw(void) {
  int i;
  gl4dpSetColor(RGB(255, 255, 255));
  gl4dpSetScreen(_screen);
  gl4dpClearScreen();
  for(i = 0; i < ECHANTILLONS; ++i) {
    int x0, y0;
    x0 = (i * (_wW - 1)) / (ECHANTILLONS - 1);
    y0 = _hauteurs[i];
    gl4dpPutPixel(x0, y0);
  }
  gl4dpUpdateScreen(NULL);
}


/*!\brief appelée quand l'audio est joué et met dans \a stream les
 * données audio de longueur \a len */
static void mixCallback(void *udata, Uint8 *stream, int len) {
  if(_plan4fftw) {
    int i, j, l = MIN(len >> 1, ECHANTILLONS);
    Sint16 *d = (Sint16 *)stream;
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

/*!\brief charge le fichier audio avec les bonnes options */
static void initAudio(const char * filename) {
#if defined(__APPLE__)
  int mult = 1;
#else
  int mult = 2;
#endif
  int mixFlags = MIX_INIT_MP3, res;
  res = Mix_Init(mixFlags);
  if( (res & mixFlags) != mixFlags ) {
    fprintf(stderr, "Mix_Init: Erreur lors de l'initialisation de la bibliothèque SDL_Mixer\n");
    fprintf(stderr, "Mix_Init: %s\n", Mix_GetError());
    //exit(3); commenté car ne réagit correctement sur toutes les architectures
  }
  if(Mix_OpenAudio(44100, AUDIO_S16LSB, 1, mult * ECHANTILLONS) < 0)
    exit(4);  
  if(!(_mmusic = Mix_LoadMUS(filename))) {
    fprintf(stderr, "Erreur lors du Mix_LoadMUS: %s\n", Mix_GetError());
    exit(5);
  }
  Mix_SetPostMix(mixCallback, NULL);
  if(!Mix_PlayingMusic())
    Mix_PlayMusic(_mmusic, 1);
}

/*!\brief Cette fonction est appelée au moment de sortir du programme
 *  (atexit), elle libère les données audio, la fenêtre SDL \ref _win
 *  et le contexte OpenGL \ref _oglContext.
 */
static void quit(void) {
  if(_mmusic) {
    if(Mix_PlayingMusic())
      Mix_HaltMusic();
    Mix_FreeMusic(_mmusic);
    _mmusic = NULL;
  }
  Mix_CloseAudio();
  Mix_Quit();
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
  gl4duClean(GL4DU_ALL);
}
