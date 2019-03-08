/*!\file window.c
 *
 * \brief GL4Dummies, exemple 3D avec fabrication d'un triangle plein
 * tourant dans un cube unitaire en fil de fer plus des
 * transformations spaciales projection/modélisation utilisant les
 * fonction gl4duXXX
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
static int _wW = 800, _wH = 800;
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
 * Exercice compliqué (corrigé en 1.5) : séparer la vue de la
 * modélisation en utilisant deux matrices disjointes (par exemple
 * viewMatrix et modelMatrix). Refaire la même vue que celle obtenue
 * ici en n'utilisant que des rotate et translate (pas de lookAt).
 *
 * Exercice compliqué (corrigé en 1.5) : supprimer les 6x4 sommets du
 * cube de data et mettre à la place 8 sommets représentant les 8
 * sommets du cube. Mettre du Rouge-Vert-Bleu-Jaune sombres pour les 4
 * de devant et saturées-claires pour les 4 de derrière. Utiliser un
 * second VBO de type GL_ELEMENT_ARRAY_BUFFER et un tableau idata
 * contenant 6x4 indices des sommets du cube (avec répétition) de
 * manière à reconstruire le même cube avec des LINE_LOOP mais cette
 * fois-ci en utilisant la fonction glDrawElements. Enfin, ajouter les
 * 3 axes X (en rouge), Y (en vert) et Z (en bleu).
 */
static void init(void) {
  /* données envoyées par tranches sommet-attributs dans le VBO */
  GLfloat data[] = {
    /* 3 coordonnées de sommets en 3D chacune suivie de sa couleur */
    -1.5f, -1.5f, 0.0f,
    1.0f, 0.0f, 0.0f, 
    1.5f, -1.5f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f,  1.5f, 0.0f,
    0.0f, 0.0f, 1.0f,
    /* 4 sommets de la face haut */
    -1, 1,  1, 0, 0, 0,
     1, 1,  1, 0, 0, 0,
     1, 1, -1, 0, 0, 0,
    -1, 1, -1, 0, 0, 0,
    /* 4 sommets de la face bas */
    -1, -1,  1, 0, 0, 0,
    -1, -1, -1, 0, 0, 0,
     1, -1, -1, 0, 0, 0,
     1, -1,  1, 0, 0, 0,
    /* 4 sommets de la face avant */
    -1, -1, 1, 0, 0, 0,
     1, -1, 1, 0, 0, 0,
     1,  1, 1, 0, 0, 0,
    -1,  1, 1, 0, 0, 0,
    /* 4 sommets de la face arrière */
    -1, -1, -1, 0, 0, 0,
    -1,  1, -1, 0, 0, 0,
     1,  1, -1, 0, 0, 0,
     1, -1, -1, 0, 0, 0,
    /* 4 sommets de la face droite */
    1, -1, -1, 0, 0, 0,
    1,  1, -1, 0, 0, 0,
    1,  1,  1, 0, 0, 0,
    1, -1,  1, 0, 0, 0,
    /* 4 sommets de la face gauche */
    -1, -1, -1, 0, 0, 0,
    -1, -1,  1, 0, 0, 0,
    -1,  1,  1, 0, 0, 0,
    -1,  1, -1, 0, 0, 0
  };
  /* Création du programme shader (voir le dossier shader) */
  _pId = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic.fs", NULL);
  /* Set de la couleur (RGBA) d'effacement OpenGL */
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
  /* activation du test de profondeur afin de prendre en compte la
   * notion de devant-derrière. */
  glEnable(GL_DEPTH_TEST);
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
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof *data, (const void *)0);  
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof *data, (const void *)(3 * sizeof *data));
  /* dé-lier le VAO et VBO */
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
/*!\brief Cette fonction dessine dans le contexte OpenGL actif. */
static void draw(void) {
  /* une variable d'angle, maintenant elle passe ne degrés */
  static GLfloat angle = 0.0f;
  /* effacement du buffer de couleur, nous rajoutons aussi le buffer
   * de profondeur afin de bien rendre les fragments qui sont devant
   * au dessus de ceux qui sont derrière. Commenter le
   * "|GL_DEPTH_BUFFER_BIT" pour voir la différence. Nous avons ajouté
   * l'activation du test de profondeur dans la fonction init via
   * l'appel glEnable(GL_DEPTH_TEST). */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  /* activation du programme _pId */
  glUseProgram(_pId);
  /* lier (mettre en avant ou "courante") la matrice modélisation-vue
   * créée dans init */
  gl4duBindMatrix("modelViewMatrix");
  /* Charger la matrice identité dans la matrice courante (liée) */
  gl4duLoadIdentityf();
  /* Composer la matrice vue courante en simulant une "caméra" à
   * l'aide de la fonction LookAt(xyz_position_cam,
   * xyz_ou_elle_regarde, xyz_son_vecteur_haut) */
  gl4duLookAtf(-3, 3, -5, 0, 0, 0, 0, 1, 0);
  /* Envoyer, au shader courant, toutes les matrices connues dans
   * GL4Dummies, ici on intègre pas la rotation qui vient après */
  gl4duSendMatrices();
  /* Lier le VAO-machine-GL à l'identifiant VAO _vao */
  glBindVertexArray(_vao);
  /* Dessiner le VAO comme des line loop pour chaque face de 4 sommets
   * en commençant par l'indice 3 (+4 à chaque fois, sur 6
   * faces). Pour tester, vous pouvez remplacer tous les GL_LINE_LOOP
   * par des GL_TRIANGLE_FAN */
  glDrawArrays(GL_LINE_LOOP, 3, 4);
  glDrawArrays(GL_LINE_LOOP, 7, 4);
  glDrawArrays(GL_LINE_LOOP, 11, 4);
  glDrawArrays(GL_LINE_LOOP, 15, 4);
  glDrawArrays(GL_LINE_LOOP, 19, 4);
  glDrawArrays(GL_LINE_LOOP, 23, 4);
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
  /* Dessiner le VAO comme une bande d'un triangle avec 3 sommets commençant à 0 */
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
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
