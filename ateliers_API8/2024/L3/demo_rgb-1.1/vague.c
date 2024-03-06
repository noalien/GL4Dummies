#include <GL4D/gl4du.h>
#include <GL4D/gl4dh.h>
#include <GL4D/gl4duw_SDL2.h>
#include "audioHelper.h"

/* Prototypes des fonctions statiques contenues dans ce fichier C */
static void init(void);
static void draw(void);

/*!\brief identifiant du (futur) GLSL program */
static GLuint _pId = 0;
/*!\brief identifiant pour une géométrie GL4D */
static GLuint _quadId = 0;


static float _volume = 0.0f;
static void  _update_volume(void) {
  int l = ahGetAudioStreamLength() / 2; /* car 16 bits */
  Sint16 * s = (Sint16 *)ahGetAudioStream();
  float moy = 0.0f;
  for(int i = 0; i < l; ++i)
    moy += abs(s[i]) / ((1 << 15) - 1.0f);
  moy /= l;
  moy = pow(moy, 0.5f);
  _volume = moy;
}
static float _get_volume(void) {
  return _volume;
}

void vague(int state) {
  switch(state) {
  case GL4DH_INIT:
    init();
    return;
  case GL4DH_FREE:
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    _update_volume();
    return;
  default: /* GL4DH_DRAW */
    draw();
    return;
  }
}

/*!\brief initialise les paramètres OpenGL et les données. */
static void init(void) {
  /* Création du programme shader (voir le dossier shader) */
  _pId = gl4duCreateProgram("<vs>shaders/vague.vs", "<fs>shaders/vague.fs", NULL);
  /* Créer un quadtrilatère */
  _quadId = gl4dgGenQuadf();
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
}
/*!\brief Cette fonction dessine dans le contexte OpenGL actif. */
static void draw(void) {
  static float angle = 0.0f;
  static double t0 = 0;
  double t = gl4dGetElapsedTime(), dt = (t - t0) / 1000.0;
  t0 = t;
  /* Set de la couleur (RGBA) d'effacement OpenGL */
  glClearColor(0.9f, 0.9f, 0.2f, 1.0f);
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
  gl4duLookAtf(0, 3, 6, 0, 0, 0, 0, 1, 0);
  /* lier (mettre en avant ou "courante") la matrice modèle créée dans
   * init */
  gl4duBindMatrix("modelMatrix");
  /* Charger la matrice identité dans la matrice courante (liée) */
  gl4duLoadIdentityf();
  /* avance vers z positifs */
  gl4duTranslatef(0.0f, 0.0f, 2.0f);
  /* faire une rotation autour de l'axe y */
  gl4duRotatef(-30.0f + 45.0f * cos(M_PI * angle / 180.0), 1.0f, 0.0f, 0.0f);

  //gl4duScalef(1.0f, 1.0, 1.0f);
  /* Envoyer, au shader courant, toutes les matrices connues dans
   * GL4Dummies, ici on intègre pas la rotation qui vient après */
  gl4duSendMatrices();

  glUniform1f(glGetUniformLocation(_pId, "temps"), _get_volume() + t / 2000.0);
  
  /* dessiner la géométrie */
  gl4dgDraw(_quadId);
  /* désactiver le programme shader */
  glUseProgram(0);

  /* un tour par seconde */
  angle += 72.0f * dt;
}
