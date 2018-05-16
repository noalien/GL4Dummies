/*!\file window.c
 *
 * \brief Lumière positionnelle + Phong + Bump mapping + Normal
 * mapping + textures et geometry shader
 *
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date March 19 2018
 */
#include <assert.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <fftw3.h>

/* Prototypes des fonctions statiques contenues dans ce fichier C */
static void         init(void);
static void         resize(int w, int h);
static void         draw(void);
static void         initAudio(const char * filename);
static void         quit(void);

/*!\brief dimensions de la fenêtre */
static int _wW = 800, _wH = 600;
/*!\brief identifiant des GLSL program */
static GLuint _pId1 = 0, _pId2 = 0;
/*!\brief identifiant de la sphere */
static GLuint _sphere = 0;
/*!\brief nombre de longitudes et latitudes de la sphere */
static GLuint _longitudes = 200, _latitudes = 200;
/*!\brief position de la lumière relativement à la sphère éclairée */
static GLfloat _lumPos0[4] = {-15.1, 20.0, 20.7, 1.0};
/*!\brief nombre d'échantillons du signal sonore */
#define ECHANTILLONS 1024
/*!\brief amplitude des basses et aigus du signal sonore */
static GLfloat _basses = 0, _aigus = 0;

/*!\brief noms des fichiers textures à charger */
static const char * _texture_filenames[] = { "images/land_ocean_ice_2048.png", 
					     "images/land_ocean_ice_lights_2048.png", 
					     "images/land_bump.png", 
					     "images/land_cloud_combined_2048.png", 
					     "images/land_ocean_ice_2048_glossmap.png" };

/*!\brief noms des variables uniform sampler2D à affecter à charque
 * texture */
static const char * _sampler_names[] = { "eday", 
					 "enight", 
					 "ebump", 
					 "ecloud", 
					 "egloss" };
/*!\brief enum pour chaque texture */
enum texture_e {
  TE_EDAY = 0, /* land_ocean_ice_2048.png */
  TE_ENIGHT, /* land_ocean_ice_lights_2048.png */
  TE_EBUMP, /* land_bump.png */
  TE_ECLOUD, /* land_cloud_combined_2048.png */
  TE_EGLOSS, /* land_ocean_ice_2048_glossmap.png */
  TE_END
};
/*!\brief tableau des identifiants de texture à charger */
static GLuint _tId[TE_END] = {0};

/*!\brief pointeur vers la musique chargée par SDL_Mixer */
static Mix_Music * _mmusic = NULL;
/*!\brief données entrées/sorties pour la lib fftw */
static fftw_complex * _in4fftw = NULL, * _out4fftw = NULL;
/*!\brief donnée à précalculée utile à la lib fftw */
static fftw_plan _plan4fftw = NULL;

/*!\brief La fonction principale créé la fenêtre d'affichage,
 * initialise GL et les données, affecte les fonctions d'événements et
 * lance la boucle principale d'affichage.
 */
int main(int argc, char ** argv) {
  if(argc != 2) {
    fprintf(stderr, "usage: %s <audio_file>\n", argv[0]);
    return 2;
  }
  if(!gl4duwCreateWindow(argc, argv, "GL4Dummies", 10, 10, 
			 _wW, _wH, GL4DW_RESIZABLE | GL4DW_SHOWN))
    return 1;
  init();
  initAudio(argv[1]);
  atexit(quit);
  gl4duwResizeFunc(resize);
  gl4duwDisplayFunc(draw);
  gl4duwMainLoop();
  return 0;
}

/*!\brief initialise les paramètres OpenGL */
static void init(void) {
  int i;
  if(!_tId[0]) {
    glGenTextures(TE_END, _tId);
    for(i = 0; i < TE_END; i++) {
      SDL_Surface * t;
      glBindTexture(GL_TEXTURE_2D, _tId[i]);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      if( (t = IMG_Load(_texture_filenames[i])) != NULL ) {
#ifdef __APPLE__
	int mode = t->format->BytesPerPixel == 4 ? GL_BGRA : GL_BGR;
#else
	int mode = t->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;
#endif       
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t->w, t->h, 0, mode, GL_UNSIGNED_BYTE, t->pixels);
	SDL_FreeSurface(t);
      } else {
	fprintf(stderr, "can't open file %s : %s\n", _texture_filenames[i], SDL_GetError());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
      }
    }
  }
  glClearColor(0.0f, 0.0f, 0.1f, 0.0f);
  _pId1  = gl4duCreateProgram("<vs>shaders/full.vs", "<fs>shaders/full.fs", NULL);
  _pId2 = gl4duCreateProgram("<vs>shaders/atmos.vs", "<fs>shaders/atmos.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  resize(_wW, _wH);
  _sphere = gl4dgGenSpheref(_longitudes, _latitudes);
}

