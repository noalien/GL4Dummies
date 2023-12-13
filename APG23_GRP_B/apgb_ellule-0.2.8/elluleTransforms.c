#include "ellule.h"
#include "elluleRaster.h"
#include "elluleMaths.h"

static int _viewport[4] = { 0, 0, 1, 1 };

/* La lumière est donnée par une constante, elle est positionnelle et fixe dans la scène.
 *
 * TODO : vous pouvez la rendre modifiable, voire aussi pouvoir la
 * placer par rapport aux objets de la scène, elle serait elle-même un
 * objet de la scène et subirait la matrice modèle. */
const vec4 _lp = { 0.0f, 0.0f, 1.0f, 1.0f };


void elSetViewport(int * vp) {
  memcpy(_viewport, vp, sizeof _viewport);
}

void elTransformations(surface_t * s, mat4 M, mat4 V, mat4 P) {
  mat4 mv, mvp, timv;
  /* la matrice model-view */
  _mat4mult(mv, V, M);

  /* timv est la transposée inverse de model-view, utile pour les
   * transformations des vecteurs normaux utilisés pour l'éclairage */
  memcpy(timv, mv, sizeof timv);
  _mat4inverse(timv);
  _mat4transpose(timv);
  
  /* la matrice model-view-projection */
  _mat4mult(mvp, P, mv);

  for(int i = 0; i < s->n; ++i) {
    for(int j = 0; j < 3; ++j) {
      vertex_t * sommet = &(s->triangles[i].v[j]);
      _mat4vec4mult(sommet->mvppos,   mvp, sommet->position);
      _mat4vec4mult(sommet->mvpos,     mv, sommet->position);
      sommet->mvppos[0] /= sommet->mvppos[3];
      sommet->mvppos[1] /= sommet->mvppos[3];
      sommet->mvppos[2] /= sommet->mvppos[3];
      sommet->mvppos[3]  = 1.0f;
      /* Partie liée à l'éclairage par Gouraud */
      /* on commence par calculer la direction à la lumière */
      vec3 dl = {_lp[0] - sommet->mvpos[0],
		 _lp[1] - sommet->mvpos[1],
		 _lp[2] - sommet->mvpos[2]};
      /* récupération de la normale, un 1 en w entraine la non prise
       * en compte des translations */
      vec4 n = { sommet->normal[0], sommet->normal[1], sommet->normal[2], 0.0f }, ntimv;
      /* on normalise la direction à la lumière */
      _vec3normalize(dl);
      /* la normale tranformée par timv puis normalisée */
      _mat4vec4mult(ntimv, timv, n);
      _vec3normalize(ntimv);
      /* on calcule le lambertien (produit scalaire) */
      sommet->il = _vec3dot(ntimv, dl);
      /* on le clampe dans [0, 1] et on le mélange à 75-25 avec l'ambient */
      sommet->il = _max(_min(1.0f, sommet->il), 0.0f) * 0.75f + 0.25f; 
    }
  }
}

void elDraw(surface_t * s) {
  for(int i = 0; i < s->n; ++i) {
    if(elIsEnabled(EL_BACKFACE_CULLING)) {
      vec3 a = { s->triangles[i].v[1].mvppos[0] - s->triangles[i].v[0].mvppos[0],
		 s->triangles[i].v[1].mvppos[1] - s->triangles[i].v[0].mvppos[1],
		 s->triangles[i].v[1].mvppos[2] - s->triangles[i].v[0].mvppos[2],
      };
      vec3 b = { s->triangles[i].v[2].mvppos[0] - s->triangles[i].v[1].mvppos[0],
		 s->triangles[i].v[2].mvppos[1] - s->triangles[i].v[1].mvppos[1],
		 s->triangles[i].v[2].mvppos[2] - s->triangles[i].v[1].mvppos[2],
      };
      vec3 c;
      _vec3cross(c, a, b);
      if(c[2] < 0.0f)
	continue;
    }
    for(int j = 0; j < 3; ++j) {
      s->triangles[i].v[j].iCoord[0] = _viewport[0] + (int)(((s->triangles[i].v[j].mvppos[0] + 1.0f) / 2.0f) * (_viewport[2] - EL_EPSILON));
      s->triangles[i].v[j].iCoord[1] = _viewport[1] + (int)(((s->triangles[i].v[j].mvppos[1] + 1.0f) / 2.0f) * (_viewport[3] - EL_EPSILON));
      /* on transforme le z du mvppos en depth */
      /* depth = l'inverse du z, passé par bias, puis dé-linéarisé pour mieux exploiter le type float entre 0 et 1 */
      s->triangles[i].v[j].mvppos[2] = pow( (-s->triangles[i].v[j].mvppos[2] + 1.0f) / 2.0f, 0.5f );
    }
    elFillTriangle(s, &(s->triangles[i]));
  }
}
