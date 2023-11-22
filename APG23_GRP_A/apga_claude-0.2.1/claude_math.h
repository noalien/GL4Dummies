/* quelques fonctions qui gèrent les matrices et vecteurs */

#ifndef CLAUDE_MATH_H
#define CLAUDE_MATH_H

#ifdef __cplusplus
extern "C" {
#endif

  typedef float mat4[16];
  typedef float vec4[4];
  
  /*!\brief Multiplication de deux matrices 4x4. Les matrices \a a et \a
   * b sont multipliées et le résultat est stocké dans \a r. */
  static inline void mat4mult(float * r, const float * a, const float * b) {
    r[0]  = a[0]  * b[0] + a[1]  * b[4] + a[2]  * b[8]  + a[3]  * b[12];
    r[1]  = a[0]  * b[1] + a[1]  * b[5] + a[2]  * b[9]  + a[3]  * b[13];
    r[2]  = a[0]  * b[2] + a[1]  * b[6] + a[2]  * b[10] + a[3]  * b[14];
    r[3]  = a[0]  * b[3] + a[1]  * b[7] + a[2]  * b[11] + a[3]  * b[15];
    r[4]  = a[4]  * b[0] + a[5]  * b[4] + a[6]  * b[8]  + a[7]  * b[12];
    r[5]  = a[4]  * b[1] + a[5]  * b[5] + a[6]  * b[9]  + a[7]  * b[13];
    r[6]  = a[4]  * b[2] + a[5]  * b[6] + a[6]  * b[10] + a[7]  * b[14];
    r[7]  = a[4]  * b[3] + a[5]  * b[7] + a[6]  * b[11] + a[7]  * b[15];
    r[8]  = a[8]  * b[0] + a[9]  * b[4] + a[10] * b[8]  + a[11] * b[12];
    r[9]  = a[8]  * b[1] + a[9]  * b[5] + a[10] * b[9]  + a[11] * b[13];
    r[10] = a[8]  * b[2] + a[9]  * b[6] + a[10] * b[10] + a[11] * b[14];
    r[11] = a[8]  * b[3] + a[9]  * b[7] + a[10] * b[11] + a[11] * b[15];
    r[12] = a[12] * b[0] + a[13] * b[4] + a[14] * b[8]  + a[15] * b[12];
    r[13] = a[12] * b[1] + a[13] * b[5] + a[14] * b[9]  + a[15] * b[13];
    r[14] = a[12] * b[2] + a[13] * b[6] + a[14] * b[10] + a[15] * b[14];
    r[15] = a[12] * b[3] + a[13] * b[7] + a[14] * b[11] + a[15] * b[15];
  }
  
  /*!\brief Multiplication d'une matrices 4x4 par un vecteur. La matrice
   * \a m et le vecteur \a v sont multipliés et le résultat est stocké
   * dans le vecteur \a r. */
  static inline void mat4vec4mult(float * r, const float * m, const float * v) {
    r[0]  = m[0]  * v[0] + m[1]  * v[1] + m[2]  * v[2]  + m[3]  * v[3];
    r[1]  = m[4]  * v[0] + m[5]  * v[1] + m[6]  * v[2]  + m[7]  * v[3];
    r[2]  = m[8]  * v[0] + m[9]  * v[1] + m[10] * v[2]  + m[11] * v[3];
    r[3]  = m[12] * v[0] + m[13] * v[1] + m[14] * v[2]  + m[15] * v[3];
  }
  
  /*!\brief Charge la matrice 4x4 identité. */
  static inline void mat4identite(float * m) {
    memset(m, 0, 16 * sizeof *m);
    m[0] = m[5] = m[10] = m[15] = 1.0f;
  }

  /*!\brief ajoute (multiplication droite) une translation à la
   * matrice \a m */
  static inline void translate(float * m, float tx, float ty, float tz) {
    mat4 t = { 1.0f, 0.0f, 0.0f, tx,
	       0.0f, 1.0f, 0.0f, ty,
	       0.0f, 0.0f, 1.0f, tz,
	       0.0f, 0.0f, 0.0f, 1.0f };
    mat4 cpy;
    memcpy(cpy, m, sizeof cpy);
    mat4mult(m, cpy, t);
}

  /*!\brief ajoute (multiplication droite) une rotation à la matrice
   * \a m */
  static inline void rotate(float * m, float angle, float x, float y, float z) {
    float n = sqrtf(x * x + y * y + z * z);
    if ( n > 0.0f ) {
      float a, s, c, cc, x2, y2, z2, xy, yz, zx, xs, ys, zs;
      mat4 r = { 0.0f, 0.0f, 0.0f, 0.0f,
		 0.0f, 0.0f, 0.0f, 0.0f,
		 0.0f, 0.0f, 0.0f, 0.0f,
		 0.0f, 0.0f, 0.0f, 1.0f };
      s  = sinf ( a = (angle * (float)M_PI / 180.0f) );
      cc = 1.0f - (c = cosf ( a ));
      x /= n;     y /= n;     z /= n;
      x2 = x * x; y2 = y * y; z2 = z * z;
      xy = x * y; yz = y * z; zx = z * x;
      xs = x * s; ys = y * s; zs = z * s;
      r[0]  = (cc * x2) + c;
      r[1]  = (cc * xy) - zs;
      r[2]  = (cc * zx) + ys;
      /* r[3]  = 0.0f; */
      r[4]  = (cc * xy) + zs;
      r[5]  = (cc * y2) + c;
      r[6]  = (cc * yz) - xs;
      /* r[7]  = 0.0f; */
      r[8]  = (cc * zx) - ys;
      r[9]  = (cc * yz) + xs;
      r[10] = (cc * z2) + c;
      /* r[11] = 0.0f; */
      /* r[12] = 0.0f; r[= 0.0f; r[14] = 0.0f; r[15] = 1.0f; */
      mat4 cpy;
      memcpy(cpy, m, sizeof cpy);
      mat4mult(m, cpy, r);
    }
  }


  /*!\brief Création d'une matrice de projection en perspective selon
   * l'ancienne fonction glFrustum. */
  static inline void frustum(mat4 mat, float l, float r, float b, float t, float n, float f) {
    (mat)[0]  = 2.0 * (n) / ((r) - (l));
    (mat)[1]  = 0.0;			
    (mat)[2]  = ((r) + (l)) / ((r) - (l));
    (mat)[3]  = 0.0;			
    (mat)[4]  = 0.0;			
    (mat)[5]  = 2.0 * (n) / ((t) - (b));
    (mat)[6]  = ((t) + (b)) / ((t) - (b));
    (mat)[7]  = 0.0;			
    (mat)[8]  = 0.0;			
    (mat)[9]  = 0.0;			
    (mat)[10] = -((f) + (n)) / ((f) - (n));
    (mat)[11] = -2.0 * (f) * (n) / ((f) - (n));
    (mat)[12] = 0.0;			
    (mat)[13] = 0.0;			
    (mat)[14] = -1.0;			
    (mat)[15] = 0.0;			
  }

  
  
#ifdef __cplusplus
}
#endif
  
#endif /* du #ifndef CLAUDE_MATH_H */
