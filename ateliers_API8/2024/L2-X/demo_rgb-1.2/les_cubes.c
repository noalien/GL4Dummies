/* inclusion des entêtes de fonctions de création et de gestion de
 * fenêtres système ouvrant un contexte favorable à GL4dummies. Cette
 * partie est dépendante de la bibliothèque SDL2 */
#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4dm.h>
#include <GL4D/gl4dg.h>
#include <GL4D/gl4dh.h>

#define WW 1024
#define WH 768


static void init(void);
static void draw(void);
static void scene(GLuint pId, float a);

/* on créé une variable pour stocker l'identifiant du programme GPU */
GLuint _pId = 0;
/* on créé une variable pour stocker l'identifiant de la géométrie d'un cube GL4D */
GLuint _cube = 0;

void les_cubes(int state) {
  /* INITIALISEZ VOS VARIABLES */
  /* ... */
  switch(state) {
  case GL4DH_INIT:
    /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    init();
    return;
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
    return;
  default: /* GL4DH_DRAW */
    /* JOUER L'ANIMATION */
    draw();
    return;
  }
}

/* initialise des paramètres GL et GL4D */
void init(void) {
  /* activer la synchronisation verticale */
  SDL_GL_SetSwapInterval(1);
  /* activerr le test de profondeur */
  glEnable(GL_DEPTH_TEST);
  /* set la couleur d'effacement OpenGL */
  glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
  /* dire lesquelles, des faces avant ou arrières, je cache (to cull) */
  glCullFace(GL_BACK); /* GL_BACK est le par défaut */
  /* activer le CULL_FACE (est désactivé par défaut) */
  glEnable(GL_CULL_FACE);
  /* générer un cube en GL4D */
  _cube = gl4dgGenCubef();
  /* créer un programme GPU pour OpenGL (en GL4D) */
  _pId = gl4duCreateProgram("<vs>shaders/les_cubes.vs", "<fs>shaders/les_cubes.fs", NULL);
  /* créer dans GL4D une matrice qui s'appelle model ; matrice de
     modélisation qu'on retrouvera dans le vertex shader */
  gl4duGenMatrix(GL_FLOAT, "model");
  /* créer dans GL4D une matrice qui s'appelle view ; matrice de
     vue qu'on retrouvera dans le vertex shader */
  gl4duGenMatrix(GL_FLOAT, "view");
  /* créer dans GL4D une matrice qui s'appelle proj ; matrice de
     projection qu'on retrouvera dans le vertex shader */
  gl4duGenMatrix(GL_FLOAT, "proj");
}

void draw(void) {
  /* une variable pour stocker un angle qui incrémente */
  static float a = 0;
  /* set une couleur d'effacement random pour OpenGL */
  //glClearColor(gl4dmURand(), gl4dmURand(), gl4dmURand(), 1.0);
  /* effacer le buffer de couleur (image) et le buffer de profondeur d'OpenGL */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  /* utiliser le programme GPU "_pId" */
  glUseProgram(_pId);


  /*********************/
  /****** VUE I-D ******/
  /*********************/
  /* coin inférieur-droit de la portion de vue */
  /* glViewport(0, 0, WW, WH); */

  /* binder (mettre au premier plan, "en courante" ou "en active") la matrice proj */
  gl4duBindMatrix("proj");
  /* mettre la matrice identité (celle qui ne change rien) dans la matrice courante */
  gl4duLoadIdentityf();
  /* combiner la matrice courante avec une matrice de projection en
     perspective. Voir le support de cours pour les six paramètres :
     left, right, bottom, top, near, far */
  gl4duFrustumf(-1, 1, -(1.0f * WH) / WW, (1.0f * WH) / WW, 1, 1000);

  /* binder (mettre au premier plan, "en courante" ou "en active") la
     matrice view */
  gl4duBindMatrix("view");
  /* mettre la matrice identité (celle qui ne change rien) dans la matrice courante */
  gl4duLoadIdentityf();
  /* simule l'observateur de la scène (voir gluLookAt) */
  gl4duLookAtf(0.0f, 2.0f, 0.0f, 8.0f * cos(M_PI * (a - 18) / 180.0f), 0, -7.0f, 0.0f, 1.0f, 0.0f);

  scene(_pId, a);


  /*********************/
  /******   FIN   ******/
  /*********************/

  /* n'utiliser aucun programme GPU (pas nécessaire) */
  glUseProgram(0);
  
  /* augmenter l'angle a de 90° par seconde */
  {
    static double t0 = 0;
    double t = gl4dGetElapsedTime(), dt = (t - t0) / 1000.0;
    t0 = t;
    a += 90.0f * dt;
  }
}


void scene(GLuint pId, float a) {
  /* binder (mettre au premier plan, "en courante" ou "en active") la
     matrice model */
  gl4duBindMatrix("model");

  /****************/
  /* PREMIER CUBE */
  /****************/
  /* Couleur du cube */
  glUniform4f(glGetUniformLocation(pId, "couleur"), 1.0f, 0.0f, 0.0f, 1.0f);
  /* mettre la matrice identité (celle qui ne change rien) dans la matrice courante */
  gl4duLoadIdentityf();
  /* composer (multiplication à droite) avec une translation dans les z négatifs (-5) */ 
  gl4duTranslatef(-2, 0, -5);
  /* composer (multiplication à droite) avec une rotation d'angle a et
     d'axe (autour de l'axe) <0, 1, 0> */
  gl4duRotatef(a, 0, 1, 0);
  /* envoyer les matrice GL4D au programme GPU OpenGL (en cours) */
  gl4duSendMatrices();
  /* demander le dessin d'un objet GL4D */
  gl4dgDraw(_cube);


  /*****************/
  /* DEUXIEME CUBE */
  /*****************/
  /* Couleur du cube */
  glUniform4f(glGetUniformLocation(pId, "couleur"), 0.0f, 1.0f, 0.0f, 1.0f);
  /* autres matrices pour le second cube */

  /* mettre la matrice identité (celle qui ne change rien) dans la matrice courante */
  gl4duLoadIdentityf();
  /* composer (multiplication à droite) avec une translation dans les z négatifs (-5) */ 
  gl4duTranslatef(2, 0, -5);
  /* composer (multiplication à droite) avec une rotation d'angle a et
     d'axe (autour de l'axe) <0, 1, 0> */
  gl4duRotatef(-a, 0, 1, 0);
  /* envoyer les matrice GL4D au programme GPU OpenGL (en cours) */
  gl4duSendMatrices();
  /* demander le dessin d'un objet GL4D */
  gl4dgDraw(_cube);


  /******************/
  /* TROISIEME CUBE */
  /******************/
  /* Couleur du cube */
  glUniform4f(glGetUniformLocation(pId, "couleur"), 0.0f, 0.0f, 1.0f, 1.0f);
  /* autres matrices pour le second cube */

  /* mettre la matrice identité (celle qui ne change rien) dans la matrice courante */
  gl4duLoadIdentityf();
  /* composer (multiplication à droite) avec une translation dans les z négatifs (-7), et oscillation sur les x */ 
  gl4duTranslatef(8.0f * cos(M_PI * a / 180.0f), 0, -7.0f);
  /* composer (multiplication à droite) avec une rotation d'angle a et
     d'axe (autour de l'axe) <0, 1, 0> */
  gl4duRotatef(-3.0f * a, 0, 1, 0);
  /* envoyer les matrice GL4D au programme GPU OpenGL (en cours) */
  gl4duSendMatrices();
  /* demander le dessin d'un objet GL4D */
  gl4dgDraw(_cube);
}
  
