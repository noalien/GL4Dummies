/*!\file window.c
 * \brief Utilisation du raster "maison" pour finaliser le pipeline de
 * rendu 3D.
 * \author Farès BELHADJ, amsi@up8.edu
 * \date November 22, 2020.
 *
 * \todo CORRIGER LE MOUVEMENT POUR QU'IL SOIT INDÉPENDANT DU FRAMERATE !!!
 */
#include <assert.h>
/* inclusion des entêtes de fonctions de gestion de primitives simples
 * de dessin. La lettre p signifie aussi bien primitive que
 * pédagogique. */
#include <GL4D/gl4dp.h>
/* inclure notre bibliothèque "maison" de rendu */
#include "moteur.h"

/* inclusion des entêtes de fonctions de création et de gestion de
 * fenêtres système ouvrant un contexte favorable à GL4dummies. Cette
 * partie est dépendante de la bibliothèque SDL2 */
#include <GL4D/gl4duw_SDL2.h>

/* protos de fonctions locales (static) */
static void init(void);
static void draw(void);
static void key(int keycode);
static void sortie(void);

/*!\brief Deux triangles qui mappent le cube unitaire et une fois la
 * texture, attention pour le premier point (en bas à gauche) on a mis
 * un z éloigné (à -1.0f). Les sommets du premier triangles sont
 * donnés dans le sens inverse (backface). */
