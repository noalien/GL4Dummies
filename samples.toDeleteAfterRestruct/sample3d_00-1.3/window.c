/*!\file window.c
 *
 * \brief GL4Dummies, exemple 3D simple avec dessin d'un quadrilatère
 * plus des transformations spaciales projection/modélisation
 * utilisant les fonction gl4duXXX  
 * \author Farès BELHADJ,
 * amsi@ai.univ-paris8.fr \date February 11 2018
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
 * Exercice compliqué (corrigé en 1.4), faire dans l'ordre : (1)
 * revenir à un triangle dépassant légèrement du cube unitaire (par
 * exemple ses coordonnées varient entre -1.5 et +1.5) et rajouter la
 * coordonnée z à 0.0f (modifier glVertexAttribPointer et le vertex
 * shader en conséquence). (2) Ajouter dans data 6x4 sommets (tous
 * couleur noire) représentant les 6 faces du cube unitaire où chaque
 * face sera dessinée avec une LINE_LOOP de 4 sommets (voir doc de
 * glDrawArrays). Le triangle continue à tourner mais le cube ne doit
 * pas tourner. (3) Modifier la vue (il sera plus facile d'utiliser
 * gl4duLookAtf) de la matrice modélisation-vue afin de voir le cube
 * et le triangle depuis une position en haut à droite. Plus tard, si
 * vous le souhaiter vous pourrez séparer la vue de la modélisation en
 * utilisant deux matrices disjointes (par exemple viewMatrix et
 * modelMatrix).
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
   /* Création des matrices GL4Dummies, une pour la projection, une pour la modélisation-vue */
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  /* on active la matrice de projection créée précédemment */
  gl4duBindMatrix("projectionMatrix");
  /* la matrice en cours reçoit identité (matrice avec des 1 sur la
   * diagonale et que des 0 sur le reste) */
  gl4duLoadIdentityf();
  /* on multiplie la matrice en cours par une matrice de projection
   * orthographique ou perspective */
  /* décommenter pour orthographique gl4duOrthof(-1, 1, -1, 1, 0, 100); */
  gl4duFrustumf(-1, 1, -1, 1, 2, 100);
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
  /* une variable d'angle, maintenant elle passe ne degrés */
  static GLfloat angle = 0.0f;
  /* effacement du buffer de couleur */
  glClear(GL_COLOR_BUFFER_BIT);
  /* activation du programme _pId */
  glUseProgram(_pId);
  /* lier (mettre en avant ou "courante") la matrice modélisation-vue
   * créée dans init */
  gl4duBindMatrix("modelViewMatrix");
  /* Charger la matrice identité dans la matrice courante (liée) */
  gl4duLoadIdentityf();
  /* Composer la matrice courante avec une translation (voir la
   * projection perspective dans le manuel pour comprendre pourquoi
   * nous devons éloigner de -3 en z les objets dessinés) */
  gl4duTranslatef(0, 0, -3);
  /* on transmet la variable d'angle en l'intégrant dans la matrice de
   * modélisation-vue. Soit composer la matrice courante avec une
   * rotation "angle" autour de l'axe y (0, 1, 0) */
  gl4duRotatef(angle, 0, 1, 0);
  /* on incrémente angle d'un 1/60 de tour soit (360° x 1/60). Tester
   * l'application en activant/désactivant la synchronisation
   * verticale de votre carte graphique. Que se passe-t-il ? Trouver
   * une solution pour que résultat soit toujours le même. */
  angle += (1.0f / 60.0f) * 360.0f;
  /* Envoyer, au shader courant, toutes les matrices connues dans
   * GL4Dummies */
  gl4duSendMatrices();
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
