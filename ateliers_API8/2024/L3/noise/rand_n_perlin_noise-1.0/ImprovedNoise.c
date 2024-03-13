/**
   Improved version of {@link Noise}.
   <p>
   Algorithm:<br>
   Find unit cube that contains point (x, y, z).<br>
   Find the relative x,y,z of the point in the cube.<br>
   Compute the fade curves for each of x,y,z.<br>
   Hash coordinates of the eight cube corners and add blended results from the
   eight corners of the cube.<br>
   Return the sum.<br>
   @author Ken Perlin 2001
*/

/**
   Computes the Perlin noise function value at the point (x, y, z).
   @param x x coordinate
   @param y y coordinate
   @param z z coordinate
   @return the noise function value at (x, y, z)
*/

#include <math.h>
#include <stdlib.h>
#include <time.h>

#define PERM_SIZE 256

static int p[PERM_SIZE<<1];
static int permutation[PERM_SIZE];

static double fade(double t) {
  return t * t * t * (t * (t * 6 - 15) + 10);
}

static double lerp(double t, double a, double b) {
  return a + t * (b - a);
}

static double grad(int hash, double x, double y, double z) {
  int h = hash & 15; // CONVERT LO 4 BITS OF HASH CODE
  double u = h < 8 || h == 12 || h == 13 ? x : y, // INTO 12 GRADIENT DIRECTIONS.
    v = h < 4 || h == 12 || h == 13 ? y : z;
  return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

static void initp (void) {
  int i;
  srand(time(NULL));
  for (i = 0; i < PERM_SIZE; i++) {
    permutation[i] = PERM_SIZE * (rand() / (RAND_MAX + 1.));
    p[PERM_SIZE + i] = p[i] = permutation[i];
  }
}

extern double noise(double x, double y, double z) {
  int X, Y, Z, A, AA, AB, B, BA, BB;
  double u, v, w;
  static int firstTime = 1;
  if(firstTime) {
    initp();
    firstTime = 0;
  }

  X = ((int)floor(x)) & (PERM_SIZE - 1); // FIND UNIT CUBE THAT
  Y = ((int)floor(y)) & (PERM_SIZE - 1); // CONTAINS POINT.
  Z = ((int)floor(z)) & (PERM_SIZE - 1);
  x -= floor(x); // FIND RELATIVE X,Y,Z
  y -= floor(y); // OF POINT IN CUBE.
  z -= floor(z);
  u = fade(x); // COMPUTE FADE CURVES
  v = fade(y); // FOR EACH OF X,Y,Z.
  w = fade(z);
  A = p[X] + Y; AA = p[A] + Z; AB = p[A + 1] + Z; // HASH COORDINATES OF
  B = p[X + 1] + Y; BA = p[B] + Z; BB = p[B + 1] + Z; // THE 8 CUBE CORNERS,
  
  return lerp(w, lerp(v, lerp(u, grad(p[AA], x, y, z), // AND ADD
			      grad(p[BA], x - 1, y, z)), // BLENDED
		      lerp(u, grad(p[AB], x, y - 1, z), // RESULTS
			   grad(p[BB], x - 1, y - 1, z))), // FROM  8
	      lerp(v, lerp(u, grad(p[AA + 1], x, y, z - 1), // CORNERS
			   grad(p[BA + 1], x - 1, y, z - 1)), // OF CUBE
		   lerp(u, grad(p[AB + 1], x, y - 1, z - 1), grad(p[BB + 1], x - 1, y - 1, z - 1))));
}
