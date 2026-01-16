/*!\file window.c
 *
 * \brief Utilisation jack et fftw pour l'affichage des
 * fréquences du son.
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \author ObaniGemini/ObaniGarage, obani@pi-et-ro.net
 * \date June 01 2023
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4dp.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL_mixer.h>
#include <jack/jack.h>
#include <fftw3.h>

/* Prototypes des fonctions statiques contenues dans ce fichier C */
static void keydown(int keycode);
static void init(const char * clientname);
static void draw(void);
static int initJack(const char * clientname);
static void quit(void);

/*!\brief nombre d'échantillons du signal sonore
 * plus ce nombre est grand, plus la précision de la transformée de fourier sera grande (surtout dans les graves), 
 */
#define ECHANTILLONS 4096
/*!\brief nombre de bandes de fréquences stockées */
#define FREQUENCES (ECHANTILLONS >> 2)
/*!\brief amplitude des échantillons du signal sonore */
static float _samples[ECHANTILLONS];
/*!\brief amplitude des fréquences du signal sonore */
static Sint16 _hauteurs[FREQUENCES];
/*!\brief dimensions de la fenêtre */
static int _wW = 1280, _wH = 512;
/*!\brief id du screen à créer */
static GLuint _screen = 0;
/*!\brief type d'affichage */
static int waveform = 0;


/*!\brief données entrées/sorties pour la lib fftw */
static fftw_complex * _in4fftw = NULL, * _out4fftw = NULL;
/*!\brief donnée à précalculée utile à la lib fftw */
static fftw_plan _plan4fftw = NULL;

/*!\brief pointeur vers le client jack */
static jack_client_t * _client = NULL;
/*!\brief pointeur vers le port d'entrée jack */
static jack_port_t * _input = NULL;

/*!\brief La fonction principale créé la fenêtre d'affichage,
 * initialise GL et les données, affecte les fonctions d'événements et
 * lance la boucle principale d'affichage.*/
int main(int argc, char ** argv) {
  if(argc != 2) {
    fprintf(stderr, "usage: %s <client_name>\n", argv[0]);
    return 2;
  }
  if(!gl4duwCreateWindow(argc, argv, "GL4Dummies", 10, 10, 
			 _wW, _wH, GL4DW_SHOWN))
    return 1;
  init(argv[1]);
  atexit(quit);
  gl4duwDisplayFunc(draw);
  gl4duwKeyDownFunc(keydown);
  gl4duwMainLoop();
  return 0;
}

static void init(const char * clientname) {
  /* préparation des conteneurs de données pour la lib FFTW */
  _in4fftw = fftw_malloc(ECHANTILLONS *  sizeof *_in4fftw);
  memset(_in4fftw, 0, ECHANTILLONS *  sizeof *_in4fftw);
  assert(_in4fftw);
  _out4fftw  = fftw_malloc(ECHANTILLONS * sizeof *_out4fftw);
  assert(_out4fftw);
  _plan4fftw = fftw_plan_dft_1d(ECHANTILLONS, _in4fftw, _out4fftw, FFTW_FORWARD, FFTW_ESTIMATE);
  assert(_plan4fftw);
  /* préparation GL */
  glViewport(0, 0, _wW, _wH);
  _screen = gl4dpInitScreen();
  /* chargement de jack audio */
  if(initJack(clientname)) {
    printf("An error occured\n");
    exit(1);
  }
}


/*!\brief dessine la forme d'onde dans le contexte OpenGL actif */
static void drawwave(void) {
  for(int i = 0; i < ECHANTILLONS; ++i) {
    int x0 = (i * (_wW - 1)) / (ECHANTILLONS - 1);
    int y0 = (_samples[i] * _wH + _wH) / 2;

    if(y0 > _wH) y0 = _wH - 1;
    if(y0 < 0) y0 = 0;
      
    gl4dpPutPixel(x0, y0);
  }
}

/*!\brief dessine les bandes de fréquences dans le contexte OpenGL actif */
static void drawfftw(void) {
  for(int i = 0; i < FREQUENCES; ++i) {
    int x0 = (i * (_wW - 1)) / (FREQUENCES - 1);
    int y0 = _hauteurs[i];

    if(y0 > _wH) y0 = _wH - 1;
    if(y0 < 0) y0 = 0;
      
    gl4dpPutPixel(x0, y0);
  }
}

