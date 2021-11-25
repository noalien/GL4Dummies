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
 */
#include "moteur.h"
#include <assert.h>

/* fonctions locale (static) */
static inline void clip2UnitCube(triangle_t * t);

/*!\brief projette le sommet \a v à l'écran (le \a viewport) selon la
   matrice de model-view \a mvMat et de projection \a projMat. \a
   timvMat est la transposée de l'inverse de la matrice \a mvMat.*/
vertex_t vtransform(surface_t * s, vertex_t v, float * mvMat, float * timvMat, float * projMat, float * viewport) {
  float dist = 1.0f;
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
  dist = 10.0f;
  if(r2.x < -dist || r2.x > dist || r2.y < -dist || r2.y > dist || r2.z < -dist || r2.z > dist) {
    v.state |= PS_TOO_FAR;
    return v;
  }
  /* Gouraud */
  if(s->options & SO_USE_LIGHTING) {
    /* la lumière est positionnelle et fixe dans la scène. \todo dans
       scene.c la rendre modifiable, voire aussi pouvoir la placer par
       rapport aux objets (elle subirait la matrice modèle). */
    const vec4 lp[1] = { {0.0f, 0.0f, 1.0f} };
    vec4 ld = {lp[0].x - r1.x, lp[0].y - r1.y, lp[0].z - r1.z, lp[0].w - r1.w};
    float n[4] = {v.normal.x, v.normal.y, v.normal.z, 0.0f}, res[4];
    MMAT4XVEC4(res, timvMat, n);
    MVEC3NORMALIZE(res);
    MVEC3NORMALIZE((float *)&ld);
    v.li = MVEC3DOT(res, (float *)&ld);
    v.li = MIN(MAX(0.0f, v.li), 1.0f);
  } else
    v.li = 1.0f;
  v.icolor = v.color0;
  /* Mapping du cube unitaire vers l'écran */
  v.x = viewport[0] + ((r2.x + 1.0f) * 0.5f) * (viewport[2] - EPSILON);
  v.y = viewport[1] + ((r2.y + 1.0f) * 0.5f) * (viewport[3] - EPSILON);
  v.z = pow((-r2.z + 1.0f) * 0.5f, 0.5);
  /* sinon pour near = 0.1f et far = 10.0f on peut rendre non linéaire la depth avec */
  /* v.z = 1.0f - (1.0f / r2.z - 1.0f / 0.1f) / (1.0f / 10.0f - 1.0f / 0.1f); */
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
 */
void stransform(surface_t * s, float * mvMat, float * projMat, float * viewport) {
  int i, j;
  float timvMat[16];
  triangle_t vcull;
  /* calcul de la transposée de l'inverse de la matrice model-view
     pour la transformation des normales et le calcul du lambertien
     utilisé par le shading Gouraud dans vtransform. */
  memcpy(timvMat, mvMat, sizeof timvMat);
  MMAT4INVERSE(timvMat);
  MMAT4TRANSPOSE(timvMat);
  for(i = 0; i < s->n; ++i) {
    s->t[i].state = PS_NONE;
    for(j = 0; j < 3; ++j) {
      s->t[i].v[j] = vtransform(s, s->t[i].v[j], mvMat, timvMat, projMat, viewport);
      if(s->options & SO_CULL_BACKFACES) {
	vcull.v[j].position.x = s->t[i].v[j].x;
	vcull.v[j].position.y = s->t[i].v[j].y;
	vcull.v[j].position.z = 0.0f;
      }
    }
    if(s->options & SO_CULL_BACKFACES) {
      tnormal(&vcull);
      if(vcull.normal.z <= 0.0f) {
	s->t[i].state |= PS_CULL;
	continue;
      }
    }
    clip2UnitCube(&(s->t[i]));
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

/*!\brief ajoute (multiplication droite) un scale à la matrice \a m */
void scale(float * m, float sx, float sy, float sz) {
  float mat[] = { sx  , 0.0f, 0.0f, 0.0f,
		  0.0f,   sy, 0.0f, 0.0f,
		  0.0f, 0.0f,   sz, 0.0f,
		  0.0f, 0.0f, 0.0f, 1.0f };
  multMatrix(m, mat);
}

/*!\brief simule une free camera, voir la doc de gluLookAt */
void lookAt(float * m, float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ) {
  float forward[3], side[3], up[3];
  float mat[] = {
		 1.0f,       0.0f,       0.0f,       0.0f,
		 0.0f,       1.0f,       0.0f,       0.0f,
		 0.0f,       0.0f,       1.0f,       0.0f,
		 0.0f,       0.0f,       0.0f,       1.0f
  };
  forward[0] = centerX - eyeX;
  forward[1] = centerY - eyeY;
  forward[2] = centerZ - eyeZ;
  up[0] = upX;
  up[1] = upY;
  up[2] = upZ;
  MVEC3NORMALIZE(forward);
  /* side = forward x up */
  MVEC3CROSS(side, forward, up);
  MVEC3NORMALIZE(side);
  /* up = side x forward */
  MVEC3CROSS(up, side, forward);
  mat[0] = side[0];
  mat[1] = side[1];
  mat[2] = side[2];
  mat[4] = up[0];
  mat[5] = up[1];
  mat[6] = up[2];
  mat[8] = -forward[0];
  mat[9] = -forward[1];
  mat[10] = -forward[2];
  multMatrix(m, mat);
  translate(m, -eyeX, -eyeY, -eyeZ);
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
