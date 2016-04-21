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

static inline GLfloat hurst(GLfloat d, GLfloat H) {
  return d / powf(2.0f, 2.0f * H);
}

static void triangle_edge(GLfloat *im, int x, int y, int w, int h, int width, GLfloat ri, GLfloat H) {
  GLint v;
  GLint p[9][2], i, w_2 = w >> 1, w_21 = w_2 + (w&1), h_2 = h >> 1, h_21 = h_2 + (h&1);
  ri = hurst(ri, H);
  p[0][0] = x;       p[0][1] = y;
  p[1][0] = x + w;   p[1][1] = y;
  p[2][0] = x + w;   p[2][1] = y + h;
  p[3][0] = x;       p[3][1] = y + h;
  p[4][0] = x + w_2; p[4][1] = y;
  p[5][0] = x + w;   p[5][1] = y + h_2;
  p[6][0] = x + w_2; p[6][1] = y + h;
  p[7][0] = x;       p[7][1] = y + h_2;
  p[8][0] = x + w_2; p[8][1] = y + h_2;
  for(i = 4; i < 8; i++) {
    if(im[p[i][0] + p[i][1] * width] > 0)
      continue;
    im[v = p[i][0] + p[i][1] * width] = (im[p[i - 4][0] + p[i - 4][1] * width] +
                                         im[p[(i - 3) % 4][0] + p[(i - 3) % 4][1] * width]) / 2.0;
    im[v] += gl4dmSURand() * ri;
    im[v] = MIN(MAX(im[v], GL4DM_EPSILON), 1.0);
  }
  if(im[p[i][0] + p[i][1] * width] < GL4DM_EPSILON) {
    im[v = p[8][0] + p[8][1] * width] = (im[p[0][0] + p[0][1] * width] +
                                         im[p[1][0] + p[1][1] * width] +
                                         im[p[2][0] + p[2][1] * width] +
                                         im[p[3][0] + p[3][1] * width]) / 4.0;
    im[v] += gl4dmSURand() * ri * sqrt(2);
    im[v] = MIN(MAX(im[v], GL4DM_EPSILON), 1.0);
  }
  if(w_2 > 1 || h_2 > 1)
    triangle_edge(im, p[0][0], p[0][1], w_2, h_2, width, ri, H);
  if(w_21 > 1 || h_2 > 1)
    triangle_edge(im, p[4][0], p[4][1], w_21, h_2, width, ri, H);
  if(w_21 > 1 || h_21 > 1)
    triangle_edge(im, p[8][0], p[8][1], w_21, h_21, width, ri, H);
  if(w_2 > 1 || h_21 > 1)
    triangle_edge(im, p[7][0], p[7][1], w_2, h_21, width, ri, H);
}

/*!\brief génère une heightmap en utilisant l'algorithme du
 * triangle-edge. Les valeurs retournées dans la map sont comprises
 * entre 0 et 1.
 *
 * \param width la largeur de la heightmap
 * \param height la hauteur de la heightmap
 * \param H exposant de Hurst, permet de controler la dimension fractale de la heightmap. La valeur standard est 0.5 (sinon entre 0 et 1)
 * \return la heightmap allouée/générée et qu'il faudra libérer avec free.
 */
GLfloat * gl4dmTriangleEdge(GLuint width, GLuint height, GLfloat H) {
  GLfloat * hm = calloc(width * height, sizeof *hm);
  hm[0] = GL4DM_EPSILON + gl4dmURand();
  hm[width - 1] = GL4DM_EPSILON + gl4dmURand();
  hm[(height - 1) * width + width - 1] = GL4DM_EPSILON + gl4dmURand();
  hm[(height - 1) * width] = GL4DM_EPSILON + gl4dmURand();
  triangle_edge(hm, 0, 0, width - 1, height - 1, width, 1, H);
  return hm;
}
