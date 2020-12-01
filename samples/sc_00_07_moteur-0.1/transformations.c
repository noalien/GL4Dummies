/*!\file transformations.c
 * \brief transformations spatiales pour un moteur de rendu basé raster "maison".
 *
 * CE CODE A ÉTÉ EN PARTIE RÉALISÉ LORS DE LA SÉANCE DE COURS. IL RESTE DES CHOSES À IMPLÉMENTER OU À OPTIMISER.
 *
 * \author Farès BELHADJ, amsi@up8.edu
 * \date November 24, 2020.
 *
 * \todo COMPLÉTER LE CLIPPING POUR GÉRER ICI LES TRIANGLES
 * PARTIELLEMENT HORS-CHAMP ET ÉVITER DES TESTS GOURMANDS DANS \ref
 * primitives.c
 *
 * \todo IMPLÉMENTER LE SCALE, UNE CAMÉRA LIBRE (LE LOOKAT), LE
 * PRODUIT SCALAIRE AVEC LA LUMIÈRE (GOURAUD), LE PRODUIT VECTORIEL
 * POUR L'ORIENTATION (FRONT OU BACK) DES TRIANGLES.
 */
#include "moteur.h"
#include <assert.h>

/* fonctions locale (static) */
static inline void clip2UnitCube(triangle_t * t);

/*!\brief projette le sommet \a v à l'écran (\a W x \a H) selon la
   matrice de model-view \a mvMat et de projection \a projMat. */
vertex_t vtransform(vertex_t v, float * mvMat, float * projMat, int W, int H) {
  float dist = 1;
  vec4 r1, r2;
  v.state = PS_NONE;
  MMAT4XVEC4((float *)&r1, mvMat, (float *)&(v.position));
  MMAT4XVEC4((float *)&r2, projMat, (float *)&r1);
  r2.x /= r2.w;
  r2.y /= r2.w;
  r2.z /= r2.w;
  r2.w = 1.0f;
  /* dist doit être à 1 ci-après */
  if(r2.x < -dist) v.state |= PS_OUT_LEFT;
  if(r2.x >  dist) v.state |= PS_OUT_RIGHT;
  if(r2.y < -dist) v.state |= PS_OUT_BOTTOM;
  if(r2.y >  dist) v.state |= PS_OUT_TOP;
  if(r2.z < -dist) v.state |= PS_OUT_NEAR;
  if(r2.z >  dist) v.state |= PS_OUT_FAR;
  /* "hack" pas terrible permettant d'éviter les gros triangles
     partiellement hors-champ. Modifier dist pour jouer sur la taille
     (une fois projetés) des triangles qu'on laisse passer (plus c'est
     gros plus c'est lent avec les gros triangles). La "vraie"
     solution est obtenue en calculant l'intersection exacte entre le
     triangle et le cube unitaire ; attention, ceci produit
     potentiellement une nouvelle liste de triangles à chaque frame,
     et les attributs des sommets doivent être recalculés. */
  dist = 10;
  if(r2.x < -dist || r2.x > dist || r2.y < -dist || r2.y > dist || r2.z < -dist || r2.z > dist) {
    v.state |= PS_TOO_FAR;
    return v;
  }
  /* TODO : Gouraud */
  v.icolor = v.color0;
  /* Mapping du cube unitaire vers l'écran */
  v.x = ((r2.x + 1.0f) * 0.5f) * (W - EPSILON);
  v.y = ((r2.y + 1.0f) * 0.5f) * (H - EPSILON);
  v.z = (r2.z + 1.0f) * 0.5f;
  v.zmod = r1.z;
  return v;
}

/*!\brief projette le triangle \a t à l'écran (\a W x \a H) selon la
 * matrice de model-view \a mvMat et de projection \a projMat.
 *
 * Cette fonction utilise \a vtransform sur chaque sommet de la
 * surface. Elle utilise aussi \a clip2UnitCube pour connaître l'état
 * du triangle par rapport au cube unitaire.
 *
 * \see vtransform 
 * \see clip2UnitCube
 * \todo utiliser la normale à la face pour la supprimer du rendu ou non.
 */
void stransform(triangle_t * t, int n, float * mvMat, float * projMat, int W, int H) {
  int i, j;
  for(i = 0; i < n; ++i) {
    t[i].state = PS_NONE;
    for(j = 0; j < 3; ++j) {
      t[i].v[j] = vtransform(t[i].v[j], mvMat, projMat, W, H);
    }
    clip2UnitCube(&t[i]);
  }
}

