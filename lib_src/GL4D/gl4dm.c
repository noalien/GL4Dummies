/*!\file gl4dm.c
 *
 * \brief fonctions liées aux calculs mathématiques propres à
 * l'utilisation de GL4Dummies.
 *
 * \author Farès BELHADJ amsi@ai.univ-paris8.fr
 * \date November 12, 2014
*/

#include "gl4dm.h"
#include <stdlib.h>
#include <math.h>

/*!\brief Retourne un nombre pseudo-aleatoire dans l'intervalle [0,
 * 1[. Ici la distribution est uniforme.
 *
 * \return un nombre pseudo-aleatoire dans l'intervalle [0, 1[.
*/
double gl4dmURand(void) {
  return (rand() / (RAND_MAX + 1.0));
}

/*!\brief Retourne un nombre pseudo-aleatoire dans l'intervalle [-1,
 * 1[. Ici la distribution est uniforme.
 *
 * \return un nombre pseudo-aleatoire dans l'intervalle [-1, 1[.
*/
double gl4dmSURand(void) {
  return 2.0 * (rand() / (RAND_MAX + 1.0)) - 1.0;
}

/*!\brief Retourne un nombre pseudo-aleatoire dans l'intervalle ]-7,
 * +7[. Ici la distribution est Gaussienne.
 *
 * Tire du knuth : le rand Gaussien a deux valeurs par passe deux rand
 * : X1 et X2.
 *
 * \return un nombre pseudo-aleatoire selon une distribution
 * gaussienne dans l'intervalle ]-7, +7[ (expérimentalement dans
 * [-6.997568, 6.882738])
 *
 */
double gl4dmGRand(void) {
  static int haveX2 = 0;
  static double X2 = 0.0;
  if (haveX2) {
    haveX2 = 0;
    return X2;
  } else {
    double V1, V2, S, precomp;
    do {
      V1 = 2.0 * (rand() / (double)RAND_MAX) - 1.0;
      V2 = 2.0 * (rand() / (double)RAND_MAX) - 1.0;
      S = V1 * V1 + V2 * V2;
    } while (S >= 1.0);
    if(S == 0.0) return (X2 = !((haveX2 = 1)));
    precomp = sqrt(-2.0 * log(S) / S);
    X2 = V2 * precomp;
    haveX2 = 1;
    return V1 * precomp; /* X1 */
  }
}

/*!\brief Retourne un nombre pseudo-aleatoire dans l'intervalle [-1,
 * +1[. Ici la distribution est Gaussienne centrée en zéro.
 *
 * \return un nombre pseudo-aleatoire selon une distribution
 * gaussienne dans l'intervalle ]-1, +1[
 *
 */
double gl4dmGURand(void) {
  return gl4dmGRand() / 7.0;
}

