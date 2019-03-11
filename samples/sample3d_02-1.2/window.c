/*!\file window.c
 *
 * \brief Lumière positionnelle + Phong + Bump mapping + Normal
 * mapping + textures et geometry shader
 *
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date March 19 2018
 */
#include <GL4D/gl4du.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL_image.h>

/* Prototypes des fonctions statiques contenues dans ce fichier C */
static void         init(void);
static void         resize(int w, int h);
static void         keyup(int keycode);
static void         keydown(int keycode);
static void         draw(void);
static void         quit(void);

/*!\brief dimensions de la fenêtre */
static int _wW = 800, _wH = 600;
/*!\brief identifiant des GLSL program */
static GLuint _pId = 0, _pId2 = 0, _pId3 = 0;
/*!\brief identifiant de la sphere */
static GLuint _sphere = 0;
/*!\brief nombre de longitudes et latitudes de la sphere */
static GLuint _longitudes = 40, _latitudes = 40;
/*!\brief arrete l'animation */
static GLuint _pause = 0;
/*!\brief normale à la face ou au point ? */
static GLuint _faceNormal = 0;
/*!\brief flag pour Phong ou pas */
static GLuint _phong = 1;
/*!\brief flag pour savoir si la touche shift est enfoncée */
static GLuint _shift = GL_FALSE;
/*!\brief flag pour calculer et utiliser une normalMap */
static GLuint _normalMap = 0;
/*!\brief flag pour utiliser la bumpMap */
static GLuint _bumpMap = 0;
/*!\brief flag pour plaquer les textures jour et nuit */
static GLuint _nightDay = 0;
/*!\brief flag pour activer la lumière spéculaire */
static GLuint _specular = 0;
/*!\brief position de la lumière relativement à la sphère éclairée */
static GLfloat _lumPos0[4] = {-15.1, 20.0, 20.7, 1.0};

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

/*!\brief La fonction principale créé la fenêtre d'affichage,
 * initialise GL et les données, affecte les fonctions d'événements et
 * lance la boucle principale d'affichage.
 */
int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "GL4Dummies", 10, 10, 
			 _wW, _wH, GL4DW_RESIZABLE | GL4DW_SHOWN))
    return 1;
  init();
  atexit(quit);
  gl4duwResizeFunc(resize);
  gl4duwKeyUpFunc(keyup);
  gl4duwKeyDownFunc(keydown);
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
	//#ifdef __APPLE__
	//int mode = t->format->BytesPerPixel == 4 ? GL_BGRA : GL_BGR;
	//#else
	int mode = t->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;
	//#endif       
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t->w, t->h, 0, mode, GL_UNSIGNED_BYTE, t->pixels);
	SDL_FreeSurface(t);
      } else {
	fprintf(stderr, "can't open file %s : %s\n", _texture_filenames[i], SDL_GetError());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
      }
    }
  }
  glClearColor(0.0f, 0.0f, 0.1f, 0.0f);
  _pId  = gl4duCreateProgram("<vs>shaders/basic.vs", "<gs>shaders/basic.gs", "<fs>shaders/basic.fs", NULL);
  _pId2 = gl4duCreateProgram("<vs>shaders/sol.vs", "<fs>shaders/sol.fs", NULL);
  _pId3 = gl4duCreateProgram("<vs>shaders/atmos.vs", "<fs>shaders/atmos.fs", NULL);
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

static void keyup(int keycode) {
  switch(keycode) {
  case SDLK_RSHIFT:
  case SDLK_LSHIFT:
    _shift = GL_FALSE;
    break;
  default:
    break;
  }
}