/*!\brief multiplie deux matrices : \a res = \a res x \a m */
void multMatrix(float * res, float * m) {
  /* res = res x m */
  float cpy[16];
  memcpy(cpy, res, sizeof cpy);
  MMAT4XMAT4(res, cpy, m);
}

/*!\brief ajoute (multiplication droite) une translation à la matrice
 * \a m */
void translate(float * m, float tx, float ty, float tz) {
  float mat[] = { 1.0f, 0.0f, 0.0f, tx,
		  0.0f, 1.0f, 0.0f, ty,
		  0.0f, 0.0f, 1.0f, tz,
		  0.0f, 0.0f, 0.0f, 1.0f };
  multMatrix(m, mat);
}

/*!\brief ajoute (multiplication droite) une rotation à la matrice \a
 * m */
void rotate(float * m, float angle, float x, float y, float z) {
  float n = sqrtf(x * x + y * y + z * z);
  if ( n > 0.0f ) {
    float a, s, c, cc, x2, y2, z2, xy, yz, zx, xs, ys, zs;
    float mat[] = { 0.0f, 0.0f, 0.0f, 0.0f,
		    0.0f, 0.0f, 0.0f, 0.0f,
		    0.0f, 0.0f, 0.0f, 0.0f,
		    0.0f, 0.0f, 0.0f, 1.0f };
    s  = sinf ( a = (angle * (float)M_PI / 180.0f) );
    cc = 1.0f - (c = cosf ( a ));
    x /= n;     y /= n;     z /= n;
    x2 = x * x; y2 = y * y; z2 = z * z;
    xy = x * y; yz = y * z; zx = z * x;
    xs = x * s; ys = y * s; zs = z * s;
    mat[0]  = (cc * x2) + c;
    mat[1]  = (cc * xy) - zs;
    mat[2]  = (cc * zx) + ys;
    /* mat[3]  = 0.0f; */
    mat[4]  = (cc * xy) + zs;
    mat[5]  = (cc * y2) + c;
    mat[6]  = (cc * yz) - xs;
    /* mat[7]  = 0.0f; */
    mat[8]  = (cc * zx) - ys;
    mat[9]  = (cc * yz) + xs;
    mat[10] = (cc * z2) + c;
    /* mat[11] = 0.0f; */
    /* mat[12] = 0.0f; mat[= 0.0f; mat[14] = 0.0f; mat[15] = 1.0f; */
    multMatrix(m, mat);
  }
}

/*!\brief intersection triangle-cube unitaire, à compléter (voir le
 * todo du fichier et le commentaire dans le code) */
void clip2UnitCube(triangle_t * t) {
    int i, oleft = 0, oright = 0, obottom = 0, otop = 0, onear = 0, ofar = 0;
    for (i = 0; i < 3; ++i) {
      if(t->v[i].state & PS_OUT_LEFT) ++oleft;
      if(t->v[i].state & PS_OUT_RIGHT) ++oright;
      if(t->v[i].state & PS_OUT_BOTTOM) ++obottom;
      if(t->v[i].state & PS_OUT_TOP) ++otop;
      if(t->v[i].state & PS_OUT_NEAR) ++onear;
      if(t->v[i].state & PS_OUT_FAR) ++ofar;
    }
    if(!(oleft | oright | obottom | otop | onear | ofar))
      return;
    if(oleft == 3 || oright == 3 || obottom == 3 || otop == 3 || onear == 3 || ofar == 3) {
      t->state |= PS_TOTALLY_OUT;
      return;
    }
    t->state |= PS_PARTIALLY_OUT;
    /* le cas PARTIALLY_OUT n'est pas réellement géré. Il serait
       nécessaire à partir d'ici de construire la liste des triangles
       qui repésentent l'intersection entre le triangle d'origine et
       le cube unitaire. Ceci permettrait de ne plus avoir besoin de
       tester si le pixel produit par le raster est bien dans le
       "screen" avant d'écrire ; et aussi de se passer du "hack"
       PS_TOO_FAR qui est problématique.  Vous pouvez vous inspirer de
       ce qui est fait là :
       https://github.com/erich666/GraphicsGems/blob/master/gems/PolyScan/poly_clip.c
       en le ramenant au cas d'un triangle.
    */
}