/*!\brief Cette fonction paramétre la vue (viewport) OpenGL en
 * fonction des dimensions de la fenêtre.
 */
static void resize(int w, int h) {
  _wW  = w;
  _wH = h;
  glViewport(0, 0, _wW, _wH);
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.5, 0.5, -0.5 * _wH / _wW, 0.5 * _wH / _wW, 1.0, 1000.0);
  gl4duBindMatrix("modelViewMatrix");
}

/*!\brief Cette fonction dessine dans le contexte OpenGL actif.
 */
static void draw(void) {
  int i;
  static GLfloat a0 = 0.0;
  static Uint32 t0 = 0;
  GLfloat dt = 0.0, steps[2] = {1.0f / _wW, 1.0f / _wH};
  GLfloat lumPos[4], *mat;
  Uint32 t;
  dt = ((t = SDL_GetTicks()) - t0) / 1000.0;
  t0 = t;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDisable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);
  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();
  gl4duTranslatef(0, 0, -3);
  mat = gl4duGetMatrixData();
  MMAT4XVEC4(lumPos, mat, _lumPos0);
  /* terre */
  glUseProgram(_pId1);
  gl4duRotatef(a0, 0, 1, 0);
  for(i = 0; i < TE_END; i++) {
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, _tId[i]);
    glUniform1i(glGetUniformLocation(_pId1, _sampler_names[i]), i);
  }
  glUniform1f(glGetUniformLocation(_pId1, "basses"), _basses);
  glUniform1f(glGetUniformLocation(_pId1, "aigus"), _aigus);
  glUniform2fv(glGetUniformLocation(_pId1, "steps"), 1, steps);
  glUniform4fv(glGetUniformLocation(_pId1, "lumPos"), 1, lumPos);
  /* envoi de toutes les matrices stockées par GL4D */
  gl4duSendMatrices();
  gl4dgDraw(_sphere);
  for(i = 0; i < TE_END; i++) {
    glActiveTexture(GL_TEXTURE0 + TE_END - 1 - i);
    glBindTexture(GL_TEXTURE_2D, 0);
  }
  /* atmosphere */
  glUseProgram(_pId2);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  gl4duScalef(1.04, 1.02, 1.04);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _tId[TE_ECLOUD]);
  glUniform1i(glGetUniformLocation(_pId2, _sampler_names[TE_ECLOUD]), 0);
  gl4duSendMatrices();
  gl4dgDraw(_sphere);

  a0 += 360.0 * dt / (24.0 /* * 60.0 */);
}

/*!\brief appelée quand l'audio est joué et met dans \a stream les
 * données audio de longueur \a len */
static void mixCallback(void *udata, Uint8 *stream, int len) {
  if(_plan4fftw) {
    int i, l = MIN(len >> 1, ECHANTILLONS);
    Sint16 *d = (Sint16 *)stream;
    for(i = 0; i < l; i++)
      _in4fftw[i][0] = d[i] / ((1 << 15) - 1.0);
    fftw_execute(_plan4fftw);
    for(i = 0, _basses = 0, _aigus = 0; i < l >> 2; i++) {
      if(i < l >> 3)
	_basses += sqrt(_out4fftw[i][0] * _out4fftw[i][0] + _out4fftw[i][1] * _out4fftw[i][1]);
      else
	_aigus  += sqrt(_out4fftw[i][0] * _out4fftw[i][0] + _out4fftw[i][1] * _out4fftw[i][1]);
    }
    _basses /= l >> 3;
    _aigus  /= l >> 3;
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
  /* préparation des conteneurs de données pour la lib FFTW */
  _in4fftw   = fftw_malloc(ECHANTILLONS *  sizeof *_in4fftw);
  memset(_in4fftw, 0, ECHANTILLONS *  sizeof *_in4fftw);
  assert(_in4fftw);
  _out4fftw  = fftw_malloc(ECHANTILLONS * sizeof *_out4fftw);
  assert(_out4fftw);
  _plan4fftw = fftw_plan_dft_1d(ECHANTILLONS, _in4fftw, _out4fftw, FFTW_FORWARD, FFTW_ESTIMATE);
  assert(_plan4fftw);
  res = Mix_Init(mixFlags);
  if( (res & mixFlags) != mixFlags ) {
    fprintf(stderr, "Mix_Init: Erreur lors de l'initialisation de la bibliotheque SDL_Mixer\n");
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
  if(_tId[0]) {
    glDeleteTextures(TE_END, _tId);
    _tId[0] = 0;
  }
  gl4duClean(GL4DU_ALL);
}

