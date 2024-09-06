/* quelques fonctions qui gèrent les matrices et vecteurs */

#ifndef ELLULE_MATHS_H
#define ELLULE_MATHS_H

#ifdef __cplusplus
extern "C" {
#endif

  typedef float mat4[16];

  /* typedef struct vec4 vec4; */
  /* typedef struct vec3 vec3; */
  /* typedef struct vec2 vec2; */
  /* typedef struct ivec2 ivec2; */

  /* struct vec4 { */
  /*   union { */
  /*     struct { float x, y, z, w; }; */
  /*     float v[4]; */
  /*   }; */
  /* }; */

  /* struct vec3 { */
  /*   union { */
  /*     struct { float x, y, z; }; */
  /*     float v[3]; */
  /*   }; */
  /* }; */

  /* struct vec2 { */
  /*   union { */
  /*     struct { float x, y; }; */
  /*     float v[2]; */
  /*   }; */
  /* }; */

  /* struct ivec2 { */
  /*   union { */
  /*     struct { int x, y, z, w; }; */
  /*     int v[2]; */
  /*   }; */
  /* }; */

  
  typedef float vec4[4];
  typedef float vec3[3];
  typedef float vec2[3];
  typedef int   ivec2[2];
  
  /*!\brief min entre deux flottants. */
  static inline float _min(float a, float b) {
    return a < b ? a : b;
  }

  /*!\brief max entre deux flottants. */
  static inline float _max(float a, float b) {
    return a > b ? a : b;
  }

  /*!\brief Produit vectoriel 3D entre \a u et \a v écrit dans \a r. */
  static inline void _vec3cross(vec3 r, vec3 u, vec3 v) {
    r[0] = u[1] * v[2] - u[2] * v[1];
    r[1] = u[2] * v[0] - u[0] * v[2];
    r[2] = u[0] * v[1] - u[1] * v[0];
  }

  /*!\brief Renvoie le produit scalaire entre deux vecteurs 3D entre \a u et \a v. */
  static inline float _vec3dot(vec3 u, vec3 v) {
    return u[0] * v[0] + u[1] * v[1] + u[2] * v[2];
  }

  /*!\brief Normalise le vecteur 3D \a v. */
  static inline void _vec3normalize(vec3 v){
    float n = sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    if(n > 0) {
      v[0] /= n;
      v[1] /= n;
      v[2] /= n;
    }
  }

  /*!\brief inverse de la matrice 4x4 \a m. */
  static inline void _mat4inverse(mat4 m) {
    mat4 tmp;
    tmp[0] = m[5] * m[10] * m[15] - m[5] * m[11] * m[14] - m[9] * m[6] * m[15] + m[9] * m[7] * m[14] + m[13] * m[6] * m[11] - m[13] * m[7] * m[10];
    tmp[4] = -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] + m[8] * m[6] * m[15] - m[8] * m[7] * m[14] - m[12] * m[6] * m[11] + m[12] * m[7] * m[10];
    tmp[8] = m[4] * m[9] * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15] + m[8] * m[7] * m[13] + m[12] * m[5] * m[11] - m[12] * m[7] * m[9];
    tmp[12] = -m[4] * m[9] * m[14] + m[4] * m[10] * m[13] + m[8] * m[5] * m[14] - m[8] * m[6] * m[13] - m[12] * m[5] * m[10] + m[12] * m[6] * m[9];
    tmp[1] = -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] + m[9] * m[2] * m[15] - m[9] * m[3] * m[14] - m[13] * m[2] * m[11] + m[13] * m[3] * m[10];
    tmp[5] = m[0] * m[10] * m[15] - m[0] * m[11] * m[14] - m[8] * m[2] * m[15] + m[8] * m[3] * m[14] + m[12] * m[2] * m[11] - m[12] * m[3] * m[10];
    tmp[9] = -m[0] * m[9] * m[15] + m[0] * m[11] * m[13] + m[8] * m[1] * m[15] - m[8] * m[3] * m[13] - m[12] * m[1] * m[11] + m[12] * m[3] * m[9];
    tmp[13] = m[0] * m[9] * m[14] - m[0] * m[10] * m[13] - m[8] * m[1] * m[14] + m[8] * m[2] * m[13] + m[12] * m[1] * m[10] - m[12] * m[2] * m[9];
    tmp[2] = m[1] * m[6] * m[15] - m[1] * m[7] * m[14] - m[5] * m[2] * m[15] + m[5] * m[3] * m[14] + m[13] * m[2] * m[7] - m[13] * m[3] * m[6];
    tmp[6] = -m[0] * m[6] * m[15] + m[0] * m[7] * m[14] + m[4] * m[2] * m[15] - m[4] * m[3] * m[14] - m[12] * m[2] * m[7] + m[12] * m[3] * m[6];
    tmp[10] = m[0] * m[5] * m[15] - m[0] * m[7] * m[13] - m[4] * m[1] * m[15] + m[4] * m[3] * m[13] + m[12] * m[1] * m[7] - m[12] * m[3] * m[5];
    tmp[14] = -m[0] * m[5] * m[14] + m[0] * m[6] * m[13] + m[4] * m[1] * m[14] - m[4] * m[2] * m[13] - m[12] * m[1] * m[6] + m[12] * m[2] * m[5];
    tmp[3] = -m[1] * m[6] * m[11] + m[1] * m[7] * m[10] + m[5] * m[2] * m[11] - m[5] * m[3] * m[10] - m[9] * m[2] * m[7] + m[9] * m[3] * m[6];
    tmp[7] = m[0] * m[6] * m[11] - m[0] * m[7] * m[10] - m[4] * m[2] * m[11] + m[4] * m[3] * m[10] + m[8] * m[2] * m[7] - m[8] * m[3] * m[6];
    tmp[11] = -m[0] * m[5] * m[11] + m[0] * m[7] * m[9] + m[4] * m[1] * m[11] - m[4] * m[3] * m[9] - m[8] * m[1] * m[7] + m[8] * m[3] * m[5];
    tmp[15] = m[0] * m[5] * m[10] - m[0] * m[6] * m[9] - m[4] * m[1] * m[10] + m[4] * m[2] * m[9] + m[8] * m[1] * m[6] - m[8] * m[2] * m[5];
    float det = m[0] * tmp[0] + m[1] * tmp[4] + m[2] * tmp[8] + m[3] * tmp[12];
    /*!\todo comment gérer det == 0.0 ? avec un assert ??? */\
    det = 1.0 / det;
    for(int i = 0; i < 16; i++)
        m[i] = tmp[i] * det;
  }

  /*!\brief Transposée de la matrice 4x4 \a m. */
  static inline void _mat4transpose(float * m) {
    float t;
    t = m[1];  m[1]  = m[4];  m[4]  = t;
    t = m[2];  m[2]  = m[8];  m[8]  = t;
    t = m[3];  m[3]  = m[12]; m[12] = t;
    t = m[6];  m[6]  = m[9];  m[9]  = t;
    t = m[7];  m[7]  = m[13]; m[13] = t;
    t = m[11]; m[11] = m[14]; m[14] = t;
  }
  
  /*!\brief Multiplication de deux matrices 4x4. Les matrices \a a et \a
   * b sont multipliées et le résultat est stocké dans \a r. */
  static inline void _mat4mult(float * r, const float * a, const float * b) {
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
  static inline void _mat4vec4mult(float * r, const float * m, const float * v) {
    r[0]  = m[0]  * v[0] + m[1]  * v[1] + m[2]  * v[2]  + m[3]  * v[3];
    r[1]  = m[4]  * v[0] + m[5]  * v[1] + m[6]  * v[2]  + m[7]  * v[3];
    r[2]  = m[8]  * v[0] + m[9]  * v[1] + m[10] * v[2]  + m[11] * v[3];
    r[3]  = m[12] * v[0] + m[13] * v[1] + m[14] * v[2]  + m[15] * v[3];
  }
  
  /*!\brief Charge la matrice 4x4 identité. */
  static inline void _mat4identite(float * m) {
    memset(m, 0, 16 * sizeof *m);
    m[0] = m[5] = m[10] = m[15] = 1.0f;
  }

  /*!\brief ajoute (multiplication droite) une translation à la
   * matrice \a m */
  static inline void _translate(float * m, float tx, float ty, float tz) {
    mat4 t = { 1.0f, 0.0f, 0.0f, tx,
	       0.0f, 1.0f, 0.0f, ty,
	       0.0f, 0.0f, 1.0f, tz,
	       0.0f, 0.0f, 0.0f, 1.0f };
    mat4 cpy;
    memcpy(cpy, m, sizeof cpy);
    _mat4mult(m, cpy, t);
  }

  /*!\brief ajoute (multiplication droite) un scale à la
   * matrice \a m */
  static inline void _scale(float * m, float sx, float sy, float sz) {
    mat4 s = { sx  , 0.0f, 0.0f, 0.0f,
	       0.0f,   sy, 0.0f, 0.0f,
	       0.0f, 0.0f,   sz, 0.0f,
	       0.0f, 0.0f, 0.0f, 1.0f };
    mat4 cpy;
    memcpy(cpy, m, sizeof cpy);
    _mat4mult(m, cpy, s);
  }

  /*!\brief ajoute (multiplication droite) une rotation à la matrice
   * \a m */
  static inline void _rotate(float * m, float angle, float x, float y, float z) {
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
      _mat4mult(m, cpy, r);
    }
  }

  /*!\brief Création d'une matrice de projection en perspective selon
   * l'ancienne fonction glFrustum. */
  static inline void _frustum(mat4 mat, float l, float r, float b, float t, float n, float f) {
    mat[0]  = 2.0f * (n) / ((r) - (l));
    mat[1]  = 0.0f;
    mat[2]  = ((r) + (l)) / ((r) - (l));
    mat[3]  = 0.0f;
    mat[4]  = 0.0f;
    mat[5]  = 2.0f * (n) / ((t) - (b));
    mat[6]  = ((t) + (b)) / ((t) - (b));
    mat[7]  = 0.0f;
    mat[8]  = 0.0f;
    mat[9]  = 0.0f;
    mat[10] = -((f) + (n)) / ((f) - (n));
    mat[11] = -2.0f * (f) * (n) / ((f) - (n));
    mat[12] = 0.0f;
    mat[13] = 0.0f;
    mat[14] = -1.0f;
    mat[15] = 0.0f;
  }

  /*!\brief Création d'une matrice de projection orthogonale selon
   * l'ancienne fonction glOrtho. */
  static inline void _ortho2D(mat4 mat, float l, float r, float b, float t, float n, float f) {
    mat[0]  = 2.0f / ((r) - (l));
    mat[1]  = 0.0f;
    mat[2]  = 0.0f;
    mat[3]  = -((r) + (l)) / ((r) - (l));
    mat[4]  = 0.0f;
    mat[5]  = 2.0f / ((t) - (b));
    mat[6]  = 0.0f;
    mat[7]  = -((t) + (b)) / ((t) - (b));
    mat[8]  = 0.0f;
    mat[9]  = 0.0f;
    mat[10] = -2.0f / ((f) - (n));
    mat[11] = -((f) + (n)) / ((f) - (n));
    mat[12] = 0.0f;
    mat[13] = 0.0f;
    mat[14] = 0.0f;
    mat[15] = 1.0f;
  }

  /*!\brief simule une free camera, voir la doc de gluLookAt */
  static inline void _lookAt(mat4 m, float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ) {
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
    _vec3normalize(forward);
    /* side = forward x up */
    _vec3cross(side, forward, up);
    _vec3normalize(side);
    /* up = side x forward */
    _vec3cross(up, side, forward);
    mat[0] = side[0];
    mat[1] = side[1];
    mat[2] = side[2];
    mat[4] = up[0];
    mat[5] = up[1];
    mat[6] = up[2];
    mat[8] = -forward[0];
    mat[9] = -forward[1];
    mat[10] = -forward[2];
    mat4 cpy;
    memcpy(cpy, m, sizeof cpy);
    _mat4mult(m, cpy, mat);
    _translate(m, -eyeX, -eyeY, -eyeZ);
  }

  
#ifdef __cplusplus
}
#endif
  
#endif /* du #ifndef ELLULE_MATHS_H */
