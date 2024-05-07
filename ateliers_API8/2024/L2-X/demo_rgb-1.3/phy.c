#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4dm.h>
#include <GL4D/gl4dg.h>
#include <GL4D/gl4dh.h>
#include <GL4D/gl4df.h>
#include <SDL_image.h>
#include "mobile.h"
#include "audioHelper.h"

static void init(void);
static void draw(GLfloat a0);
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

void phy(int state) {
  static float volumes[4] = { 0.0f }, volume = 0.0f;
  switch(state) {
  case GL4DH_INIT:
    init();
    return;
  case GL4DH_FREE:
    quit();
    return;
  case GL4DH_UPDATE_WITH_AUDIO: {
    int i, l = ahGetAudioStreamLength() / 2;
    float moyenne;
    Sint16 * stream = (Sint16 *)ahGetAudioStream();
    for(i = 0, moyenne = 0.0f; i < l; ++i) {
      moyenne += abs(stream[i]) / (float)(1 << 15);
    }
    moyenne /= l;
    for(i = 3; i > 0; --i)
      volumes[i - 1] = volumes[i];
    volume = pow(moyenne, 0.5f);
    volumes[3] = volume;
    return;
  }
  default: /* GL4DH_DRAW */
    mobile_simu();
    draw(0.012f * volumes[0] + 0.122f * volumes[1] + 0.484f * volumes[2] + 0.766f * volumes[3]);
    return;
  }
}


/* initialise des paramètres GL et GL4D */
void init(void) {
  /* paramétrer le mélange pour le calcul de la transparence */
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  /* dire lesquelles, des faces avant ou arrières, je cache (to cull) */
  glCullFace(GL_BACK); /* GL_BACK est le par défaut */
  /* activer le CULL_FACE (est désactivé par défaut) */
  glEnable(GL_CULL_FACE);
  /* générer un plan en GL4D */
  _quad = gl4dgGenQuadf();
  /* générer un cube en GL4D */
  _cube = gl4dgGenCubef();
  /* générer un tore en GL4D */
  _tore = gl4dgGenTorusf(10, 10, 0.45f);
  /* créer un programme GPU pour OpenGL (en GL4D) */
  _pId = gl4duCreateProgram("<vs>shaders/phy.vs", "<fs>shaders/phy.fs", NULL);
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

void draw(GLfloat a0) {
  /* une variable pour stocker un angle qui incrémente */
  static float a = 0;
  GLboolean dt = glIsEnabled(GL_DEPTH_TEST);
  /* activer le test de profondeur si désactivé */
  if(!dt)
    glEnable(GL_DEPTH_TEST);
  /* set la couleur d'effacement OpenGL */
  glClearColor(0.0f, 0.0f, 0.5f, 1.0f);


  /* set une couleur d'effacement random pour OpenGL */
  //glClearColor(gl4dmURand(), gl4dmURand(), gl4dmURand(), 1.0);
  /* effacer le buffer de couleur (image) et le buffer de profondeur d'OpenGL */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  /* utiliser le programme GPU "_pId" */
  glUseProgram(_pId);

  GLint WW, WH, vp[4];
  glGetIntegerv(GL_VIEWPORT, vp);
  WW = vp[2];
  WH = vp[3];
  
  /*********************/
  /****** VUE I-D ******/
  /*********************/

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
  GLfloat angle = a0 + a * M_PI / 180.0f;
  gl4duLookAtf(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -3.0f, -sin(angle), cos(angle), 0.0f);
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
  /* remettre à Disable le depth_test s'il était désactivé */
  if(!dt)
    glDisable(GL_DEPTH_TEST);
}

/* appelée lors du exit */
void quit(void) {
  mobile_quit();
  if(_texId) {
    glDeleteTextures(1, &_texId);
    _texId = 0;
  }
}

void scene(GLuint pId, float a) {
  GLboolean bl = glIsEnabled(GL_BLEND);
  /* binder (mettre au premier plan, "en courante" ou "en active") la
     matrice model */
  gl4duBindMatrix("model");

  gl4duLoadIdentityf();
  gl4duTranslatef(0.0f, 0.0f, -3.0f);
  gl4duRotatef(25.0f, 0.0f, 1.0f, 0.0f);
  mobile_draw(pId, _tore);

  GLint tId;
  glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &tId);
  //printf("%d\n", tId); exit(0);
  gl4dfBlur(tId, tId, 50, 1, 0, GL_FALSE);
  //return;
  /* LE CUBE */
  /* activer la transparence si désactivée */
  if(!bl)
    glEnable(GL_BLEND);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _texId);
  glUniform1i(glGetUniformLocation(pId, "tex"), 0); /* le 0 correspond à glActiveTexture(GL_TEXTURE0) */
  glUniform1i(glGetUniformLocation(pId, "has_tex"), 1); 
  /* envoyer les matrice GL4D au programme GPU OpenGL (en cours) */
  gl4duSendMatrices();
  /* set la variable "uniform" couleur pour mettre du rouge */
  glUniform4f(glGetUniformLocation(pId, "couleur"), 1.0f, 1.0f, 1.0f, 0.5f);
  /* demander le dessin d'un objet GL4D */
  gl4dgDraw(_cube);
  glBindTexture(GL_TEXTURE_2D, 0);
  glUniform1i(glGetUniformLocation(pId, "has_tex"), 0); 
  /* activer la transparence */
  /* remettre à Disable le blending s'il était désactivé */
  if(!bl)
    glDisable(GL_BLEND);
}
  
