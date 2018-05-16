/*!\file window.c
 *
 * \brief Terre en GL / GL4Dummies + Texture + Lumière positionnelle +
 * Phong + Spéculaire
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date April 15 2016 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL_image.h>

/* Prototypes des fonctions statiques contenues dans ce fichier C */
static void         initGL(void);
static void         initData(void);
static void         resize(int w, int h);
static void         keyup(int keycode);
static void         keydown(int keycode);
static void         draw(void);
static void         quit(void);

/*!\brief dimensions de la fenêtre */
static int _windowWidth = 1200, _windowHeight = 600;
/*!\brief identifiant du programme GLSL */
static GLuint _pId = 0;
/*!\brief arrete l'animation */
static GLuint _pause = 0;
/*!\brief flag pour savoir si la touche shift est enfoncée */
static GLuint _shift = GL_FALSE;
/*!\brief flag pour activer la lumière spéculaire */
static GLuint _specular = 0;
/*!\brief position de la lumière relativement à la sphère éclairée */
static GLfloat _lumPos0[4] = {1500.0, 20.0, 30.0, 1.0};
/*!\brief tableau des identifiants de texture à charger */
static GLuint _tId[3] = {0};
/*!\brief pour les deux astres : terre et lune */
static GLuint _sphere = {0};
/*!\brief les différents modes de vue */
static GLuint _mode = 0;

/*!\brief La fonction principale créé la fenêtre d'affichage,
 * initialise GL et les données, affecte les fonctions d'événements et
 * lance la boucle principale d'affichage.
 */
int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "GL4Dummies", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
			 _windowWidth, _windowHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN))
    return 1;
  initGL();
  initData();
  atexit(quit);
  gl4duwResizeFunc(resize);
  gl4duwKeyUpFunc(keyup);
  gl4duwKeyDownFunc(keydown);
  gl4duwDisplayFunc(draw);
  gl4duwMainLoop();
  return 0;
}

/*!\brief initialise les paramètres OpenGL */
static void initGL(void) {
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.0f, 0.0f, 0.1f, 0.0f);
  _pId  = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  resize(_windowWidth, _windowHeight);
}

/*!\brief initialise les données */
static void initData(void) {
  int i;
  SDL_Surface * t;
  static char * files[] = {"images/land_ocean_ice_2048.png", "images/land_ocean_ice_2048_glossmap.png", "images/moon.jpg" };
  glGenTextures(3, _tId);
  for(i = 0; i < 3; i++) {
    glBindTexture(GL_TEXTURE_2D, _tId[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    if( (t = IMG_Load(files[i])) != NULL ) {
#ifdef __APPLE__
      int mode = t->format->BytesPerPixel == 4 ? GL_BGRA : GL_BGR;
#else
      int mode = t->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;
#endif       
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t->w, t->h, 0, mode, GL_UNSIGNED_BYTE, t->pixels);
      SDL_FreeSurface(t);
    } else {
      fprintf(stderr, "can't open file %s : %s\n", files[i], SDL_GetError());
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }
  }
  _sphere = gl4dgGenSpheref(30, 30);
}

/*!\brief Cette fonction paramétre la vue (viewport) OpenGL en
 * fonction des dimensions de la fenêtre.
 */
static void resize(int w, int h) {
  _windowWidth  = w;
  _windowHeight = h;
  glViewport(0, 0, _windowWidth, _windowHeight);
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.5, 0.5, -0.5 * _windowHeight / _windowWidth, 0.5 * _windowHeight / _windowWidth, 1.0, 1000.0);
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
    if(_shift)
      _lumPos0[2] -= 1;
    else
      _lumPos0[1] += 1;
    break;
  case SDLK_DOWN:
    if(_shift)
      _lumPos0[2] += 1;
    else
      _lumPos0[1] -= 1;
    break;
  case SDLK_RIGHT:
    _lumPos0[0] += 1;
    break;
  case SDLK_LEFT:
    _lumPos0[0] -= 1;
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
  case 's':
    _specular = !_specular;
    break;
  case 'm':
    _mode = (_mode + 1) % 2;
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

/*!\brief dessine dans le contexte OpenGL actif. */
static void draw(void) {
  static GLfloat a0 = 0.0;
  static Uint32 t0 = 0, t;
  GLfloat dt = 0.0, lumPos[4], *mat;
  dt = ((t = SDL_GetTicks()) - t0) / 1000.0;
  t0 = t;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();
  if(_mode == 0)
    gl4duTranslatef(0, 0, -5);
  else
    gl4duTranslatef(0, 0, -14);
  mat = gl4duGetMatrixData();
  MMAT4XVEC4(lumPos, mat, _lumPos0);

  glUseProgram(_pId);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _tId[0]);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, _tId[1]);
  glUniform1i(glGetUniformLocation(_pId, "tex0"), 0);
  glUniform1i(glGetUniformLocation(_pId, "tex1"), 1);
  glUniform4fv(glGetUniformLocation(_pId, "lumPos"), 1, lumPos);
  glUniform1i(glGetUniformLocation(_pId, "specular"), _specular);
  /* envoi de toutes les matrices stockées par GL4D */
  gl4duPushMatrix(); {
    gl4duRotatef(a0, 0, 1, 0);
    gl4duSendMatrices();
  } gl4duPopMatrix();
  gl4dgDraw(_sphere);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, 0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _tId[2]);
  gl4duPushMatrix(); {
    gl4duRotatef(a0 / 28, 0, 1, 0);
    gl4duTranslatef(-6, 0, 0);  
    gl4duScalef(0.2, 0.2, 0.2);
    gl4duSendMatrices();
  } gl4duPopMatrix();
  glUniform1i(glGetUniformLocation(_pId, "specular"), 0);
  gl4dgDraw(_sphere);

  if(!_pause)
    a0 += 360.0 * dt / (_mode == 0 ? 24.0 * 6.0 : 6.0);
}

/*!\brief appelée au moment de sortir du programme (atexit), libère les éléments utilisés */
static void quit(void) {
  gl4duClean(GL4DU_ALL);
}

