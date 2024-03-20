/*!\file window.c
 * \brief GL4Dummies, exemple simple 3D avec GL4Dummies
 * \author Farès BELHADJ, amsi@up8.edu
 * \date February 03 2022
 */

/* inclusion des entêtes de fonctions de création et de gestion de
 * fenêtres système ouvrant un contexte favorable à GL4dummies. Cette
 * partie est dépendante de la bibliothèque SDL2 */
#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4dm.h>
#include <GL4D/gl4dg.h>
#include <SDL_image.h>
#include "mobile.h"

#define WW 800
#define WH 600


static void init(void);
static void draw(void);
static void quit(void);
static void scene(GLuint pId, float a);

/* on créé une variable pour stocker l'identifiant du programme GPU */
static GLuint _pId = 0;
/* on créé une variable pour stocker l'identifiant d'une texture */
static GLuint _texId = 0;
/* on créé une variable pour stocker l'identifiant de la géométrie d'un cube GL4D */
static GLuint _cube = 0;
/* on créé une variable pour stocker l'identifiant de la géométrie d'un tore GL4D */
static GLuint _tore = 0;
/* on créé une variable pour stocker l'identifiant de la géométrie d'un quadrilatère GL4D */
static GLuint _quad = 0;

/*!\brief créé la fenêtre, un screen 2D effacé en noir et lance une
 *  boucle infinie.*/
int main(int argc, char ** argv) {
  /* tentative de création d'une fenêtre pour GL4Dummies */
  if(!gl4duwCreateWindow(argc, argv, /* args du programme */
			 "GL4Dummies' Hello World", /* titre */
			 10, 10, WW, WH, /* x,y, largeur, heuteur */
			 GL4DW_SHOWN) /* état visible */) {
    /* ici si échec de la création souvent lié à un problème d'absence
     * de contexte graphique ou d'impossibilité d'ouverture d'un
     * contexte OpenGL (au moins 3.2) */
    return 1;
  }
  /* appeler init pour initialiser des paramètres GL et GL4D */
  init();
  /* placer quit comme fonction à appeler au moment du exit */
  atexit(quit);
  /* placer draw comme fonction à appeler pour dessiner chaque frame */
  gl4duwDisplayFunc(draw);
  /* calculs physiques (simulation) */
  gl4duwIdleFunc(mobile_simu);
  /* boucle infinie pour éviter que le programme ne s'arrête et ferme
   * la fenêtre immédiatement */
  gl4duwMainLoop();
  return 0;
}

/* initialise des paramètres GL et GL4D */
void init(void) {
  /* activer la synchronisation verticale */
  SDL_GL_SetSwapInterval(1);
  /* activer le test de profondeur */
  glEnable(GL_DEPTH_TEST);
  /* activer la transparence */
  glEnable(GL_BLEND);
  /* paramétrer le mélange pour le calcul de la transparence */
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  /* set la couleur d'effacement OpenGL */
  glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
  /* dire lesquelles, des faces avant ou arrières, je cache (to cull) */
  glCullFace(GL_BACK); /* GL_BACK est le par défaut */
  /* activer le CULL_FACE (est désactivé par défaut) */
  glEnable(GL_CULL_FACE);
  /* générer un plan en GL4D */
  _quad = gl4dgGenQuadf();
  /* générer un cube en GL4D */
  /* essai fractales */
  /* GLfloat * hm = gl4dmTriangleEdge (513, 513, 0.4f); */
  /* _cube = gl4dgGenGrid2dFromHeightMapf (513, 513, hm);//gl4dgGenCubef(); */
  /* free(hm); */
  _cube = gl4dgGenCubef();
  /* générer un tore en GL4D */
  _tore = gl4dgGenTorusf(10, 10, 0.45f);
  /* créer un programme GPU pour OpenGL (en GL4D) */
  _pId = gl4duCreateProgram("<vs>shaders/hello.vs", "<fs>shaders/hello.fs", NULL);
  /* créer dans GL4D une matrice qui s'appelle model ; matrice de
     modélisation qu'on retrouvera dans le vertex shader */
  gl4duGenMatrix(GL_FLOAT, "model");
  /* créer dans GL4D une matrice qui s'appelle view ; matrice de
     vue qu'on retrouvera dans le vertex shader */
  gl4duGenMatrix(GL_FLOAT, "view");
  /* créer dans GL4D une matrice qui s'appelle proj ; matrice de
     projection qu'on retrouvera dans le vertex shader */
  gl4duGenMatrix(GL_FLOAT, "proj");

  mobile_init(200);

  /* création d'une texture à partir d'un fichier */
  SDL_Surface * s = IMG_Load("images/sol.png");
  GLubyte p[] = {255, 0, 0, 255};
  glGenTextures(1, &_texId);
  glBindTexture(GL_TEXTURE_2D, _texId);
  /* paramétrage de l'interpolation pour rapetisser ou agrandir la texture */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  if(s == NULL) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, p);
  } else {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, s->pixels);
    SDL_FreeSurface(s);
  }

  /* on dé-bind la texture */
  glBindTexture(GL_TEXTURE_2D, 0);
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
  glViewport(0, 0, WW, WH);

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
  /* gl4duLookAtf(0.0f, 2.0f, 0.0f, 8.0f * cos(M_PI * (a - 18) / 180.0f), 0, -7.0f, 0.0f, 1.0f, 0.0f); */
  GLfloat angle = a * M_PI / 180.0f;
  gl4duLookAtf(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -3.0f, -sin(angle), cos(angle), 0.0f);
  //gl4duLookAtf(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -3.0f, 0.0f, 1.0f, 0.0f);
  mobile_update_g(M_PI +  angle);
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
    a += 30.0f * dt;
  }
}

/* appelée lors du exit */
void quit(void) {
  mobile_quit();
  if(_texId) {
    glDeleteTextures(1, &_texId);
    _texId = 0;
  }
  /* nettoyer (libérer) tout objet créé avec GL4D */
  gl4duClean(GL4DU_ALL);
}

void scene(GLuint pId, float a) {
  /* binder (mettre au premier plan, "en courante" ou "en active") la
     matrice model */
  gl4duBindMatrix("model");

  gl4duLoadIdentityf();
  gl4duTranslatef(0.0f, 0.0f, -3.0f);
  mobile_draw(pId, _tore);

  //return;
  /* LE CUBE */
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _texId);
  glUniform1i(glGetUniformLocation(pId, "tex"), 0); /* le 0 correspond à glActiveTexture(GL_TEXTURE0) */
  glUniform1i(glGetUniformLocation(pId, "has_tex"), 1); 
  gl4duRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
  /* envoyer les matrice GL4D au programme GPU OpenGL (en cours) */
  gl4duSendMatrices();
  /* set la variable "uniform" couleur pour mettre du rouge */
  glUniform4f(glGetUniformLocation(pId, "couleur"), 1.0f, 1.0f, 1.0f, 0.5f);
  /* demander le dessin d'un objet GL4D */
  gl4dgDraw(_cube);
  glBindTexture(GL_TEXTURE_2D, 0);
  glUniform1i(glGetUniformLocation(pId, "has_tex"), 0); 
}
  
