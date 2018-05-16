/*!\file window.c
 *
 * \brief Lumière positionnelle + Gouraud + Phong et introduction au
 * geometry shader
 *
 * Exercice : ajouter le support de la lumière spéculaire (voir doc
 * wikipédia sur la lumière spéculaire et utiliser la fonction reflect
 * de GLSL).
 *
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date March 11 2018
 */

#include <GL4D/gl4du.h>
#include <GL4D/gl4duw_SDL2.h>

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
static GLuint _pId = 0, _pId2 = 0;
/*!\brief identifiant de la sphere */
static GLuint _sphere = 0;
/*!\brief nombre de longitudes et latitudes de la sphere */
static GLuint _longitudes = 10, _latitudes = 10;
/*!\brief arrete l'animation */
static GLuint _pause = 0;
/*!\brief normale à la face ou au point ? */
static GLuint _faceNormal = 0;
/*!\brief flag pour Phong ou pas (Gouraud) */
static GLuint _phong = 0;
/*!\brief flag pour savoir si la touche shift est enfoncée */
static GLuint _shift = GL_FALSE;
/*!\brief position de la lumière relativement à la sphère éclairée */
static GLfloat _lumPos0[4] = {1.1, 0.0, 0.7, 1.0};

/*!\brief La fonction principale créé la fenêtre d'affichage,
 * initialise GL et les données, affecte les fonctions d'événements et
 * lance la boucle principale d'affichage.
 */
int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "Lights", 10, 10, 
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
  glClearColor(0.0f, 0.4f, 0.9f, 0.0f);
  _sphere = gl4dgGenSpheref(_longitudes, _latitudes);
  _pId  = gl4duCreateProgram("<vs>shaders/basic.vs", "<gs>shaders/basic.gs", "<fs>shaders/basic.fs", NULL);
  _pId2 = gl4duCreateProgram("<vs>shaders/sol.vs", "<fs>shaders/sol.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  resize(_wW, _wH);
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
      _latitudes++;
      gl4dgDelete(_sphere);
      _sphere = gl4dgGenSpheref(_longitudes, _latitudes);
    }
    break;
  case SDLK_DOWN:
    if(_shift) {
      _lumPos0[2] += 0.1;
    } else {
      if(_latitudes > 2) {
	_latitudes--;
	gl4dgDelete(_sphere);
	_sphere = gl4dgGenSpheref(_longitudes, _latitudes);
      }
    }
    break;
  case SDLK_RIGHT:
    if(_shift) {
      _lumPos0[0] += 0.1;
    } else {
      _longitudes++;
      gl4dgDelete(_sphere);
      _sphere = gl4dgGenSpheref(_longitudes, _latitudes);
      
    }
    break;
  case SDLK_LEFT:
    if(_shift) {
      _lumPos0[0] -= 0.1;
    } else {
      if(_longitudes > 3) {
	_longitudes--;
	gl4dgDelete(_sphere);
	_sphere = gl4dgGenSpheref(_longitudes, _latitudes);
	
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
  static GLfloat a0 = 0.0;
  static Uint32 t0 = 0;
  GLfloat dt = 0.0;
  GLfloat lumPos[4], *mat;
  Uint32 t;
  dt = ((t = SDL_GetTicks()) - t0) / 1000.0;
  t0 = t;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();
  gl4duTranslatef(0, 0, -3);
  /* on récupère l'état de la matrice courante */
  mat = gl4duGetMatrixData();
  /* lumPos = mat x _lumPos0 */
  MMAT4XVEC4(lumPos, mat, _lumPos0);
  glUseProgram(_pId);
  gl4duPushMatrix();
  gl4duRotatef(a0, 0, 1, 0);
  glUniform4fv(glGetUniformLocation(_pId, "lumPos"), 1, lumPos);
  glUniform1i(glGetUniformLocation(_pId, "faceNormal"), _faceNormal);
  glUniform1i(glGetUniformLocation(_pId, "phong"), _phong);
  /* envoi de toutes les matrices stockées par GL4D */
  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_sphere);
  gl4duPopMatrix();
  gl4duTranslatef(_lumPos0[0], _lumPos0[1], _lumPos0[2]);
  gl4duScalef(0.05, 0.05, 0.05);
  glUseProgram(_pId2);
  gl4duSendMatrices();
  /* dessiner une seconde fois la sphère avec le _pId2 */
  gl4dgDraw(_sphere);
  if(!_pause)
    a0 += 360 * dt;
}

/*!\brief appelée au moment de sortir du programme (atexit), libère
 * les éléments utilisés */
static void quit(void) {
  gl4duClean(GL4DU_ALL);
}