static triangle_t _t[2] = {
			   { { { {-1.0f, -1.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, { 0.0f,   0.0f}},
			       { {-1.0f,  1.0f,  0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, { 0.0f,  -1.0f}},
			       { { 1.0f,  1.0f,  0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, { 1.0f,  -1.0f}} } },
			   { { { {-1.0f, -1.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, { 0.0f,   0.0f}},
			       { { 1.0f, -1.0f,  0.0f, 1.0f}, {1.0f, 1.0f, 0.0f, 1.0f}, { 1.0f,   0.0f}},
			       { { 1.0f,  1.0f,  0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, { 1.0f,  -1.0f}} } }
};

/*!\brief un identifiant pour l'écran (de dessin) */
static GLuint _screenId = 0;

/*!\brief la surface composée des 2 triangles dans \a _t */
static surface_t * _s = NULL;

/* des variable d'états pour activer/désactiver des options de rendu */
static int _use_cull = 1, _use_tex = 0, _use_color = 1, _use_cm = 1, _use_lighting = 0, _use_ortho = 0;

/*!\brief paramètre l'application et lance la boucle infinie. */
int main(int argc, char ** argv) {
  /* tentative de création d'une fenêtre pour GL4Dummies */
  if(!gl4duwCreateWindow(argc, argv, /* args du programme */
			 "Mon moteur de rendu <<Maison>>", /* titre */
			 10, 10, 640, 480, /* x, y, largeur, heuteur */
			 GL4DW_SHOWN) /* état visible */) {
    /* ici si échec de la création souvent lié à un problème d'absence
     * de contexte graphique ou d'impossibilité d'ouverture d'un
     * contexte OpenGL (au moins 3.2) */
    return 1;
  }
  /* Pour forcer la désactivation de la synchronisation verticale */
  SDL_GL_SetSwapInterval(0);
  init();
  /* création d'un screen GL4Dummies (texture dans laquelle nous
   * pouvons dessiner) aux dimensions de la fenêtre */
  _screenId = gl4dpInitScreen();
  /* mettre _screenId comme "screen en cours" */
  gl4dpSetScreen(_screenId);

  /* mettre en place la fonction d'interception clavier */
  gl4duwKeyDownFunc(key);
  /* mettre en place la fonction de display */
  gl4duwDisplayFunc(draw);
  /* boucle infinie pour éviter que le programme ne s'arrête et ferme
   * la fenêtre immédiatement */
  gl4duwMainLoop();
  return 0;
}

/*!\brief init de nos données */
void init(void) {
  /* céer la surface, son comportement par défaut est : de cacher les
     backfaces, d'utiliser les couleurs aux sommets pour colorer, et
     de ne pas utiliser de texture. */
  _s = newSurface(_t, 2, 0);
  /* on lui rajoute une texture */
  setTexId(_s, getTexFromBMP("images/gl4d.bmp"));
  /* on lui demande de l'utiliser */
  if(_use_tex)
    enableSurfaceOption(_s, SO_USE_TEXTURE);
  /* mettre en place la fonction à appeler en cas de sortie */
  atexit(sortie);
}

/*!\brief la fonction appelée à chaque display. */
void draw(void) {
  static float a = 0.0f;
  float mvMat[16], projMat[16];
  /* des macros facilitant le travail avec des matrices et des
   * vecteurs se trouvent dans la bibliothèque GL4Dummies, dans le
   * fichier gl4dm.h */
  /* charger la matrice identité dans model-view */
  MIDENTITY(mvMat);
  /* ajouter une translation (à droite de la multiplication) */
  translate(mvMat, 0, 0, -3.0f + sin(a));
  /* ajouter une rotation (à droite de la multiplication) */
  rotate(mvMat, 100.0f * a, 0, 1, 0);
  /* choix entre orthogonale ou perspective */
  if(_use_ortho)
    MORTHO(projMat, -1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 10.0f);
  else
    MFRUSTUM(projMat, -0.05f, 0.05f, -0.05f, 0.05f, 0.1f, 10.0f);
  /* effacer l'écran */
  gl4dpClearScreen();
  /* effacer le buffer de profondeur */
  clearDepth();
  /* effectuer le rendu de la surface _s */
  transform_n_raster(_s, mvMat, projMat);
  /* déclarer qu'on a changé (en bas niveau) des pixels du screen  */
  gl4dpScreenHasChanged();
  /* fonction permettant de raffraîchir l'ensemble de la fenêtre*/
  gl4dpUpdateScreen(NULL);
  a += 0.001f;
}

/*!\brief intercepte l'événement clavier pour modifier les options. */
void key(int keycode) {
  switch(keycode) {
  case GL4DK_b: /* 'b' le back face culling */
    _use_cull = !_use_cull;
    if(_use_cull)
      enableSurfaceOption(_s, SO_CULL_BACKFACES);
    else
      disableSurfaceOption(_s, SO_CULL_BACKFACES);
    break;
  case GL4DK_t: /* 't' la texture */
    _use_tex = !_use_tex;
    if(_use_tex)
      enableSurfaceOption(_s, SO_USE_TEXTURE);
    else
      disableSurfaceOption(_s, SO_USE_TEXTURE);
    break;
  case GL4DK_c: /* 'c' utiliser une couleur (color material ou diffuse
		   color de la surface */
    _use_color = !_use_color;
    if(_use_color)
      enableSurfaceOption(_s, SO_USE_COLOR);
    else
      disableSurfaceOption(_s, SO_USE_COLOR);
    break;
  case GL4DK_m: /* 'm' utiliser la couleur au sommet comme matériel (color
		   material) */
    _use_cm = !_use_cm;
    if(_use_cm)
      enableSurfaceOption(_s, SO_COLOR_MATERIAL);
    else
      disableSurfaceOption(_s, SO_COLOR_MATERIAL);
    break;
  case GL4DK_l: /* 'l' utiliser l'ombrage par la méthode Gouraud */
    _use_lighting = !_use_lighting;
    if(_use_lighting)
      enableSurfaceOption(_s, SO_USE_LIGHTING);
    else
      disableSurfaceOption(_s, SO_USE_LIGHTING);
    break;
  case GL4DK_o: /* 'o' projection orthogonale ou perspective */
    _use_ortho = !_use_ortho;
    break;
  default: break;
  }
}

/*!\brief à appeler à la sortie du programme. */
void sortie(void) {
  if(_s) {
    freeSurface(_s);
    _s = NULL;
  }
  /* libère tous les objets produits par GL4Dummies, ici
   * principalement les screen */
  gl4duClean(GL4DU_ALL);
}
