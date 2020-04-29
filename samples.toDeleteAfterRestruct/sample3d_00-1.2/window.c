/*!\file window.c
 *
 * \brief GL4Dummies, exemple 3D simple avec dessin d'un quadrilatère
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date February 11 2018
 */

#include <GL4D/gl4du.h>
#include <GL4D/gl4duw_SDL2.h>

/* Prototypes des fonctions statiques contenues dans ce fichier C */
static void init(void);
static void draw(void);
static void quit(void);

/*!\brief largeur et hauteur de la fenêtre */
static int _wW = 800, _wH = 600;
/*!\brief identifiant du (futur) Vertex Array Object */
static GLuint _vao = 0;
/*!\brief identifiant du (futur) buffer de data - VBO ou Vertex Buffer Object */
static GLuint _buffer = 0;
/*!\brief identifiant du (futur) GLSL program */
static GLuint _pId = 0;

/*!\brief créé la fenêtre d'affichage, initialise GL et les données,
 * affecte les fonctions d'événements et lance la boucle principale
 * d'affichage.
 */
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
 *
 * Exercice (corrigé en 1.3) : utiliser GL4Dummies (voir manuel + doc
 * de référence concernant gl4du.h) pour créer
 * (gl4duGenMatrix/gl4duBindMatrix) une matrice de projection
 * (orthogonale (gl4duOrthof) ou perspective (gl4duFrustumf)) et une
 * matrice de modélisation et de vue. De plus, les fonctions
 * gl4duLoadIdentityf, gl4duTranslatef, gl4duRotatef, gl4duSendMatrix
 * ou gl4duSendMatrices peuvent être utiles ; ne pas oublier de
 * récupérer les matrices dans le vertex shader et de les appliquer.
 */
static void init(void) {
  /* données envoyées par tranches sommet-attributs dans le VBO */
  GLfloat data[] = {
    /* 4 coordonnées de sommets en 2D chacune suivie de sa couleur */
    -1.0f, -1.0f, 
    1.0f, 0.0f, 0.0f, 
    1.0f, -1.0f,
    0.0f, 1.0f, 0.0f,
    -1.0f,  1.0f,
    0.0f, 0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 1.0f, 0.0f
  };
  /* Création du programme shader (voir le dossier shader) */
  _pId = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic.fs", NULL);
  /* Set de la couleur (RGBA) d'effacement OpenGL */
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
  /* dans quelle partie de l'écran on effectue le rendu */
  glViewport(0, 0, _wW, _wH);
  /* Génération d'un identifiant de VAO */
  glGenVertexArrays(1, &_vao);
  /* Lier le VAO-machine-GL à l'identifiant VAO généré */
  glBindVertexArray(_vao);
  /* Activation des 2 premiers indices d'attribut de sommet */
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  /* Génération d'un identifiant de VBO */
  glGenBuffers(1, &_buffer);
  /* Lier le VBO-machine-GL à l'identifiant VBO généré */
  glBindBuffer(GL_ARRAY_BUFFER, _buffer);
  /* Transfert des données VBO */
  glBufferData(GL_ARRAY_BUFFER, sizeof data, data, GL_STATIC_DRAW);
  /* Paramétrage 2 premiers indices d'attribut de sommet */
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof *data, (const void *)0);  
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof *data, (const void *)(2 * sizeof *data));
  /* dé-lier le VAO et VBO */
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
/*!\brief Cette fonction dessine dans le contexte OpenGL actif. */
static void draw(void) {
  /* une variable d'angle */
  static GLfloat angle = 0.0f;
  /* effacement du buffer de couleur */
  glClear(GL_COLOR_BUFFER_BIT);
  /* activation du programme _pId */
  glUseProgram(_pId);
  /* on transmet la variable uniform d'angle */
  glUniform1f(glGetUniformLocation(_pId, "angle"), angle);
  /* on incrémente angle d'un 1/60 de tour. Tester l'application en
   * activant/désactivant la synchronisation verticale de votre carte
   * graphique. Que se passe-t-il ? Trouver une solution pour que
   * résultat soit toujours le même. */
  angle += (1.0f / 60.0f) * 2.0f * M_PI;
  /* Lier le VAO-machine-GL à l'identifiant VAO _vao */
  glBindVertexArray(_vao);
  /* Dessiner le VAO comme une bande de deux triangles avec 4 sommets commençant à 0 */
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  /* dé-lier le VAO */
  glBindVertexArray(0);
  /* désactiver le programme shader */
  glUseProgram(0);
}
/*!\brief appelée au moment de sortir du programme (atexit), elle
 *  libère les éléments OpenGL utilisés.*/
static void quit(void) {
  /* suppression du VAO _vao en GPU */
  if(_vao)
    glDeleteVertexArrays(1, &_vao);
  /* suppression du VBO _buffer en GPU */
  if(_buffer)
    glDeleteBuffers(1, &_buffer);
  /* nettoyage des éléments utilisés par la bibliothèque GL4Dummies */
  gl4duClean(GL4DU_ALL);
}
