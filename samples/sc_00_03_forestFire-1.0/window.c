/*!\file window.c
 * \brief Feu de forêt basé sur les primitives 2D de GL4Dummies
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date May 19 2020
 */

/* inclusion des entêtes de fonctions de gestion de primitives simples
 * de dessin. La lettre p signifie aussi bien primitive que
 * pédagogique. */
#include <GL4D/gl4dp.h>

/* inclusion des entêtes de fonctions mathématiques de GL4Dummies. La
 * lettre m signifie math. */
#include <GL4D/gl4dm.h>

/* inclusion de stdlib.h pour les fonctions malloc et free. */
#include <stdlib.h>

/* inclusion de string.h pour la fonction memcpy. */
#include <string.h>

/* inclusion de stdlib.h pour la macro assert. */
#include <assert.h>

/* inclusion des entêtes de fonctions de création et de gestion de
 * fenêtres système ouvrant un contexte favorable à GL4dummies. Cette
 * partie est dépendante de la bibliothèque SDL2 */
#include <GL4D/gl4duw_SDL2.h>

/*!\brief enumération d'états pris par les cellules de l'automate
 * cellulaire du feu de forêt. */
enum ca_states_t {
		EMPTY = RGB(96, 46, 8),
		TREE = RGB(41, 175, 29),
		FIRE = RGB(235, 38, 31),
		CALCINED = RGB(94, 94, 94)
};

/*!\brief initialisation de la carte (forêt) avec une densité
 * d'arbres passée en argument, le reste étant vide. 
 *
 * gl4dmURand() se trouve dans gl4dm.h, elle retourne un nombre
 * pseudo-aleatoire dans l'intervalle [0, 1[. Ici la distribution est
 * uniforme. Ne pas oublier d'initialiser la chaîne aléatoire à l'aide
 * de srand(une_seed).
 *
 * \param density valeur dans [0, 1] exprimant la proportion d'arbres
 * dans la carte allant de 0 à 100%.
 */
static void initForest(double density) {
  GLuint i, wh = gl4dpGetWidth() * gl4dpGetHeight(), * map = gl4dpGetPixels();
  for(i = 0; i < wh; ++i)
    map[i] = (gl4dmURand() < density) ? TREE : EMPTY;
}

/*!\brief allume le feu à tous les arbres du bord gauche. */
static void lightTheFire(void) {
  GLuint i, w = gl4dpGetWidth(), wh = w * gl4dpGetHeight(), * map = gl4dpGetPixels();
  for(i = 0; i < wh; i += w)
    map[i] = (map[i] == TREE) ? FIRE : map[i];
}

/*!\brief appelée au moment de sortir du programme (atexit), elle
 *  libère les éléments utilisés par GL4Dummies.*/
static void quitte(void) {
  gl4duClean(GL4DU_ALL);
}

/*!\brief simulation d'une étape de changement d'état pour l'ensemble
 * des cellules. */
