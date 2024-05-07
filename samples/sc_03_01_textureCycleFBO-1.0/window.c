/*!\file window.c
 *
 * \brief texture avec cycle (sortie vers entrée) en utilisant le FBO 
 * \author Farès BELHADJ, amsi@up8.edu
 * \date July 15 2020
 */

#include <GL4D/gl4du.h>
#include <GL4D/gl4dg.h>
#include <GL4D/gl4duw_SDL2.h>
#include <stdlib.h>
#include <assert.h>

/* Prototypes des fonctions statiques contenues dans ce fichier C */
static void init(void);
static void draw(void);
static void quit(void);

/*!\brief largeur et hauteur de la fenêtre */
static int _wW = 800, _wH = 600;
/*!\brief identifiant du GLSL program */
static GLuint _pId = 0;
/*!\brief identifiant pour un quadrilatère GL4Dummies */
static GLuint _quad = 0;
/*!\brief identifiant du FBO et des textures E/S */
static GLuint _fbo, _fboTex[2];

/*!\brief créé la fenêtre d'affichage, initialise GL et les données,
 * affecte les fonctions d'événements et lance la boucle principale
 * d'affichage.
 */
int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "GL4Dummies", 20, 20, 
			 _wW, _wH, GL4DW_SHOWN))
    return 1;
  SDL_GL_SetSwapInterval(1);
  init();
  atexit(quit);
  gl4duwDisplayFunc(draw);
  gl4duwMainLoop();
  return 0;
}
/*!\brief initialise les paramètres OpenGL et les données. 
 */
static void init(void) {
  int i;
  GLuint * tex = malloc(_wW * _wH * sizeof *tex);
  assert(tex);
  /* remplir tex de valeurs aléatoires */
  for(i = 0; i < _wW * _wH; ++i)
    tex[i] = rand();
  /* Génération d'un framebuffer pour pouvoir passer d'une texture à l'autre */
  glGenFramebuffers(1, &_fbo);
  /* générer deux identifiants de texture */
  glGenTextures(2, _fboTex);
  for(i = 0; i < 2; ++i) {
    /* lier un identifiant de texture comme texture 2D (1D ou 3D
     * possibles) */
    glBindTexture(GL_TEXTURE_2D, _fboTex[i]);
    /* paramétrer la texture, voir la doc de la fonction glTexParameter
     * sur
     * https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexParameter.xhtml */
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    /* envoi de la donnée texture depuis la RAM CPU vers la RAM GPU voir
     * la doc de glTexImage2D (voir aussi glTexImage1D et glTexImage3D)
     * sur
     * https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml */
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _wW, _wH, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex);
  }
  /* plus besoin de la texture en CPU */
  free(tex);
  /* dé-lier la texture 2D */
  glBindTexture(GL_TEXTURE_2D, 0);

  /* Création du programme shader (voir le dossier shader) */
  _pId = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic.fs", NULL);
  /* Création du quadrilatère */
  _quad = gl4dgGenQuadf();
  /* Set de la couleur (RGBA) d'effacement OpenGL */
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
  /* activation de la texture 2D */
  glEnable(GL_TEXTURE_2D);
  /* dans quelle partie de l'écran on effectue le rendu */
  glViewport(0, 0, _wW, _wH);
}
/*!\brief Cette fonction dessine dans le contexte OpenGL actif. */
static void draw(void) {
  /* variable qui va permettre de passer d'une texture à l'autre, l'une en écriture, l'autre en lecture */
  static int ntex = 0;
  GLfloat step[] = {1.0f / _wW, 1.0f / _wH};
  /* activer le fbo _fbo en lecture/écriture (donc on ne dessine pas à l'écran) */
  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
  /* lier une des deux textures sur le fbo (pour écrire dedans) */
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _fboTex[ntex % 2],  0);
  /* effacement du buffer de couleur, donc de la texture liée au fbo. */
  glClear(GL_COLOR_BUFFER_BIT);
  /* activation du programme _pId */
  glUseProgram(_pId);
  /* activer l'étage de textures 0, plusieurs étages sont disponibles,
   * nous pouvons lier une texture par type et par étage */
  glActiveTexture(GL_TEXTURE0);
  /* utiliser l'autre texture (en écriture) */
  glBindTexture(GL_TEXTURE_2D, _fboTex[(ntex + 1) % 2]);
  /* envoyer une info au program shader indiquant que tex est une
   * texture d'étage 0, voir le type (sampler2D) de la variable tex
   * dans le shader */
  glUniform1i(glGetUniformLocation(_pId, "tex"), 0);
  /* envoyer les tailles des pas pour passer au texel suivant en x et y */
  glUniform2fv(glGetUniformLocation(_pId, "step"), 1, step);
  /* dessiner le quadrilatère */
  gl4dgDraw(_quad);
  /* désactiver le programme shader */
  glUseProgram(0);
  /* le <<fbo>> écran passe en actif sur le mode écriture */
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  /* on blit _fbo vers écran (0) */
  glBlitFramebuffer(0, 0, _wW, _wH, 0, 0, _wW, _wH, GL_COLOR_BUFFER_BIT, GL_NEAREST);
  //glBindFramebuffer(GL_FRAMEBUFFER, 0);
  ++ntex; /* pour échanger les rôles le coup d'après */
}
/*!\brief appelée au moment de sortir du programme (atexit), elle
 *  libère les éléments OpenGL utilisés.*/
static void quit(void) {
  /* suppression des deux textures en GPU */
  if(_fboTex[0]) {
    glDeleteTextures(2, _fboTex);
    _fboTex[0] = 0;
  }
  /* nettoyage des éléments utilisés par la bibliothèque GL4Dummies */
  gl4duClean(GL4DU_ALL);
}
