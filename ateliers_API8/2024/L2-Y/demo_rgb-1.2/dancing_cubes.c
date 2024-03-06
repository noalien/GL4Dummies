/* inclusion des entêtes de fonctions de création et de gestion de
 * fenêtres système ouvrant un contexte favorable à GL4dummies. Cette
 * partie est dépendante de la bibliothèque SDL2 */
#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4dm.h>
#include <GL4D/gl4dg.h>
#include <GL4D/gl4dp.h>
#include <GL4D/gl4dh.h>

static void init(void);
static void draw(void);
static void quit(void);
static void scene(GLuint pId, float a);

/* on créé une variable pour stocker l'identifiant du programme GPU */
static GLuint _pId = 0;
/* on créé une variable pour stocker l'identifiant de la géométrie d'un cube GL4D */
static GLuint _cube = 0;
/* on se créé 3 identifiants de texture */
static GLuint _texId[3] = { 0 };

void dancing_cubes(int state) {
  switch(state) {
  case GL4DH_INIT:
    /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    init();
    return;
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    quit();
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
    return;
  default: /* GL4DH_DRAW */
    draw();
    return;
  }

} 


/* initialise des paramètres GL et GL4D */
void init(void) {
  /* activer le BACKFACE_CULLING */
  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);
  /* on active le test de profondeur */
  glEnable(GL_DEPTH_TEST);
  /* set la couleur d'effacement OpenGL */
  glClearColor(0.7f, 0.7f, 0.0f, 1.0f);
  /* générer un cube en GL4D */
  _cube = gl4dgGenCubef();
  /* créer un programme GPU pour OpenGL (en GL4D) */
  _pId = gl4duCreateProgram("<vs>shaders/dancing_cubes.vs", "<fs>shaders/dancing_cubes.fs", NULL);
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
}

void draw(void) {
  /* une variable pour stocker un ange qui incrémente */
  static float a = 0;
  int WW, WH;
  GLint vp[4];
  /* récupérer la largeur et hauteur de la fenêtre depuis le viewport */
  glGetIntegerv(GL_VIEWPORT, vp);
  WW = vp[2];
  WH = vp[3];
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
  gl4duLookAtf(0.0f, 3.0f, 0.0f, 0.0f, 0.0f, -5.0f, 0.0f, 1.0f, 0.0f);

  scene(_pId, a);

  /* n'utiliser aucun programme GPU (pas nécessaire) */
  glUseProgram(0);
  /* augmenter l'ange a de 1 */
  a += 1;
}

/* appelée lors du exit */
void quit(void) {
  if(_texId[0]) {
    /* on supprime la texture côté OpenGL */
    glDeleteTextures(3, _texId);
    _texId[0] = 0;
  }
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
  /* mettre la matrice identité (celle qui ne change rien) dans la matrice courante */
  gl4duLoadIdentityf();
  /* composer (multiplication à droite) avec une translation dans les z négatifs (-5) */ 
  gl4duTranslatef(-2, 0, -5);
  /* composer (multiplication à droite) avec une rotation d'angle a et
     d'axe (autour de l'axe) <0, 1, 0> */
  gl4duRotatef(a, 0, 1, 0);
  /* envoyer les matrice GL4D au programme GPU OpenGL (en cours) */
  gl4duSendMatrices();
  /* set la variable "uniform" couleur pour mettre du rouge */
  glUniform4f(glGetUniformLocation(pId, "couleur"), 1.0f, 0.0f, 0.0f, 1.0f);
  /* demander le dessin d'un objet GL4D */
  gl4dgDraw(_cube);


  /* autres matrices pour le second cube */

  /* on bind la texture _texId[1] */
  glBindTexture(GL_TEXTURE_2D, _texId[1]);
  /* mettre la matrice identité (celle qui ne change rien) dans la matrice courante */
  gl4duLoadIdentityf();
  /* composer (multiplication à droite) avec une translation dans les z négatifs (-5) */ 
  gl4duTranslatef(2, 0, -5);
  /* composer (multiplication à droite) avec une rotation d'angle a et
     d'axe (autour de l'axe) <0, 1, 0> */
  gl4duRotatef(-a, 0, 1, 0);
  /* envoyer les matrice GL4D au programme GPU OpenGL (en cours) */
  gl4duSendMatrices();
  /* set la variable "uniform" couleur pour mettre du vert */
  glUniform4f(glGetUniformLocation(pId, "couleur"), 0.0f, 1.0f, 0.0f, 1.0f);
  /* demander le dessin d'un objet GL4D */
  gl4dgDraw(_cube);


  /* on bind la texture _texId[2] */
  glBindTexture(GL_TEXTURE_2D, _texId[2]);

  /* autres matrices pour un troisième cube */

  /* mettre la matrice identité (celle qui ne change rien) dans la matrice courante */
  gl4duLoadIdentityf();
  /* composer (multiplication à droite) avec une rotation d'angle a et
     d'axe (autour de l'axe) <0, 1, 0> */
  gl4duRotatef(0.5f * a, 0, 1, 0);
  /* composer (multiplication à droite) avec une translation dans les z négatifs (-5) */ 
  gl4duTranslatef(0, 0, -8);
  /* composer (multiplication à droite) avec une rotation d'angle a et
     d'axe (autour de l'axe) <0, 1, 0> */
  gl4duRotatef(-2.0f * a, 0, 1, 0);
  /* scale 1.5 * le x, 0.5 * le y, 2.0 * le z*/
  gl4duScalef(1.5f, 0.5f, 2.0f);
  /* envoyer les matrice GL4D au programme GPU OpenGL (en cours) */
  gl4duSendMatrices();
  /* set la variable "uniform" couleur pour mettre du bleu */
  glUniform4f(glGetUniformLocation(pId, "couleur"), 0.0f, 0.0f, 1.0f, 1.0f);
  /* demander le dessin d'un objet GL4D */
  gl4dgDraw(_cube);
  /* on dé-bind la texture */
  glBindTexture(GL_TEXTURE_2D, 0);
}

