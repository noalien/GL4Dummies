/*!\file window.c
 * \brief GL4Dummies, exemple simple 3D avec mobiles et GL4Dummies
 * \author Farès BELHADJ, amsi@up8.edu
 * \date February 03 2022
 */

/* inclusion des entêtes de fonctions de création et de gestion de
 * fenêtres système ouvrant un contexte favorable à GL4dummies. Cette
 * partie est dépendante de la bibliothèque SDL2 */
#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4dm.h>
#include <GL4D/gl4dg.h>
#include <GL4D/gl4dp.h>
#include "mobile.h"

#define WW 800
#define WH 600


static void init(void);
static void draw(void);
static void quit(void);
static void scene(GLuint pId, float a);

/* on créé une variable pour stocker l'identifiant du programme GPU */
GLuint _pId = 0;
/* on créé une variable pour stocker l'identifiant de la géométrie d'un cube GL4D */
GLuint _cube = 0;
/* on créé une variable pour stocker l'identifiant de la géométrie d'une sphère */
GLuint _sphere = 0;
/* on se créé 3 identifiants de texture */
GLuint _texId[3] = { 0 };

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
  /* placer mobile_simu comme fonction à appeler pour calculer la simulation */
  gl4duwIdleFunc(mobile_simu);
  /* placer draw comme fonction à appeler pour dessiner chaque frame */
  gl4duwDisplayFunc(draw);
  /* boucle infinie pour éviter que le programme ne s'arrête et ferme
   * la fenêtre immédiatement */
  gl4duwMainLoop();
  return 0;
}

/* initialise des paramètres GL et GL4D */
void init(void) {
  /* activer la synchronisation verticale */
  SDL_GL_SetSwapInterval(1);
  /* activation de la transparence */
  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
  /* activer le BACKFACE_CULLING */
  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);
  /* on active le test de profondeur */
  glEnable(GL_DEPTH_TEST);
  /* set la couleur d'effacement OpenGL */
  glClearColor(0.7f, 0.7f, 0.0f, 1.0f);
  /* générer un cube en GL4D */
  _cube = gl4dgGenCubef();
  /* générer une sphère en GL4D */
  _sphere = gl4dgGenSpheref(6, 6);
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


  /* on se créé une texture à la main, de 4 pixels Rouge, Blan, Blanc, Noir */
  GLuint pixels[] = { RGBA(255, 0, 0, 255), RGBA(255, 255, 255, 255), RGBA(255, 255, 255, 255), 0 };
  /* on génère un seul identifiant de texture côté OpenGL */
  glGenTextures(3, _texId);

  /* on bind la texture _texId[0] */
  glBindTexture(GL_TEXTURE_2D, _texId[0]);
  /* paramétrage de l'interpolation pour rapetisser ou agrandir la texture */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

  /* on bind la texture _texId[1] */
  glBindTexture(GL_TEXTURE_2D, _texId[1]);
  /* paramétrage de l'interpolation pour rapetisser ou agrandir la texture */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  /* on récupère une image depuis SDL/BMP */
  SDL_Surface * s = SDL_LoadBMP("images/A.bmp");
  assert(s);
  /* je sais que mon images est RGB */
  assert(s->format->BytesPerPixel == 3);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h, 0, GL_RGB, GL_UNSIGNED_BYTE, s->pixels);
  SDL_FreeSurface(s);

  /* on bind la texture _texId[2] */
  glBindTexture(GL_TEXTURE_2D, _texId[2]);
  /* paramétrage de l'interpolation pour rapetisser ou agrandir la texture */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  /* on récupère une image depuis SDL/BMP */
  s = SDL_LoadBMP("images/B.bmp");
  assert(s);
  /* je sais que mon images est RGBA */
  assert(s->format->BytesPerPixel == 4);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, s->pixels);
  SDL_FreeSurface(s);

  /* on dé-bind la texture 2D */
  glBindTexture(GL_TEXTURE_2D, 0);

  mobile_init(100);
}

void draw(void) {
  /* une variable pour stocker un ange qui incrémente */
  static float a = 0;
  /* set une couleur d'effacement random pour OpenGL */
  //glClearColor(gl4dmURand(), gl4dmURand(), gl4dmURand(), 1.0);
  /* effacer le buffer de couleur (image) et le buffer de profondeur d'OpenGL */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  /* utiliser le programme GPU "_pId" */
  glUseProgram(_pId);

  /****************************/
  /* VUE EN PERSPECTIVE       */
  /* dans portion bas-droite  */
  /****************************/

  /* je définis la portion de vue correspondante */
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
  gl4duLookAtf(0.5f, 1.4f, 2.2f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

  scene(_pId, a);

  /* n'utiliser aucun programme GPU (pas nécessaire) */
  glUseProgram(0);
  /* augmenter l'ange a de 1 */
  a += 1;
}

/* appelée lors du exit */
void quit(void) {
  mobile_quit();
  if(_texId[0]) {
    /* on supprime la texture côté OpenGL */
    glDeleteTextures(3, _texId);
    _texId[0] = 0;
  }
  /* nettoyer (libérer) tout objet créé avec GL4D */
  gl4duClean(GL4DU_ALL);
}

void scene(GLuint pId, float a) {
  /* on précise qu'on utilise l'étage de texture 0 */
  glActiveTexture(GL_TEXTURE0);
  /* on bind la texture _texId[0] */
  glBindTexture(GL_TEXTURE_2D, _texId[0]);
  /* on donne un nom à notre texture dans GLSL et on y met 0 car c'est l'étage où elle se trouve */
  glUniform1i(glGetUniformLocation(pId, "tex"), 0);


  /* binder (mettre au premier plan, "en courante" ou "en active") la
     matrice model */
  gl4duBindMatrix("model");
  gl4duLoadIdentityf();
  mobile_draw(pId, _sphere);


  /* on dé-bind la texture */
  glBindTexture(GL_TEXTURE_2D, 0);
  gl4duTranslatef(0.0f, 1.0f, 0.0f);
  glUniform4f(glGetUniformLocation(pId, "couleur"), 0.0f, 0.0f, 1.0f, 0.5f);
  gl4duSendMatrices();
  
  gl4dgDraw(_cube);
}

