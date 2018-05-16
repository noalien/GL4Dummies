/*!\file window.c
 * \brief affichage du spectre sonore
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date April 15 2016 */
#include <stdio.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4dp.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL_mixer.h>
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

/*!\brief initialise les paramètres OpenGL */
static void init(const char * filename) {
  glViewport(0, 0, _wW, _wH);
  _screen = gl4dpInitScreen();
  initAudio(filename);
}

/*!\brief dessine dans le contexte OpenGL actif. Vous pouvez
 * décommenter pour dessiner avec des lignes. */
static void draw(void) {
  int i;
  gl4dpSetColor(RGB(255, 255, 255));
  gl4dpSetScreen(_screen);
  gl4dpClearScreen();
  for(i = 0; i < ECHANTILLONS/*  - 1 */; i+=2) {
    int x0, y0;//, x1, y1;
    x0 = (i * (_wW - 1)) / (ECHANTILLONS - 1);
    y0 = _hauteurs[i];
    //x1 = ((i + 1) * (_wW - 1)) / (ECHANTILLONS - 1);
    //y1 = _hauteurs[i + 1];
    //gl4dpSetColor(rand());
    gl4dpPutPixel(x0, y0);
    //gl4dpLine(x0, y0, x1, y1);
  }
  gl4dpSetColor(RGB(255, 0, 0));
  for(i = 1; i < ECHANTILLONS/*  - 1 */; i+=2) {
    int x0, y0;//, x1, y1;
    x0 = (i * (_wW - 1)) / (ECHANTILLONS - 1);
    y0 = _hauteurs[i];
    //x1 = ((i + 1) * (_wW - 1)) / (ECHANTILLONS - 1);
    //y1 = _hauteurs[i + 1];
    //gl4dpSetColor(rand());
    gl4dpPutPixel(x0, y0);
    //gl4dpLine(x0, y0, x1, y1);
  }
  gl4dpUpdateScreen(NULL);
}

/*!\brief Cette fonction est appelée quand l'audio est joué et met 
 * dans \a stream les données audio de longueur \a len.
 * \param udata pour user data, données passées par l'utilisateur, ici NULL.
 * \param stream flux de données audio.
 * \param len longueur de \a stream. */
static void mixCallback(void *udata, Uint8 *stream, int len) {
  int i;
  Sint16 *s = (Sint16 *)stream;
  if(len >= 2 * ECHANTILLONS)
    for(i = 0; i < ECHANTILLONS; i++)
      _hauteurs[i] = _wH / 2 + (_wH / 2) * s[i] / ((1 << 15) - 1.0);
  return;
}

/*!\brief Cette fonction initialise les paramètres SDL_Mixer et charge
 *  le fichier audio.*/
static void initAudio(const char * filename) {
  int mixFlags = MIX_INIT_OGG | MIX_INIT_MP3 | MIX_INIT_MOD, res;
  res = Mix_Init(mixFlags);
  if( (res & mixFlags) != mixFlags ) {
    fprintf(stderr, "Mix_Init: Erreur lors de l'initialisation de la bibliotheque SDL_Mixer\n");
    fprintf(stderr, "Mix_Init: %s\n", Mix_GetError());
    //exit(3); commenté car ne réagit correctement sur toutes les architectures
  }
  if(Mix_OpenAudio(44100, AUDIO_S16LSB, 2, 1024) < 0)
    exit(4);
  if(!(_mmusic = Mix_LoadMUS(filename))) {
    fprintf(stderr, "Erreur lors du Mix_LoadMUS: %s\n", Mix_GetError());
    exit(5);
  }
  Mix_SetPostMix(mixCallback, NULL);
  if(!Mix_PlayingMusic())
    Mix_PlayMusic(_mmusic, 1);
}

/*!\brief appelée au moment de sortir du programme (atexit), libère les éléments utilisés */
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
  gl4duClean(GL4DU_ALL);
}
