/*!\file window.c
 *
 * \brief GL4Dummies, exemple 3D avec fabrication indexée d'un
 * triangle plein, un cube unitaire en fil de fer et le dessin des
 * trois axes x y et z.  plus des transformations spaciales
 * projection/modélisation utilisant les fonction gl4duXXX
 * 
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
static GLuint _buffer[2] = { 0 };
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
/*!\brief initialise les paramètres OpenGL et les données. */
static void init(void) {
  /* indices pour réaliser le maillage des géométrie, envoyés dans le
   * VBO ELEMENT_ARRAY_BUFFER */
  GLuint idata[] = {
    /* le triangle rouge-vert-bleu */
    0, 1, 2,
    /* face haut du cube unitaire */
    3, 4, 5, 6,
    /* face bas du cube unitaire  */
    7, 10, 9, 8,
    /* face avant du cube unitaire  */
    3, 7, 8, 4,
    /* face arriere du cube unitaire  */
    10, 6, 5, 9,
    /* face gauche du cube unitaire  */
    7, 3, 6, 10,
    /* face droite du cube unitaire  */
    8, 9, 5, 4,
    /* l'axe des x (abscisses) : une flèche */
    11, 12, 13, 12, 14,
    /* l'axe des y (ordonnées) : une flèche */
    11, 15, 16, 15, 17,
    /* l'axe des z (profondeur) : une flèche */
    11, 18, 19, 18, 20
  };
  /* données-sommets envoyée dans le VBO ARRAY_BUFFER */
  GLfloat data[] = {
    /* 3 coordonnées de sommets en 3D chacune suivie de sa couleur
     * pour un triangle */
    /* sommet  0 */ -1.5f, -1.5f, 0.0f, 1.0f, 0.0f, 0.0f, 
    /* sommet  1 */ 1.5f, -1.5f, 0.0f, 0.0f, 1.0f, 0.0f,
    /* sommet  2 */ 0.0f,  1.5f, 0.0f, 0.0f, 0.0f, 1.0f,
    /* 8 coordonnées de sommets en 3D chacune suivie de sa couleur
     * pour les sommets du cube unitaire */
    /* sommet  3 */ -1, 1, 1, 0.5, 0, 0,
    /* sommet  4 */ 1, 1, 1, 0, 0.5, 0,
    /* sommet  5 */ 1, 1, -1, 0, 1, 0,
    /* sommet  6 */ -1, 1, -1, 1, 0, 0,
    /* sommet  7 */ -1, -1, 1, 0.5, 0.5, 0,
    /* sommet  8 */ 1, -1, 1, 0, 0, 0.5,
    /* sommet  9 */ 1, -1, -1, 0, 0, 1,
    /* sommet 10 */ -1, -1, -1, 1, 1, 0,
    /* sommet 11 */ 0, 0, 0, 0, 0, 0,
    /* 3x5 sommets pour les 3 flèches représentant les axes */
    /* sommet 12 */ 3, 0, 0, 1, 0, 0,
    /* sommet 13 */ 2.9f, 0.1f, 0, 1, 0, 0,
    /* sommet 14 */ 2.9f, -0.1f, 0, 1, 0, 0,
    /* sommet 15 */ 0, 3, 0, 0, 1, 0,
    /* sommet 16 */ 0.1f, 2.9f, 0, 0, 1, 0,
    /* sommet 17 */ -0.1f, 2.9f, 0, 0, 1, 0,
    /* sommet 18 */ 0, 0, 3, 0, 0, 1,
    /* sommet 19 */ 0, 0.1f, 2.9f, 0, 0, 1,
    /* sommet 20 */ 0, -0.1f, 2.9f, 0, 0, 1
  };
  /* Création du programme shader (voir le dossier shader) */
  _pId = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic.fs", NULL);
  /* Set de la couleur (RGBA) d'effacement OpenGL */
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
  /* activation du test de profondeur afin de prendre en compte la
   * notion de devant-derrière. */
  glEnable(GL_DEPTH_TEST);
  /* activation du lissage de dessin de lignes ; permet d'utiliser
   * aussi le glLineWidth (voir dans le draw) */
  glEnable(GL_LINE_SMOOTH);
  /* décommenter la ligne du bas pour l'activation de l'option
   * permettant de cacher les faces arrières */
  /* glEnable(GL_CULL_FACE); */
  /* décommenter la ligne du bas si inversion du sens pour face
   * avant */
  /* glFrontFace(GL_CW); */
  /* Création des matrices GL4Dummies, une pour la projection, une
   * pour la modélisation et une pour la vue */
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  gl4duGenMatrix(GL_FLOAT, "modelMatrix");
  gl4duGenMatrix(GL_FLOAT, "viewMatrix");
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
  /* Génération de deux identifiants de VBO un pour ARRAY_BUFFER, un
   * pour ELEMENT_ARRAY_BUFFER */
  glGenBuffers(2, _buffer);
  /* Lier le VBO-ARRAY_BUFFER à l'identifiant du premier VBO généré */
  glBindBuffer(GL_ARRAY_BUFFER, _buffer[0]);
  /* Transfert des données VBO-ARRAY_BUFFER */
  glBufferData(GL_ARRAY_BUFFER, sizeof data, data, GL_STATIC_DRAW);
  /* Paramétrage 2 premiers indices d'attribut de sommet */
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof *data, (const void *)0);  
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof *data, (const void *)(3 * sizeof *data));
  /* Lier le VBO-ELEMENT_ARRAY_BUFFER à l'identifiant du second VBO généré */
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffer[1]);
  /* Transfert des données d'indices VBO-ELEMENT_ARRAY_BUFFER */
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof idata, idata, GL_STATIC_DRAW);
  /* dé-lier le VAO puis les VAO */
  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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
  /* lier (mettre en avant ou "courante") la matrice vue créée dans
   * init */
  gl4duBindMatrix("viewMatrix");
  /* Charger la matrice identité dans la matrice courante (liée) */
  gl4duLoadIdentityf();
  /* Composer la matrice vue courante en simulant une "caméra" à
   * l'aide de la fonction LookAt(xyz_position_cam,
   * xyz_ou_elle_regarde, xyz_son_vecteur_haut) */
  gl4duLookAtf(3, 3, 6, 0, 0, 0, 0, 1, 0);
  /* lier (mettre en avant ou "courante") la matrice modèle créée dans
   * init */
  gl4duBindMatrix("modelMatrix");
  /* Charger la matrice identité dans la matrice courante (liée) */
  gl4duLoadIdentityf();
  /* Envoyer, au shader courant, toutes les matrices connues dans
   * GL4Dummies, ici on intègre pas la rotation qui vient après */
  gl4duSendMatrices();
  /* modifier l'épaisseur d'une ligne dessinée quand c'est supporté
   * par l'implémentation */
  glLineWidth(3.0f);
  /* Lier le VAO-machine-GL à l'identifiant VAO _vao */
  glBindVertexArray(_vao);
  /* Dessiner la partie du VAO concernant le cube unitaire comme des
   * line loop pour chaque face de 4 sommets en commençant par
   * l'indice 3 (+4 à chaque fois, sur 6 faces). Pour tester, vous
   * pouvez remplacer tous les GL_LINE_LOOP par des GL_TRIANGLE_FAN
   *
   * Attention ! Maintenant nous dessinons avec DrawElement qui
   * utilise les indices des sommets passés pour mailler. Remarquer la
   * progression des indices de 4 en 4 */
  glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (const GLvoid *)(3 * sizeof(GLuint)));
  glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (const GLvoid *)(7 * sizeof(GLuint)));
  glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (const GLvoid *)(11 * sizeof(GLuint)));
  glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (const GLvoid *)(15 * sizeof(GLuint)));
  glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (const GLvoid *)(19 * sizeof(GLuint)));
  glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (const GLvoid *)(23 * sizeof(GLuint)));

  /* Dessiner la partie du VAO concernant les 3 axes comme des line
   * strip pour chaque axe 5 sommets sont utilisés à chaque fois
   * (indice + 5 à chaque fois).
   *
   * Attention ! Maintenant nous dessinons avec DrawElement qui
   * utilise les indices des sommets passés pour mailler. Remarquer la
   * progression des indices de 5 en 5 */
  glDrawElements(GL_LINE_STRIP, 5, GL_UNSIGNED_INT, (const GLvoid *)(27 * sizeof(GLuint)));
  glDrawElements(GL_LINE_STRIP, 5, GL_UNSIGNED_INT, (const GLvoid *)(32 * sizeof(GLuint)));
  glDrawElements(GL_LINE_STRIP, 5, GL_UNSIGNED_INT, (const GLvoid *)(37 * sizeof(GLuint)));

  /* on transmet la variable d'angle en l'intégrant dans la matrice
   * modèle. Soit composer la matrice courante avec une rotation
   * "angle" autour de l'axe y (0, 1, 0) */
  gl4duRotatef(angle, 0, 1, 0);
  /* on incrémente angle d'un 1/60 de tour soit (360° x 1/60). Tester
   * l'application en activant/désactivant la synchronisation
   * verticale de votre carte graphique. Que se passe-t-il ? Trouver
   * une solution pour que résultat soit toujours le même. */
  angle += (1.0f / 60.0f) * 360.0f;
  /* Envoyer, au shader courant, toutes les matrices connues dans
   * GL4Dummies */
  gl4duSendMatrices();
  /* Dessiner le VAO comme une bande d'un triangle avec 3 sommets
   * commençant à 0
   *
   * Attention ! Maintenant nous dessinons avec DrawElement qui
   * utilise les indices des sommets poassés pour mailler */
  glDrawElements(GL_TRIANGLE_STRIP, 3, GL_UNSIGNED_INT, (const GLvoid *)0);
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
  /* suppression du VBO _buffer en GPU, maintenant il y en a deux */
  if(_buffer[0])
    glDeleteBuffers(2, _buffer);
  /* nettoyage des éléments utilisés par la bibliothèque GL4Dummies */
  gl4duClean(GL4DU_ALL);
}
