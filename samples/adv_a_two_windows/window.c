/*!\file window.c
 *
 * \brief exemple GL4D avec gestion de deux fenêtres
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr 
 * \date June 20 2019
 */

#include <assert.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4dp.h>
#include <GL4D/gl4dg.h>
#include <GL4D/gl4duw_SDL2.h>

/* Prototypes des fonctions statiques contenues dans ce fichier C */
static void initF1(void);
static void initF2(void);
static void drawF2(void);
static void drawF1(void);
static void quit(void);

/*!\brief largeur et hauteur de la fenêtre */
static int _wW = 800, _wH = 600;
/*!\brief identifiants des GLSL programs */
static GLuint _pIdF1 = 0, _pIdF2 = 0;
/*!\brief identifiants des quadrilatères */
static GLuint _quadF1 = 0, _quadF2 = 0;
/*!\brief identifiants des textures */
static GLuint _texIdF1 = 0,  _texIdF2 = 0;

/*!\brief créé la fenêtre d'affichage, initialise GL et les données,
 * affecte les fonctions d'événements et lance la boucle principale
 * d'affichage.
 */
int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "GL4Dummies Fenêtre 1", 20, 20, 
			 _wW, _wH, GL4DW_RESIZABLE | GL4DW_SHOWN))
    return 1;
  if(!gl4duwCreateWindow(argc, argv, "GL4Dummies Fenêtre 2", 80, 80, 
			 _wW, _wH, GL4DW_RESIZABLE | GL4DW_SHOWN))
    return 1;
  atexit(quit);

  initF1();

  initF2();

  gl4duwMainLoop();
  return 0;
}
/*!\brief initialise les paramètres OpenGL et les données pour la fenêtre 1. 
 */
static void initF1(void) {
  const GLuint R = RGB(255, 0, 0), B = RGB(255, 255, 255), N = 0;
  GLuint tex[] = {
    B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, 
    B, B, N, N, N, B, B, N, B, B, B, B, B, B, B, R, B, B, B, N, N, N, B, B, 
    B, N, B, B, B, B, B, N, B, B, B, B, B, B, R, R, B, B, B, N, B, B, N, B, 
    B, N, B, B, B, B, B, N, B, B, B, B, B, R, B, R, B, B, B, N, B, B, N, B, 
    B, N, B, N, N, B, B, N, B, B, B, B, R, B, B, R, B, B, B, N, B, B, N, B, 
    B, N, B, B, N, B, B, N, B, B, B, R, R, R, R, R, R, B, B, N, B, B, N, B, 
    B, B, N, N, N, B, B, N, N, N, N, B, B, B, B, R, B, B, B, N, N, N, B, B, 
    B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B
  };
  /* on bind la Fenêtre 1 pour faire passer GL au contexte de cette fenêtre */
  gl4duwBindWindow ("GL4Dummies Fenêtre 1");
  /* on dit quelle fonction appeler pour dessiner dans cette fenêtre
     (les fonctions de display se font automatiquement dans le bon
     contexte) */
  gl4duwDisplayFunc(drawF1);
  /* Création du programme shader de la fenêtre 1 (voir le dossier shader) */
  _pIdF1 = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic.fs", NULL);
  /* générer un identifiant de texture */
  glGenTextures(1, &_texIdF1);
  /* lier l'identifiant de texture comme texture 2D (1D ou 3D
   * possibles) */
  glBindTexture(GL_TEXTURE_2D, _texIdF1);
  /* paramétrer la texture, voir la doc de la fonction glTexParameter
   * sur
   * https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexParameter.xhtml */
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  /* envoi de la donnée texture depuis la RAM CPU vers la RAM GPU voir
   * la doc de glTexImage2D (voir aussi glTexImage1D et glTexImage3D)
   * sur
   * https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml */
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 24, 8, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex);
  /* dé-lier la texture 2D */
  glBindTexture(GL_TEXTURE_2D, 0);
  /* génération du quadrilatère */
  _quadF1 = gl4dgGenQuadf();
  /* Set de la couleur (RGBA) d'effacement OpenGL */
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
  /* activation de la texture 2D */
  glEnable(GL_TEXTURE_2D);
  /* activation du test de profondeur afin de prendre en compte la
   * notion de devant-derrière. */
  glEnable(GL_DEPTH_TEST);
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
}
/*!\brief initialise les paramètres OpenGL et les données pour la fenêtre 2. 
 */
