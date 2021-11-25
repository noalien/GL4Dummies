/*!\file geometry.h 
 *
 * \brief quelques surfaces basiques sous forme polygonale : un plan
 * (quad), un cube et une sphere.
 *
 * \author Farès BELHADJ, amsi@up8.edu
 * \date December 2, 2020. 
*/
#include "moteur.h"
#include <assert.h>
#if defined(_MSC_VER)
#  define _USE_MATH_DEFINES
#endif
#include <math.h>

/*!\brief fabrique et renvoie une surface représentant un
 * quadrilatère "debout" et à la profondeur 0. Il fait la hauteur et
 * la largeur du cube unitaire (-1 à 1).*/
surface_t * mkQuad(void) {
  static const float
    data[] = {
	      -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,  0.0f,
	       1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,  0.0f,
	      -1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f,
	       1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, -1.0f
  };
  static const int order[] = { 0, 1, 2, 2, 1, 3 };
  surface_t * s;
  /* on met du jaune partout */
  const vec4 color0 = { 1.0f, 1.0f, 0.0f, 1.0f }; 
  triangle_t t[2];
  int i, j, k, o;
  for(i = 0, o = 0; i < 2; ++i)
    for(j = 0; j < 3; ++j, ++o) {
      k = order[o] * 8;
      t[i].v[j].position = *(vec4 *)&(data[k]);
      t[i].v[j].position.w = 1.0f;
      t[i].v[j].normal   = *(vec3 *)&(data[k + 3]);
      t[i].v[j].texCoord = *(vec2 *)&(data[k + 6]);
      t[i].v[j].color0   = color0;
    }
  s = newSurface(t, 2, 1, 1);
  snormals(s);
  return s;
}

/*!\brief fabrique et renvoie une surface représentant un
 * cube unitaire (de -1 à 1).*/
surface_t * mkCube(void) {
  const float
    data[] = {
	      /* front */
	      -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	      1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
	      -1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
	      1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
	      /* back */
	      1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
	      -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
	      1.0f,  1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
	      -1.0f,  1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
	      /* right */
	      1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
	      1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	      1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	      1.0f,  1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
	      /* left */
	      -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
	      -1.0f, -1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	      -1.0f,  1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	      -1.0f,  1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
	      /* top */
	      -1.0f, 1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
	      1.0f, 1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
	      -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	      1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
	      /* bottom */
	      -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
	      1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	      -1.0f, -1.0f,  1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
	      1.0f, -1.0f,  1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f
  };
  const int order[] = { 0, 1, 2, 2, 1, 3 };
  surface_t * s;
  /* on met du vert-clair partout */
  const vec4 color0 = { 0.5f, 1.0f, 0.0f, 1.0f }; 
  triangle_t t[12];
  int i, j, k, o;
  for(i = 0, o = 0; i < 12; ++i)
    for(j = 0; j < 3; ++j, ++o) {
      k = 8 * (order[o % 6] + 4 * (i / 2));
      t[i].v[j].position = *(vec4 *)&(data[k]);
      t[i].v[j].position.w = 1.0f;
      t[i].v[j].normal   = *(vec3 *)&(data[k + 3]);
      t[i].v[j].texCoord = *(vec2 *)&(data[k + 6]);
      t[i].v[j].color0   = color0;
    }
  s = newSurface(t, 12, 1, 1);
  snormals(s);
  return s;
}

/*!\brief fabrique et renvoie une surface représentant une sphère
 * centrée en zéro et de rayon 1. Elle est découpée en \a longitudes
 * longitudes et \a latitudes latitudes. */
surface_t * mkSphere(int longitudes, int latitudes) {
  triangle_t * t;
  vertex_t * data;
  double phi, theta, r, y;
  double c2MPI_Long = 2.0 * M_PI / longitudes;
  double cMPI_Lat = M_PI / latitudes;
  /* on met du vert-clair partout */
  const vec4 color0 = { 0.5f, 1.0f, 0.0f, 1.0f }; 
  int z, nz, x, nx, zw, nzw, k, n = 2 * longitudes * latitudes;
  assert(n);
  data = malloc((longitudes + 1) * (latitudes + 1) * sizeof *data);
  assert(data);
  t = malloc(n * sizeof *t);
  assert(t);
  for(z = 0, k = 0; z <= latitudes; ++z) {
    theta  = -M_PI_2 + z * cMPI_Lat;
    y = sin(theta);
    r = cos(theta);
    for(x = 0; x <= longitudes; ++x, ++k) {
      phi = x * c2MPI_Long;
      data[k].position.x = r * cos(phi);
      data[k].position.y = y;
      data[k].position.z = r * sin(phi);
      data[k].position.w = 1.0f;
      data[k].texCoord.x = phi / (2.0 * M_PI);
      data[k].texCoord.y = (theta + M_PI_2) / M_PI;
      data[k].color0     = color0;
      /* gcc 7.5 et plus abusent : data[k].normal     = *(vec3 *)&(data[k].position); */
      data[k].normal.x   = data[k].position.x;
      data[k].normal.y   = data[k].position.y;
      data[k].normal.z   = data[k].position.z;
    }
  }
  for(z = 0, k = 0; z < latitudes; ++z) {
    nz = z + 1;
    zw = z * (longitudes + 1);
    nzw = nz * (longitudes + 1);
    for(x = 0; x < longitudes; ++x) {
      nx = x + 1;
      t[k].v[0] = data[zw  +  x];
      t[k].v[1] = data[nzw +  x];
      t[k].v[2] = data[zw  + nx];
      tnormal(&t[k]);++k;
      t[k].v[0] = data[zw  + nx];
      t[k].v[1] = data[nzw +  x];
      t[k].v[2] = data[nzw + nx];
      tnormal(&t[k]);++k;
    }
  }
  free(data);
  return newSurface(t, n, 0, 1);
}