/*!\brief dessine dans le contexte OpenGL actif */
static void draw(void) {
  gl4dpSetColor(RGB(255, 255, 255));
  gl4dpSetScreen(_screen);
  gl4dpClearScreen();

  if (waveform == 1) {
    drawwave();
  } else {
    drawfftw();
  }
  gl4dpUpdateScreen(NULL);
}

/*!\brief fonction appellée par GL4Dummies' quand un événement
 * clavier est détecté
 * permet de changer le type de visualisation du signal
 * waveform = 0 -> visualisation des amplitudes de fréquences
 * waveform = 1 -> visualisation du signal audio
 */
static void keydown(int keycode) {
  switch(keycode) {
  case GL4DK_LEFT:
    waveform = 0;
    break;
  case GL4DK_RIGHT:
    waveform = 1;
    break;
  default:
    break;
  }
}


/*!\brief remplit un tableau '_samples' de taille fixe */
static void fillwave(float * buffer, int nframes) {
  if(nframes > ECHANTILLONS) {
    for(int i = 0; i < ECHANTILLONS; ++i)
      _samples[i] = buffer[i];
  } else {
    // si 'buffer' a une taille inférieur à notre tableau _samples de taille fixe,
    // on bouge les données précédentes pour faire avancer le signal de 'nframes' échantillons
    for(int i = nframes; i < ECHANTILLONS; ++i)
      _samples[i - nframes] = _samples[i];

    int end = ECHANTILLONS - nframes;
    for(int i = 0; i < nframes; ++i)
      _samples[i + end] = buffer[i];
  }
}

/*!\brief calcule la transformée de fourier sur le tableau '_samples' */
static void processfftw(void) {
  if(_plan4fftw) {
    int i;
    for(i = 0; i < ECHANTILLONS; i++)
      _in4fftw[i][0] = _samples[i];

    fftw_execute(_plan4fftw);

    for(i = 0; i < FREQUENCES; i++)
      _hauteurs[i] = (int)(sqrt(_out4fftw[i][0] * _out4fftw[i][0] + _out4fftw[i][1] * _out4fftw[i][1]) * exp(2.0 * i / (double)(FREQUENCES)));
  }
}

/*!\brief fonction appellée par jack liée au client jack
 * les données d'entrée du port \ref _input sont récupérées dans
 * \a buffer. Ce buffer a une aille \a nframes
 */
static int mixCallback(jack_nframes_t nframes, void * arg) {
  float * buffer = jack_port_get_buffer((jack_port_t*)arg, nframes);

  fillwave(buffer, nframes);
  processfftw();

  return 0;
}

/*!\brief créée le client jack \ref _client avec un seul port d'entrée \ref _input
 * et lui assigne la fonction process \a mixCallback qui sera appellée régulièrement
 */
static int initJack(const char * clientname) {
  jack_status_t status;
  _client = jack_client_open(clientname, JackServerName, &status, "default");
  
  if(status == JackFailure) {
    printf("jack server not running?\n");
    return -1;
  }

  if(_client == NULL) {
    printf("jack client is NULL\n");
    return -1;
  }

  printf("Created client %s\n", clientname);

  const char * port_name = "input";
  const char * type_name = JACK_DEFAULT_AUDIO_TYPE;
  _input = jack_port_register(_client, port_name, type_name, JackPortIsInput, 0);
  if(_input == NULL) {
    printf("Couldn't create port %s with type %s\n", port_name, type_name);
    return -1;
  }

  return jack_set_process_callback(_client, mixCallback, _input) || jack_activate(_client);
}

/*!\brief Cette fonction est appelée au moment de sortir du programme
 *  (atexit), elle ferme le client jack, libère les données audio, 
 * la fenêtre SDL \ref _win et le contexte OpenGL \ref _oglContext.
 */
static void quit(void) {
  if(_client) {
    printf("Closing client %s\n", jack_get_client_name(_client));
    jack_client_close(_client);
    _client = NULL;
  }

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