static void initF2(void) {
  const GLuint R = RGB(255, 0, 0), B = RGB(255, 255, 255), N = 0;
  GLuint tex[] = {
    B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, 
    B, B, N, N, N, B, B, N, B, B, B, B, B, B, B, R, B, B, B, N, N, N, B, B, 
    B, N, B, B, B, B, B, N, B, B, B, B, B, B, R, R, B, B, B, N, B, B, N, B, 
    B, N, B, B, B, B, B, N, B, B, B, B, B, R, B, R, B, B, B, N, B, B, N, B, 
    B, N, B, N, N, B, B, N, B, B, B, B, R, B, B, R, B, B, B, N, B, B, N, B, 
    B, N, B, B, N, B, B, N, B, B, B, R, R, R, R, R, R, B, B, N, B, B, N, B, 
    B, B, N, N, N, B, B, N, N, N, N, B, B, B, B, R, B, B, B, N, N, N, B, B, 
    B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B
  };
  /* on bind la Fenêtre 1 pour faire passer GL au contexte de cette fenêtre */
  gl4duwBindWindow ("GL4Dummies Fenêtre 2");
  /* on dit quelle fonction appeler pour dessiner dans cette fenêtre
     (les fonctions de display se font automatiquement dans le bon
     contexte) */
  gl4duwDisplayFunc(drawF2);
  /* Création du programme shader de la fenêtre 2 (voir le dossier shader) */
  /* Pour Nour : ici si je remets basic.vs et/ou basic.fs le shader ne
     marche plus, l'idée est que ça différencie tout car nous ne
     sommes pas dans le même contexte opengl (gl4duwBindWindow change
     le contexte). Au passage tu peux te servir du nom de la fenêtre
     comme id "unique" supplémentaire, c'est toi qui voit. */
  _pIdF2 = gl4duCreateProgram("<vs>shaders/basic2.vs", "<fs>shaders/basic2.fs", NULL);
  /* générer un identifiant de texture */
  glGenTextures(1, &_texIdF2);
  /* lier l'identifiant de texture comme texture 2D (1D ou 3D
   * possibles) */
  glBindTexture(GL_TEXTURE_2D, _texIdF2);
  /* paramétrer la texture, voir la doc de la fonction glTexParameter
   * sur
   * https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexParameter.xhtml */
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  /* envoi de la donnée texture depuis la RAM CPU vers la RAM GPU voir
   * la doc de glTexImage2D (voir aussi glTexImage1D et glTexImage3D)
   * sur
   * https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml */
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 24, 8, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex);
  /* dé-lier la texture 2D */
  glBindTexture(GL_TEXTURE_2D, 0);
  /* génération du quadrilatère */
  _quadF2 = gl4dgGenQuadf();
  /* Set de la couleur (RGBA) d'effacement OpenGL */
  glClearColor(0.7f, 0.2f, 0.2f, 1.0f);
  /* activation de la texture 2D */
  glEnable(GL_TEXTURE_2D);
  /* activation du test de profondeur afin de prendre en compte la
   * notion de devant-derrière. */
  glEnable(GL_DEPTH_TEST);
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
}
/*!\brief Cette fonction dessine dans le contexte de la fenêtre 1. */
static void drawF1(void) {
  /* une variable d'angle, maintenant elle passe ne degrés */
  static GLfloat angle = 0.0f;
  /* effacement du buffer de couleur, nous rajoutons aussi le buffer
   * de profondeur afin de bien rendre les fragments qui sont devant
   * au dessus de ceux qui sont derrière. Commenter le
   * "|GL_DEPTH_BUFFER_BIT" pour voir la différence. Nous avons ajouté
   * l'activation du test de profondeur dans la fonction init via
   * l'appel glEnable(GL_DEPTH_TEST). */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  /* activation du programme _pIdF1 */
  glUseProgram(_pIdF1);
  /* lier (mettre en avant ou "courante") la matrice vue créée dans
   * init */
  gl4duBindMatrix("viewMatrix");
  /* Charger la matrice identité dans la matrice courante (liée) */
  gl4duLoadIdentityf();
  /* Composer la matrice vue courante en simulant une "caméra" à
   * l'aide de la fonction LookAt(xyz_position_cam,
   * xyz_ou_elle_regarde, xyz_son_vecteur_haut) */
  gl4duLookAtf(0, 0, 3, 0, 0, 0, 0, 1, 0);
  /* lier (mettre en avant ou "courante") la matrice modèle créée dans
   * init */
  gl4duBindMatrix("modelMatrix");
  /* Charger la matrice identité dans la matrice courante (liée) */
  gl4duLoadIdentityf();
  /* on transmet la variable d'angle en l'intégrant dans la matrice
   * modèle. Soit composer la matrice courante avec une rotation
   * "angle" autour de l'axe y (0, 1, 0) */
  gl4duRotatef(angle, 0, 1, 0);
  /* on incrémente angle d'un 1/60 de 1/4 de tour soit (360° x 1/60). Tester
   * l'application en activant/désactivant la synchronisation
   * verticale de votre carte graphique. Que se passe-t-il ? Trouver
   * une solution pour que résultat soit toujours le même. */
  angle += 0.25f * (1.0f / 60.0f) * 360.0f;
  /* Envoyer, au shader courant, toutes les matrices connues dans
   * GL4Dummies */
  gl4duSendMatrices();
  /* activer l'étage de textures 0, plusieurs étages sont disponibles,
   * nous pouvons lier une texture par type et par étage */
  glActiveTexture(GL_TEXTURE0);
  /* lier la texture _texIdF1 comme texture 2D */
  glBindTexture(GL_TEXTURE_2D, _texIdF1);
  /* envoyer une info au program shader indiquant que tex est une
   * texture d'étage 0, voir le type (sampler2D) de la variable tex
   * dans le shader */
  glUniform1i(glGetUniformLocation(_pIdF1, "tex"), 0);
  /* envoi d'un booléen pour inverser l'axe y des coordonnées de
   * textures (plus efficace à faire dans le vertex shader */
  glUniform1i(glGetUniformLocation(_pIdF1, "inv"), 1);
  /* dessin du quadrilatère */
  gl4dgDraw(_quadF1);
  /* désactiver le programme shader */
  glUseProgram(0);
}
/*!\brief Cette fonction dessine dans le contexte de la fenêtre 2. */
static void drawF2(void) {
  /* une variable d'angle, maintenant elle passe ne degrés */
  static GLfloat angle = 0.0f;
  /* effacement du buffer de couleur, nous rajoutons aussi le buffer
   * de profondeur afin de bien rendre les fragments qui sont devant
   * au dessus de ceux qui sont derrière. Commenter le
   * "|GL_DEPTH_BUFFER_BIT" pour voir la différence. Nous avons ajouté
   * l'activation du test de profondeur dans la fonction init via
   * l'appel glEnable(GL_DEPTH_TEST). */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  /* activation du programme _pIdF2 */
  glUseProgram(_pIdF2);
  /* lier (mettre en avant ou "courante") la matrice vue créée dans
   * init */
  gl4duBindMatrix("viewMatrix");
  /* Charger la matrice identité dans la matrice courante (liée) */
  gl4duLoadIdentityf();
  /* Composer la matrice vue courante en simulant une "caméra" à
   * l'aide de la fonction LookAt(xyz_position_cam,
   * xyz_ou_elle_regarde, xyz_son_vecteur_haut) */
  gl4duLookAtf(0, 0, 3, 0, 0, 0, 0, 1, 0);
  /* lier (mettre en avant ou "courante") la matrice modèle créée dans
   * init */
  gl4duBindMatrix("modelMatrix");
  /* Charger la matrice identité dans la matrice courante (liée) */
  gl4duLoadIdentityf();
  /* on transmet la variable d'angle en l'intégrant dans la matrice
   * modèle. Soit composer la matrice courante avec une rotation
   * "angle" autour de l'axe y (0, 1, 0) */
  gl4duRotatef(angle, 1, 0, 0);
  /* on incrémente angle d'un 1/60 de 1/4 de tour soit (360° x 1/60). Tester
   * l'application en activant/désactivant la synchronisation
   * verticale de votre carte graphique. Que se passe-t-il ? Trouver
   * une solution pour que résultat soit toujours le même. */
  angle += 0.25f * (1.0f / 60.0f) * 360.0f;
  /* Envoyer, au shader courant, toutes les matrices connues dans
   * GL4Dummies */
  gl4duSendMatrices();
  /* activer l'étage de textures 0, plusieurs étages sont disponibles,
   * nous pouvons lier une texture par type et par étage */
  glActiveTexture(GL_TEXTURE0);
  /* lier la texture _texIdF2 comme texture 2D */
  glBindTexture(GL_TEXTURE_2D, _texIdF2);
  /* envoyer une info au program shader indiquant que tex est une
   * texture d'étage 0, voir le type (sampler2D) de la variable tex
   * dans le shader */
  glUniform1i(glGetUniformLocation(_pIdF2, "tex"), 0);
  /* envoi d'un booléen pour inverser l'axe y des coordonnées de
   * textures (plus efficace à faire dans le vertex shader */
  glUniform1i(glGetUniformLocation(_pIdF2, "inv"), 1);
  /* dessin du quadrilatère */
  gl4dgDraw(_quadF2);
  /* désactiver le programme shader */
  glUseProgram(0);
}
/*!\brief appelée au moment de sortir du programme (atexit), elle
 *  libère les éléments OpenGL utilisés.*/
static void quit(void) {
  /* suppression de la texture _texIdF1 en GPU */
  if(_texIdF1) {
    glDeleteTextures(1, &_texIdF1);
    _texIdF1 = 0;
  }
  /* suppression de la texture _texIdF2 en GPU */
  if(_texIdF2) {
    glDeleteTextures(1, &_texIdF2);
    _texIdF2 = 0;
  }
  /* nettoyage des éléments utilisés par la bibliothèque GL4Dummies */
  gl4duClean(GL4DU_ALL);
}
