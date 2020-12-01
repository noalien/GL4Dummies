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
static void draw(void);
static void sortie(void);
static void copyFromSDLSurface(SDL_Surface * s);

/*!\brief Deux triangles qui mappent le cube unitaire et une fois la
 * texture, attention pour le premier point (en bas à gauche) on a mis
 * un z éloigné (à -1.0f) */
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
/*!\brief un autre identifiant pour un screen-texture (une texture
 * chargée depuis un fichier et collé dans un screen) */
static GLuint _texId = 0;

/*!\brief paramètre l'application et lance la boucle infinie. */
int main(int argc, char ** argv) {
  SDL_Surface *s;
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
  /* création d'un screen GL4Dummies (texture dans laquelle nous
   * pouvons dessiner) aux dimensions de la fenêtre */
  _screenId = gl4dpInitScreen();


  /* chargement d'une image dans une surface SDL */
  s = SDL_LoadBMP("images/gl4d.bmp");
  assert(s);
  /* création d'un screen GL4Dummies aux dimensions de la texture */
  _texId = gl4dpInitScreenWithDimensions(s->w, s->h);
  /* copie de la surface SDL vers le screen en cours (_texId) */
  copyFromSDLSurface(s);
  /* libération de la surface SDL */
  SDL_FreeSurface(s);
  /* mettre en place la texture qui sera utilisée pour mapper la surface */
  setTexture(_texId);
  /* mettre _screenId comme "screen en cours" */
  gl4dpSetScreen(_screenId);
  /* mettre en place la fonction de display */
  gl4duwDisplayFunc(draw);
  /* mettre en place la fonction à appeler en cas de sortie */
  atexit(sortie);
  /* boucle infinie pour éviter que le programme ne s'arrête et ferme
   * la fenêtre immédiatement */
  gl4duwMainLoop();
  return 0;
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
  /* ajouter une translation (à droite) */
  translate(mvMat, 0, 0, -3.0f + sin(a));
  /* ajouter une rotation (à droite) */
  rotate(mvMat, 100.0f * a, 0, 1, 0);
  /* charger la matrice identité dans projection (pas nécessaire) */
  MIDENTITY(projMat);
  /* exemples de ce qu'on peut charger comme "projections" dans
   * projMat */
  /* MORTHO(projMat, -1.0f, 1.0f, -1.0f, 5.0f, 0.0f, 10.0f); */
  /* MFRUSTUM(projMat, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 10.0f); */
  MFRUSTUM(projMat, -0.05f, 0.05f, -0.05f, 0.05f, 0.1f, 10.0f);
  /* transformer l'ensemble des sommets des 2 triangles pour
   * projection à l'écran WxH */
  stransform(_t, 2, mvMat, projMat, gl4dpGetWidth(), gl4dpGetHeight());
  /* effacer l'écran */
  gl4dpClearScreen();
  /* rasteriser les 2 triangles précédemment transformés */
  fillTriangle(&_t[0]);
  fillTriangle(&_t[1]);
  /* déclarer qu'on a changé (en bas niveau) des pixels du screen  */
  gl4dpScreenHasChanged();
  /* fonction permettant de raffraîchir l'ensemble de la fenêtre*/
  gl4dpUpdateScreen(NULL);
  a += 0.001f;
}

/*!\brief à appeler à la sortie du programme. */
void sortie(void) {
  /* libère tous les objets produits par GL4Dummies, ici
   * principalement les screen */
  gl4duClean(GL4DU_ALL);
}

/*!\brief copier une surface SDL vers le screen courant */
void copyFromSDLSurface(SDL_Surface * s) {
  GLuint * p = gl4dpGetPixels();
  SDL_Surface * d = SDL_CreateRGBSurface(0, s->w, s->h, 32, R_MASK, G_MASK, B_MASK, A_MASK);
  SDL_BlitSurface(s, NULL, d, NULL);  
  memcpy(p, d->pixels, d->w * d->h * sizeof *p);
  SDL_FreeSurface(d);
}
