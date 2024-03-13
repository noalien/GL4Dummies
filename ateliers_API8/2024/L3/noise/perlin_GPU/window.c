/*!\file window.c
 *
 * \brief Bruit de Perlin appliqué en GPU
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date March 3 2017
 */
#include <GL4D/gl4du.h>
#include <GL4D/gl4dg.h>
#include <GL4D/gl4duw_SDL2.h>

/* Prototypes des fonctions statiques contenues dans ce fichier C */
static void         init(void);
static void         keydown(int keycode);
static void         draw(void);
static void         quit(void);
/* fonctions externes dans noise.c */
extern void initNoiseTextures(void);
extern void useNoiseTextures(GLuint pid, int shift);
extern void unuseNoiseTextures(int shift);
extern void freeNoiseTextures(void);


/*!\brief identifiant de la géométrie */
static GLuint _geom = 0;
/*!\brief identifiant des GLSL program */
static GLuint _pId = 0;
/*!\brief arrete l'animation */
static GLuint _pause = 0;
/*!\brief coefficient de zoom */
static GLfloat _zoom = 3.0;
/*!\brief temps */
static GLfloat _temps = 0.1;

/*!\brief La fonction principale créé la fenêtre d'affichage,
 * initialise GL et les données, affecte les fonctions d'événements et
 * lance la boucle principale d'affichage.
 */
int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "DL4D-Earth", 0, 0, 
			 1280, 1024, SDL_WINDOW_SHOWN))
    return 1;
  init();
  atexit(quit);
  gl4duwKeyDownFunc(keydown);
  gl4duwDisplayFunc(draw);
  gl4duwMainLoop();
  return 0;
}

/*!\brief initialise les paramètres OpenGL */
static void init(void) {
  initNoiseTextures();
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.0f, 0.0f, 0.1f, 0.0f);
  glViewport(0, 0, 1280, 1024);
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.5, 0.5, -0.5, 0.5, 1.0, 1000.0);
  gl4duBindMatrix("modelViewMatrix");
  _pId  = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic.fs", NULL);
  _geom = gl4dgGenTorusf(100, 20, 0.3);
}

static void keydown(int keycode) {
  switch(keycode) {
  case SDLK_DOWN:
    _zoom -= 0.1;
    break;
  case SDLK_UP:
    _zoom += 0.1;
    break;
  case ' ':
    _pause = !_pause;
    break;
  case SDLK_ESCAPE:
  case 'q':
    exit(0);
  default:
    break;
  }
}

/*!\brief Cette fonction dessine dans le contexte OpenGL actif.*/
static void draw(void) {
  static GLfloat a0 = 0.0;
  static Uint32 t0 = 0;
  GLfloat dt = 0.0;
  Uint32 t;
  dt = ((t = SDL_GetTicks()) - t0) / 1000.0;
  t0 = t;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();
  gl4duTranslatef(0, 0, -3);
  glUseProgram(_pId);
  gl4duPushMatrix();
  gl4duRotatef(0.2 * a0, 0, 1, 0);
  gl4duRotatef(a0, 1, 0, 0);
  glUniform1f(glGetUniformLocation(_pId, "zoom"), _zoom);
  glUniform1f(glGetUniformLocation(_pId, "temps"), _temps);
  /* envoi de toutes les matrices stockées par GL4D */
  useNoiseTextures(_pId, 0);
  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_geom);
  unuseNoiseTextures(0);
  gl4duPopMatrix();

  if(!_pause)
    _temps += dt / 50.0;
  a0 += 360.0 * dt / (24.0 /* * 60.0 */);
}

/*!\brief appelée au moment de sortir du programme (atexit), libère les éléments utilisés */
static void quit(void) {
  freeNoiseTextures();
  gl4duClean(GL4DU_ALL);
}
