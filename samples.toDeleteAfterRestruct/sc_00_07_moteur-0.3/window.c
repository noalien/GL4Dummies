/*!\file window.c
 * \brief Utilisation du raster "maison" pour finaliser le pipeline de
 * rendu 3D. Ici on peut voir les géométries disponibles.
 * \author Farès BELHADJ, amsi@up8.edu
 * \date December 4, 2020.
 * \todo pour les étudiant(e)s : changer la variation de l'angle de
 * rotation pour qu'il soit dépendant du temps et non du framerate
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

/*!\brief un identifiant pour l'écran (de dessin) */
static GLuint _screenId = 0;

/*!\brief une surface représentant un quadrilatère */
static surface_t * _quad = NULL;
/*!\brief une surface représentant un cube */
static surface_t * _cube = NULL;
/*!\brief une surface représentant une sphere */
static surface_t * _sphere = NULL;

/* des variable d'états pour activer/désactiver des options de rendu */
static int _use_tex = 1, _use_color = 1, _use_lighting = 1;

/*!\brief on peut bouger la caméra vers le haut et vers le bas avec cette variable */
static float _ycam = 3.0f;

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
  /* mettre en place la fonction d'interception clavier */
  gl4duwKeyDownFunc(key);
  /* mettre en place la fonction de display */
  gl4duwDisplayFunc(draw);
  /* boucle infinie pour éviter que le programme ne s'arrête et ferme
   * la fenêtre immédiatement */
  gl4duwMainLoop();
  return 0;
}

/*!\brief init de nos données, spécialement les trois surfaces
 * utilisées dans ce code */
void init(void) {
  GLuint id;
  vec4 r = {1, 0, 0, 1}, g = {0, 1, 0, 1}, b = {0, 0, 1, 1};
  /* on créé nos trois type de surfaces */
  _quad   =   mkQuad();       /* ça fait 2 triangles        */
  _cube   =   mkCube();       /* ça fait 2x6 triangles      */
  _sphere = mkSphere(12, 12); /* ça fait 12x12x2 trianles ! */
  /* on change les couleurs de surfaces */
  _quad->dcolor = r; _cube->dcolor = b; _sphere->dcolor = g; 
  /* on leur rajoute la même texture */
  id = getTexFromBMP("images/tex.bmp");
  setTexId(  _quad, id);
  setTexId(  _cube, id);
  setTexId(_sphere, id);
  /* si _use_tex != 0, on active l'utilisation de la texture pour les
   * trois */
  if(_use_tex) {
    enableSurfaceOption(  _quad, SO_USE_TEXTURE);
    enableSurfaceOption(  _cube, SO_USE_TEXTURE);
    enableSurfaceOption(_sphere, SO_USE_TEXTURE);
  }
  /* si _use_lighting != 0, on active l'ombrage */
  if(_use_lighting) {
    enableSurfaceOption(  _quad, SO_USE_LIGHTING);
    enableSurfaceOption(  _cube, SO_USE_LIGHTING);
    enableSurfaceOption(_sphere, SO_USE_LIGHTING);
  }
  /* on désactive le back cull face pour le quadrilatère, ainsi on
   * peut voir son arrière quand le lighting est inactif */
  disableSurfaceOption(_quad, SO_CULL_BACKFACES);
  /* mettre en place la fonction à appeler en cas de sortie */
  atexit(sortie);
}

