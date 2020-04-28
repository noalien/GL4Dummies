/*!\file window.c
 * \brief GL4Dummies, exemple simple 2D avec GL4Dummies. Seconde
 * approche pour dessiner des segments de droites : Analyse Discrète
 * Différencielle et Algorithme de Bressenham 1965.
 *
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date February 04 2018
 */

/* inclusion des entêtes de fonctions de gestion de primitives simples
 * de dessin. La lettre p signifie aussi bien primitive que
 * pédagogique. */
#include <GL4D/gl4dp.h>

/* inclusion des entêtes de fonctions de création et de gestion de
 * fenêtres système ouvrant un contexte favorable à GL4dummies. Cette
 * partie est dépendante de la bibliothèque SDL2 */
#include <GL4D/gl4duw_SDL2.h>

/*!\brief appelée au moment de sortir du programme (atexit), elle
 *  libère les éléments utilisés par GL4Dummies.*/
static void quitte(void) {
  gl4duClean(GL4DU_ALL);
}

/*!\brief implémentation Bressenham'65 du tracé de droite. 
 * 
 * Exercice : réaliser l'analyse discrète différencielle de l'équation
 * du cercle et en faire une implémentation.
 */
static void br65(int x0, int y0, int x1, int y1) {
  int u = x1 - x0, v = y1 - y0, del, incH, incO, au, av, pasX, pasY;
  if(u < 0) {
    au = -u; pasX = -1;
  } else {
    au = u;  pasX = 1;
  }
  if(v < 0) {
    av = -v; pasY = -1;
  } else {
    av = v;  pasY = 1;
  }
  if(au > av) { /* premier octant */
    del = (incH = -2 * av) + au;
    incO = 2 * au + incH;
    for(x1 += pasX; x0 != x1; x0 += pasX) {
      gl4dpPutPixel(x0, y0); 
      if(del < 0) {
	del += incO;
	y0 += pasY;
      } else 
	del += incH;
    }
  } else {  /* deuxième octant */
    del = (incH = -2 * au) + av;
    incO = 2 * av + incH;
    for(y1 += pasY; y0 != y1; y0 += pasY) {
      gl4dpPutPixel(x0, y0);
      if(del < 0) {
	del += incO;
	x0 += pasX;
      } else 
	del += incH;
    }
  }
}

/*!\brief fonction appelée à chaque draw par la gl4duwMainLoop. */
static void dessin(void) {
  float a;
  /* partant du centre du screen, nous dessinons des segments
   * décrivant un cercle ; nous utilisons la formule
   * trigonométrique */
  for(a = 0; a < 2.0f * M_PI; a += 0.02f) {
    /* choisissons au hasard 3 intensités de composantes primaires de
     * couleur (min est 0, max est 255 car sur un octet non signé). */
    GLubyte r = rand() % 256, g = rand() % 256, b = rand() % 256;
    /* nous mettons la couleur aléatoire */
    gl4dpSetColor(RGB(r, g, b));
    /* dessinons le segment de droite */
    br65(gl4dpGetWidth() / 2, gl4dpGetHeight() / 2, 
	    gl4dpGetWidth() / 2  + (gl4dpGetWidth() / 2  - 1) * cos(a), 
	    gl4dpGetHeight() / 2 + (gl4dpGetHeight() / 2 - 1) * sin(a));
  }
  /* mise à jour de l'écran, essayez de mettre cet appel dans la
   * boule ci-dessus. */
  gl4dpUpdateScreen(NULL);
}

/*!\brief créé la fenêtre, un screen 2D effacé en noir et lance une
 *  boucle infinie.*/
int main(int argc, char ** argv) {
  /* tentative de création d'une fenêtre pour GL4Dummies */
  if(!gl4duwCreateWindow(argc, argv, /* args du programme */
			 "GL4Dummies' Hello Pixels", /* titre */
			 10, 10, 800, 800, /* x,y, largeur, heuteur */
			 GL4DW_SHOWN) /* état visible */) {
    /* ici si échec de la création souvent lié à un problème d'absence
     * de contexte graphique ou d'impossibilité d'ouverture d'un
     * contexte OpenGL (au moins 3.2) */
    return 1;
  }
  /* création d'un screen GL4Dummies (texture dans laquelle nous
   * pouvons dessiner) aux dimensions de la fenêtre */
  gl4dpInitScreen();
  /* ajoute la fonction quitte à la pile des choses à faire en sortant
   * du programme */
  atexit(quitte);
  /* met en place une fonction de display au sein de la boucle
   * event-simu-draw gl4duwMainLoop */
  gl4duwDisplayFunc(dessin);
  /* boucle infinie pour éviter que le programme ne s'arrête et ferme
   * la fenêtre */
  gl4duwMainLoop();
  return 0;
}
