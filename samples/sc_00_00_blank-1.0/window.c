/*!\file window.c
 * \brief GL4Dummies, exemple simple 2D avec GL4Dummies
 * \author Farès BELHADJ, amsi@up8.edu
 * \date February 04 2018, modified on March 06, 2020 by 
 * adding Visual Studio configuration files.
 *
 * A l'exécution, il est possible de constater un scintillement de
 * l'image. Ceci est dû au fait que le screen créé dans cet exemple
 * n'est copié qu'une seule fois dans le buffer couleur de la surface
 * GL (par le biais de gl4dpUpdateScreen) ; or il y a deux buffers (en
 * double buffering) le second peut donc s'afficher avec son contenu
 * non initialisé (ou initialisé par défaut). Si les deux contenus
 * diffèrent, on observe un scintillement. Ceci n'apparaît pas si on
 * créé une callback sur une fonction display qui raffraîchit les
 * buffers à chaque frame (toujours par le biais de
 * gl4dpUpdateScreen). Une autre solution consisterait à forcer le
 * second buffer à se raffraîchir au moins une fois juste avant
 * d'entrer dans la mainloop :
 * SDL_GL_SwapWindow(gl4duwGetSDL_Window()); 
 * gl4dpUpdateScreen(NULL);
 */

/* inclusion des entêtes de fonctions de gestion de primitives simples
 * de dessin. La lettre p signifie aussi bien primitive que
 * pédagogique. */
#include <GL4D/gl4dp.h>

/* inclusion des entêtes de fonctions de création et de gestion de
 * fenêtres système ouvrant un contexte favorable à GL4dummies. Cette
 * partie est dépendante de la bibliothèque SDL2 */
#include <GL4D/gl4duw_SDL2.h>

/*!\brief créé la fenêtre, un screen 2D effacé en noir et lance une
 *  boucle infinie.*/
int main(int argc, char ** argv) {
  /* tentative de création d'une fenêtre pour GL4Dummies */
  if(!gl4duwCreateWindow(argc, argv, /* args du programme */
			 "GL4Dummies' Hello World", /* titre */
			 10, 10, 320, 240, /* x,y, largeur, heuteur */
			 GL4DW_SHOWN) /* état visible */) {
    /* ici si échec de la création souvent lié à un problème d'absence
     * de contexte graphique ou d'impossibilité d'ouverture d'un
     * contexte OpenGL (au moins 3.2) */
    return 1;
  }

  /* création d'un screen GL4Dummies (texture dans laquelle nous
   * pouvons dessiner) aux dimensions de la fenêtre */
  gl4dpInitScreen();

  /* effacement du screen en cours en utilisant la couleur par défaut,
   * le noir */
  gl4dpClearScreen();

  /* décommentez la ligne suivante pour effacer l'écran en bleu foncé. */
  //gl4dpClearScreenWith(RGB(0, 0, 155));

  /* fonction permettant de raffraîchir l'ensemble de la fenêtre*/
  gl4dpUpdateScreen(NULL);
  /* boucle infinie pour éviter que le programme ne s'arrête et ferme
   * la fenêtre immédiatement */
  gl4duwMainLoop();
  return 0;
}