static void cellularAutomatonStep(void) {
  GLuint i, w = gl4dpGetWidth(), h = gl4dpGetHeight(), wh = w * h, * map = gl4dpGetPixels();
  /* à l'étape actuelle, notée (t + 1) car elle représente les états à
   * venir de l'automate, il est nécessaire de se renseigner sur les
   * états de la carte de l'étape précédente notée (t), soit avant
   * modification. On fait donc une sauvegarde de la carte avant
   * modification, et on utilise cette sauvegarde pour réaliser les
   * tests provoquant un changement d'états. */
  /* (1) on alloue la mémoire nécessaire à cette sauvegarde. La
   * fonction malloc alloue dynamiquement une zone mémoire de la
   * taille souhaitée en octets et retourne son adresse. Ici on veut
   * wh (soit w x h) fois la taille d'un des éléments pointés par le
   * pointeur (soit *map_copy qui est un GLuint dans ce cas). Cette
   * fonction est prototypée dans <stdlib.h>. */
  GLuint * map_copy = malloc( wh * sizeof *map_copy );
  /* (2) assert sert à ajouter une obligation de non nullité sur sont
   * argument, si ce dernier est nul le programme s'arrête à la
   * position de l'assert et affiche un message d'erreur "assertion
   * failed" indiquant le fichier source et le numéro de ligne. Cette
   * macro est donnée dans <assert.h>.*/
  assert(map_copy);
  /* (3) copier les octets vers une zone mémoire à partir d'une autre
   * zone, il est nécessaire de spécifier le nombre d'octets à
   * copier. memcpy(adresse_destination, adresse_source,
   * nombre_d_octets) se trouve prototypée dans <string.h>. */
  memcpy(map_copy, map, wh * sizeof *map_copy);
  /* tout est prêt pour lancer l'automate sur l'ensemble des cellules */
  for(i = 0; i < wh; ++i) {
    switch (map[i]) {
    case EMPTY: /* rien à faire, ou */
    case CALCINED: /* rien à faire aussi */
      break;
    case FIRE: /* il a brulé => devient calciné */
      map[i] = CALCINED;
      break;
    case TREE: /* regardons les voisins */
      {
	/* récupérons l'abscisse et l'ordonnée de la cellule à l'indice i */
	int x = i % w, y = i / w;
	/* l'abscisse et l'ordonnée du voisin (neighbor), un j pour les parcourir */
	int nx, ny, j;
	/* les 4 directions pour le voisinage en 4-connexité */
	const int d[][2] = { /* est   */ { 1,  0},
			     /* nord  */ { 0, -1},
			     /* ouest */ {-1,  0},
			     /* sud   */ { 0,  1} };
	for(j = 0; j < 4; ++j) {
	  nx = x + d[j][0];
	  ny = y + d[j][1];
	  if( (nx >= 0 && ny >= 0 && nx < w && ny < h) /* pour ne pas sortir de la map */ && /* ET ... */
	      map_copy[ny * w + nx] == FIRE /* ... ce voisin <était> en feu */ ) {
	    map[i] = FIRE; /* la cellule i prend feu */
	    break; /* plus besoin de chercher plus loin */
	  }
	}
      }
      break;
    }
  }
  /* plus besoin de la sauvegarde de l'état précédent de la map. Il
   * est donc nécessaire de libérer la mémoire allouée via malloc
   * (sinon fuites mémoires => application qui consomme de plus en
   * plus de mémoire de tour en tour. Utiliser la fonction
   * free(pointeur_vers_la_zone_allouée) pour libérer cette mémoire. */
  free(map_copy);
  /* Il est nécessaire d'indiquer que map (qui fait référence au
   * screen) a changé pour que gl4dpUpdateScreen (est appelée dans la
   * fonction dessine ci-après) déclenche une mise à jour de
   * l'affichage. En l'absence d'une déclaration de changements,
   * gl4dpUpdateScreen ne fait rien. Cette appel n'est pas nécessaire
   * quand nous utilisons les primtives de dessin telles que
   * gl4dpPutPixel, gl4dpLine, ... */
  gl4dpScreenHasChanged();
}

/*!\brief fonction appelée à chaque draw par la gl4duwMainLoop.*/
static void dessine(void) {
  /* mise à jour du screen côté OpenGL */
  gl4dpUpdateScreen(NULL);
}

/*!\brief créé la fenêtre, un screen 2D effacé en noir et lance une
 *  boucle infinie.*/
int main(int argc, char ** argv) {
  /* tentative de création d'une fenêtre de 800x600 */
  if(!gl4duwCreateWindow(argc, argv, /* args du programme */
			 "GL4Dummies' Forest fire !!!", /* titre */
			 10, 10, 800, 600, /* x,y, largeur, heuteur */
			 GL4DW_SHOWN) /* état visible */) {
    /* ici si échec de la création souvent lié à un problème d'absence
     * de contexte graphique ou d'impossibilité d'ouverture d'un
     * contexte OpenGL (au moins 3.2) */
    return 1;
  }
  /* création d'un screen GL4Dummies (texture dans laquelle nous
   * pouvons dessiner) à des dimensions deux fois inférieures à celle
   * de la fenêtre */
  gl4dpInitScreenWithDimensions(400, 300);
  initForest(0.6);
  lightTheFire();
  /* ajoute la fonction quitte à la pile des choses à faire en sortant
   * du programme */
  atexit(quitte);
  gl4duwIdleFunc(cellularAutomatonStep);
  /* met en place une fonction de display au sein de la boucle
   * event-simu-draw gl4duwMainLoop */
  gl4duwDisplayFunc(dessine);
  /* boucle infinie pour éviter que le programme ne s'arrête et ferme
   * la fenêtre */
  gl4duwMainLoop();
  return 0;
}