static void keydown(int keycode) {
  GLint v[2];
  switch(keycode) {
  case SDLK_UP:
    if(_shift) {
      _lumPos0[2] -= 0.1;
    } else {
      gl4dgDelete(_sphere);
      _sphere = gl4dgGenSpheref(_longitudes, ++_latitudes);
    }
    break;
  case SDLK_DOWN:
    if(_shift) {
      _lumPos0[2] += 0.1;
    } else {
      if(_latitudes > 2) {
	gl4dgDelete(_sphere);
	_sphere = gl4dgGenSpheref(_longitudes, --_latitudes);
      }
    }
    break;
  case SDLK_RIGHT:
    if(_shift) {
      _lumPos0[0] += 0.1;
    } else {
	gl4dgDelete(_sphere);
	_sphere = gl4dgGenSpheref(++_longitudes, _latitudes);
    }
    break;
  case SDLK_LEFT:
    if(_shift) {
      _lumPos0[0] -= 0.1;
    } else {
      if(_longitudes > 3) {
	gl4dgDelete(_sphere);
	_sphere = gl4dgGenSpheref(--_longitudes, _latitudes);
      }
    }
    break;
  case 'u':
    _lumPos0[1] += 0.1;
    break;
  case 'd':
    _lumPos0[1] -= 0.1;
    break;
  case 'w':
    glGetIntegerv(GL_POLYGON_MODE, v);
    if(v[0] == GL_FILL)
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    break;
  case ' ':
    _pause = !_pause;
    break;
  case 'p':
    _phong = !_phong;
    break;
  case 's':
    _specular = !_specular;
    break;
  case 't':
    _nightDay = !_nightDay;
    break;
  case 'n':
    _normalMap = !_normalMap;
    break;
  case 'b':
    _bumpMap = !_bumpMap;
    break;
  case 'f':
    _faceNormal = !_faceNormal;
    break;
  case SDLK_ESCAPE:
  case 'q':
    exit(0);
  case SDLK_RSHIFT:
  case SDLK_LSHIFT:
    _shift = GL_TRUE;
    break;
  default:
    break;
  }
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
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);
  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();
  gl4duTranslatef(0, 0, -3);
  mat = gl4duGetMatrixData();
  MMAT4XVEC4(lumPos, mat, _lumPos0);
  glUseProgram(_pId);
  gl4duPushMatrix();
  gl4duRotatef(a0, 0, 1, 0);
  for(i = 0; i < TE_END; i++) {
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, _tId[i]);
    glUniform1i(glGetUniformLocation(_pId, _sampler_names[i]), i);
  }
  glUniform2fv(glGetUniformLocation(_pId, "steps"), 1, steps);
  glUniform4fv(glGetUniformLocation(_pId, "lumPos"), 1, lumPos);
  glUniform1i(glGetUniformLocation(_pId, "faceNormal"), _faceNormal);
  glUniform1i(glGetUniformLocation(_pId, "phong"), _phong);
  glUniform1i(glGetUniformLocation(_pId, "normalMap"), _normalMap);
  glUniform1i(glGetUniformLocation(_pId, "bumpMap"), _bumpMap);
  glUniform1i(glGetUniformLocation(_pId, "nightDay"), _nightDay);
  glUniform1i(glGetUniformLocation(_pId, "specular"), _specular);
  /* envoi de toutes les matrices stockées par GL4D */
  gl4duSendMatrices();
  gl4dgDraw(_sphere);
  for(i = 0; i < TE_END; i++) {
    glActiveTexture(GL_TEXTURE0 + TE_END - 1 - i);
    glBindTexture(GL_TEXTURE_2D, 0);
  }
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glUseProgram(_pId3);
  gl4duScalef(1.04, 1.02, 1.04);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _tId[TE_ECLOUD]);
  glUniform1i(glGetUniformLocation(_pId, _sampler_names[TE_ECLOUD]), 0);
  gl4duSendMatrices();
  gl4dgDraw(_sphere);

  gl4duPopMatrix();

  gl4duTranslatef(_lumPos0[0], _lumPos0[1], _lumPos0[2]);
  gl4duScalef(0.05, 0.05, 0.05);
  glUseProgram(_pId2);
  gl4duSendMatrices();
  /* dessiner une troisième fois la sphère avec le _pId2 */
  gl4dgDraw(_sphere);
  if(!_pause)
    a0 += 360.0 * dt / (24.0 /* * 60.0 */);
}

/*!\brief appelée au moment de sortir du programme (atexit), libère les éléments utilisés */
static void quit(void) {
  if(_tId[0]) {
    glDeleteTextures(TE_END, _tId);
    _tId[0] = 0;
  }
  gl4duClean(GL4DU_ALL);
}

