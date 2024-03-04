/*!\file window.c
 *
 * \brief utilisation de GL4Dummies et Lib Assimp pour chargement de
 * modèles 3D sous différents formats.
 *
 * \author Farès Belhadj amsi@up8.edu
 * \date May 21 2023
 */

#include <GL4D/gl4duw_SDL2.h>
#include <SDL_image.h>
#include "assimp.h"

/*!\brief opened window width */
static int _windowWidth = 600;
/*!\brief opened window height */
static int _windowHeight = 800;
/*!\brief GLSL program Id */
static GLuint _pId = 0;

/*!\brief identifiant de modèles générés à partir de fichiers 3D (3dsmax, obj, ...) */
static GLuint _id_modele[3] = { 0 };

static void init(void);
static void sortie(void);
static void resize(int w, int h);
static void draw(void);

int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "GL4Dummies", GL4DW_POS_UNDEFINED, GL4DW_POS_UNDEFINED,
                         _windowWidth, _windowHeight, GL4DW_RESIZABLE | GL4DW_SHOWN))
    return 1;
  init();
  atexit(sortie);
  gl4duwResizeFunc(resize);
  gl4duwDisplayFunc(draw);
  gl4duwMainLoop();
  return 0;
}

void init(void) {
  /* charger 3 modèles différents */
  _id_modele[0] = assimpGenScene("models/soccer/soccerball.obj");
  _id_modele[1] = assimpGenScene("models/nixanz.3ds");
  _id_modele[2] = assimpGenScene("models/balloon/balloon_low.obj");  

  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  _pId = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  resize(_windowWidth, _windowHeight);
}

/*!\brief function called by GL4Dummies' loop at resize. Sets the
 *  projection matrix and the viewport according to the given width
 *  and height.
 * \param w window width
 * \param h window height
 */
void resize(int w, int h) {
  _windowWidth = w; 
  _windowHeight = h;
  glViewport(0, 0, _windowWidth, _windowHeight);
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.005, 0.005, -0.005 * _windowHeight / _windowWidth, 0.005 * _windowHeight / _windowWidth, 0.01, 1000.0);
  /* alternative : gl4duPerspectivef(60.0, (GLfloat)_windowWidth/(GLfloat)_windowHeight, 0.01, 1000.0); */
  gl4duBindMatrix("modelViewMatrix");
}

void draw(void) {
  int i;
  const int nb_modeles = sizeof _id_modele / sizeof *_id_modele, total = 12;
  static GLfloat angle = 0.0f;
  GLfloat lum[4] = {0.0, 0.0, 5.0, 1.0};
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(_pId);

  glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lum);

  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();

  gl4duLookAtf(0.0f, 5.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
  /* on va en dessiner "total" objets en alternant entre les 3 modèles ... */
  for(i = 0; i < total; ++i) {
    gl4duPushMatrix(); /* sauver la matrice (modelViewMatrix) */
    gl4duRotatef(angle + i * (360.0f / total), 0.0f, 1.0f, 0.0f);   
    gl4duTranslatef(3.0f, 0.0f, 0.0f);
    gl4duRotatef(-angle, 0.0f, 1.0f, 0.0f);
    assimpDrawScene(_id_modele[i % nb_modeles]);
    gl4duPopMatrix(); /* restaurer la matrice (modelViewMatrix) */
  }
  
  /* gestion de l'angle en fonction du temps.
   *
   * L'idéal est de le mettre dans une fonction idle (simulation) ...
   */
  {
    static double t0 = 0.0;
    double t = gl4dGetElapsedTime(), dt = (t - t0) / 1000.0;
    angle += 45.0 * dt; /* quart de tour par seconde : 45° */
    t0 = t;
  }
}

void sortie(void) {
  gl4duClean(GL4DU_ALL);
}