/*!\brief la fonction appelée à chaque display. */
void draw(void) {
  static float a = 0.0f;
  float mvMat[16], projMat[16], nmv[16];
  /* effacer l'écran et le buffer de profondeur */
  gl4dpClearScreen();
  clearDepth();
  /* des macros facilitant le travail avec des matrices et des
   * vecteurs se trouvent dans la bibliothèque GL4Dummies, dans le
   * fichier gl4dm.h */
  /* charger un frustum dans projMat */
  MFRUSTUM(projMat, -0.05f, 0.05f, -0.05f, 0.05f, 0.1f, 1000.0f);
  /* charger la matrice identité dans model-view */
  MIDENTITY(mvMat);
  /* on place la caméra en arrière-haut, elle regarde le centre de la scène */
  lookAt(mvMat, 0, _ycam, 10, 0, 0, 0, 0, 1, 0);
  /* le quadrilatère est mis à gauche et tourne autour de son axe x */
  memcpy(nmv, mvMat, sizeof nmv); /* copie mvMat dans nmv */
  translate(nmv, -3.0f, 0.0f, 0.0f);
  rotate(nmv, a, 1.0f, 0.0f, 0.0f);
  transform_n_raster(_quad, nmv, projMat);
  /* le cube est mis à droite et tourne autour de son axe z */
  memcpy(nmv, mvMat, sizeof nmv); /* copie mvMat dans nmv */
  translate(nmv, 3.0f, 0.0f, 0.0f);
  rotate(nmv, a, 0.0f, 0.0f, 1.0f);
  transform_n_raster(_cube, nmv, projMat);
  /* la sphère est laissée au centre et tourne autour de son axe y */
  memcpy(nmv, mvMat, sizeof nmv); /* copie mvMat dans nmv */
  rotate(nmv, a, 0.0f, 1.0f, 0.0f);
  transform_n_raster(_sphere, nmv, projMat);
  /* déclarer qu'on a changé (en bas niveau) des pixels du screen  */
  gl4dpScreenHasChanged();
  /* fonction permettant de raffraîchir l'ensemble de la fenêtre*/
  gl4dpUpdateScreen(NULL);
  a += 0.1f;
}

/*!\brief intercepte l'événement clavier pour modifier les options. */
void key(int keycode) {
  switch(keycode) {
  case GL4DK_UP:
    _ycam += 0.05f;
    break;
  case GL4DK_DOWN:
    _ycam -= 0.05f;
    break;
  case GL4DK_t: /* 't' la texture */
    _use_tex = !_use_tex;
    if(_use_tex) {
      enableSurfaceOption(  _quad, SO_USE_TEXTURE);
      enableSurfaceOption(  _cube, SO_USE_TEXTURE);
      enableSurfaceOption(_sphere, SO_USE_TEXTURE);
    } else {
      disableSurfaceOption(  _quad, SO_USE_TEXTURE);
      disableSurfaceOption(  _cube, SO_USE_TEXTURE);
      disableSurfaceOption(_sphere, SO_USE_TEXTURE);
    }
    break;
  case GL4DK_c: /* 'c' utiliser la couleur */
    _use_color = !_use_color;
    if(_use_color) {
      enableSurfaceOption(  _quad, SO_USE_COLOR);
      enableSurfaceOption(  _cube, SO_USE_COLOR);
      enableSurfaceOption(_sphere, SO_USE_COLOR);
    } else { 
      disableSurfaceOption(  _quad, SO_USE_COLOR);
      disableSurfaceOption(  _cube, SO_USE_COLOR);
      disableSurfaceOption(_sphere, SO_USE_COLOR);
    }
    break;
  case GL4DK_l: /* 'l' utiliser l'ombrage par la méthode Gouraud */
    _use_lighting = !_use_lighting;
    if(_use_lighting) {
      enableSurfaceOption(  _quad, SO_USE_LIGHTING);
      enableSurfaceOption(  _cube, SO_USE_LIGHTING);
      enableSurfaceOption(_sphere, SO_USE_LIGHTING);
    } else { 
      disableSurfaceOption(  _quad, SO_USE_LIGHTING);
      disableSurfaceOption(  _cube, SO_USE_LIGHTING);
      disableSurfaceOption(_sphere, SO_USE_LIGHTING);
    }
    break;
  default: break;
  }
}

/*!\brief à appeler à la sortie du programme. */
void sortie(void) {
  /* on libère nos trois surfaces */
  if(_quad) {
    freeSurface(_quad);
    _quad = NULL;
  }
  if(_cube) {
    freeSurface(_cube);
    _cube = NULL;
  }
  if(_sphere) {
    freeSurface(_sphere);
    _sphere = NULL;
  }
  /* libère tous les objets produits par GL4Dummies, ici
   * principalement les screen */
  gl4duClean(GL4DU_ALL);
}
