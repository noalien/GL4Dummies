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
#include <GL4D/gl4dh.h>
#include "audioHelper.h"

/* Prototypes des fonctions statiques contenues dans ce fichier C */
static void         init(void);
static void         audio(void);
static void         draw(void);
static void         quit(void);

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

/*!\brief données entrées/sorties pour la lib fftw */
static fftw_complex * _in4fftw = NULL, * _out4fftw = NULL;
/*!\brief donnée à précalculée utile à la lib fftw */
static fftw_plan _plan4fftw = NULL;

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
  _pId1  = gl4duCreateProgram("<vs>shaders/full.vs", "<fs>shaders/full.fs", NULL);
  _pId2 = gl4duCreateProgram("<vs>shaders/atmos.vs", "<fs>shaders/atmos.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  _sphere = gl4dgGenSpheref(_longitudes, _latitudes);

  /* préparation des conteneurs de données pour la lib FFTW */
  _in4fftw   = fftw_malloc(ECHANTILLONS *  sizeof *_in4fftw);
  memset(_in4fftw, 0, ECHANTILLONS *  sizeof *_in4fftw);
  assert(_in4fftw);
  _out4fftw  = fftw_malloc(ECHANTILLONS * sizeof *_out4fftw);
  assert(_out4fftw);
  _plan4fftw = fftw_plan_dft_1d(ECHANTILLONS, _in4fftw, _out4fftw, FFTW_FORWARD, FFTW_ESTIMATE);
  assert(_plan4fftw);

}

/*!\brief Cette fonction dessine dans le contexte OpenGL actif.
 */
static void draw(void) {
  int i;
  GLint vp[4];
  static GLfloat a0 = 0.0;
  static Uint32 t0 = 0;
  GLfloat lumPos[4], *mat;
  Uint32 t;
  GLboolean gdt = glIsEnabled(GL_DEPTH_TEST);
  glGetIntegerv(GL_VIEWPORT, vp);
  GLfloat dt = 0.0, steps[2] = {1.0f / vp[2], 1.0f / vp[3]};
  dt = ((t = SDL_GetTicks()) - t0) / 1000.0;
  t0 = t;

  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.5, 0.5, -0.5 * vp[3] / vp[2], 0.5 * vp[3] / vp[2], 1.0, 1000.0);
  gl4duBindMatrix("modelViewMatrix");

  glClearColor(0.0f, 0.0f, 0.1f, 1.0f);

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
  if(!gdt)
    glDisable(GL_DEPTH_TEST);
}

/*!\brief appelée quand l'audio est joué et met dans \a stream les
 * données audio de longueur \a len */
static void audio(void) {
  if(_plan4fftw) {
    int i, l = MIN(ahGetAudioStreamLength() >> 1, ECHANTILLONS);
    Sint16 *d = (Sint16 *)ahGetAudioStream();
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

/*!\brief appelée au moment de sortir du programme (atexit), libère les éléments utilisés */
static void quit(void) {
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
}


void earth(int state) {
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
