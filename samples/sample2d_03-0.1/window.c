/*!\file window.c
 *
 * \brief premier exemple de code GL4Dummies/OpenGL avec création et
 * utilisation d'un shader sur une géométrie de quadrilatère. Le
 * quadrilatère subit un changement d'échelle variable utilisant une
 * sinusoïdale.
 *
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date March 11, 2018
 */
#include <GL4D/gl4du.h>
#include <GL4D/gl4duw_SDL2.h>
/* Prototypes des fonctions statiques contenues dans ce fichier C */
static void init(void);
static void draw(void);
static void quit(void);
/*!\brief largeur et hauteur de la fenêtre */
static int _wW = 800, _wH = 600;
/*!\brief identifiant du quadrilatère faisant office de "rectangle écran" */
static GLuint _quad = 0;
/*!\brief identifiant du (futur) GLSL program */
static GLuint _pId = 0;
/*!\brief créé la fenêtre d'affichage, initialise GL et les données,
 * affecte les fonctions d'événements et lance la boucle principale
 * d'affichage. */
int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "GL4Dummies", 20, 20, 
			 _wW, _wH, GL4DW_RESIZABLE | GL4DW_SHOWN))
    return 1;
  init();
  atexit(quit);
  gl4duwDisplayFunc(draw);
  gl4duwMainLoop();
  return 0;
}
/*!\brief initialise les paramètres OpenGL et les données. 
 */
static void init(void) {
  /* Création du programme shader (voir le dossier shader) */
  _pId = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic.fs", NULL);
  /* Set de la couleur (RGBA) d'effacement OpenGL */
  glClearColor(1.2f, 0.2f, 0.2f, 1.0f);
  /* dans quelle partie de l'écran on effectue le rendu */
  glViewport(0, 0, _wW, _wH);
  /* génération de la géométrie du quadrilatère */
  _quad = gl4dgGenQuadf();
}
/*!\brief Cette fonction dessine dans le contexte OpenGL actif. */
static void draw(void) {
  static int c = 0;
  /* effacement du buffer de couleur */
  glClear(GL_COLOR_BUFFER_BIT);
  /* activation du programme de rendu _pId */
  glUseProgram(_pId);
  /* envoi de la valeur de la variable c dans la variable uniform
   * count du côté shader (GPU). Puis c incrémente de 1 pour l'appel
   * suivant. */
  glUniform1i(glGetUniformLocation(_pId, "count"), c++);
  /* dessin de la géométrie du quadrilatère */
  gl4dgDraw(_quad);
  /* désactiver le programme shader */
  glUseProgram(0);
}
/*!\brief appelée au moment de sortir du programme (atexit), elle
 *  libère les éléments OpenGL utilisés.*/
static void quit(void) {
  /* nettoyage des éléments utilisés par la bibliothèque GL4Dummies */
  gl4duClean(GL4DU_ALL);
}
