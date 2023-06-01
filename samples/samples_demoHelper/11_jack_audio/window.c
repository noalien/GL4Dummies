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
static void init(const char * clientname);
static void draw(void);
static int initJack(const char * clientname);
static void quit(void);

/*!\brief nombre d'échantillons du signal sonore */
#define ECHANTILLONS 1024
/*!\brief amplitude des échantillons du signal sonore */
static Sint16 _hauteurs[ECHANTILLONS];
/*!\brief dimensions de la fenêtre */
static int _wW = ECHANTILLONS, _wH = 256;
/*!\brief id du screen à créer */
static GLuint _screen = 0;


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
  /* chargement de l'audio */
  if(initJack(clientname)) {
    printf("An error occured\n");
    exit(1);
  }
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


/*!\brief fonction appellée par jack liée au client jack
 * les données d'entrée du port \ref _input sont récupérées dans
 * \a buffer. Ce buffer a une aille \a nframes
 */
static int mixCallback(jack_nframes_t nframes, void * arg) {
  if(_plan4fftw) {
    jack_default_audio_sample_t * buffer = jack_port_get_buffer((jack_port_t*)arg, nframes);

    int i, j, l = MIN(nframes, ECHANTILLONS);
    for(i = 0; i < l; i++)
      _in4fftw[i][0] = buffer[i];
    fftw_execute(_plan4fftw);
    for(i = 0; i < l >> 2; i++) {
      _hauteurs[4 * i] = (int)(sqrt(_out4fftw[i][0] * _out4fftw[i][0] + _out4fftw[i][1] * _out4fftw[i][1]) * exp(2.0 * i / (double)(l / 4.0)));
      for(j = 1; j < 4; j++)
	_hauteurs[4 * i + j] = MIN(_hauteurs[4 * i], 255);
    }
  }

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
