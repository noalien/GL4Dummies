/*!\file gl4dm.inl
 * \brief Fonctions de gestion de matrix (inline).
 * \author Kevin HAVRANEK mzartek@hotmail.fr
 * \date Mars 21, 2015
 *
*/

INLINE GL4DMMatrix gl4dmMatrixXMatrix (GL4DMMatrix mat1, GL4DMMatrix mat2) {
  GL4DMMatrix res;
  
  res.r[0].x = mat1.r[0].x * mat2.r[0].x + mat1.r[0].y * mat2.r[1].x + mat1.r[0].z * mat2.r[2].x + mat1.r[0].w * mat2.r[3].x;
  res.r[0].y = mat1.r[0].x * mat2.r[0].y + mat1.r[0].y * mat2.r[1].y + mat1.r[0].z * mat2.r[2].y + mat1.r[0].w * mat2.r[3].y;
  res.r[0].z = mat1.r[0].x * mat2.r[0].z + mat1.r[0].y * mat2.r[1].z + mat1.r[0].z * mat2.r[2].z + mat1.r[0].w * mat2.r[3].z;
  res.r[0].w = mat1.r[0].x * mat2.r[0].w + mat1.r[0].y * mat2.r[1].w + mat1.r[0].z * mat2.r[2].w + mat1.r[0].w * mat2.r[3].w;
  
  res.r[1].x = mat1.r[1].x * mat2.r[0].x + mat1.r[1].y * mat2.r[1].x + mat1.r[1].z * mat2.r[2].x + mat1.r[1].w * mat2.r[3].x;
  res.r[1].y = mat1.r[1].x * mat2.r[0].y + mat1.r[1].y * mat2.r[1].y + mat1.r[1].z * mat2.r[2].y + mat1.r[1].w * mat2.r[3].y;
  res.r[1].z = mat1.r[1].x * mat2.r[0].z + mat1.r[1].y * mat2.r[1].z + mat1.r[1].z * mat2.r[2].z + mat1.r[1].w * mat2.r[3].z;
  res.r[1].w = mat1.r[1].x * mat2.r[0].w + mat1.r[1].y * mat2.r[1].w + mat1.r[1].z * mat2.r[2].w + mat1.r[1].w * mat2.r[3].w;
  
  res.r[2].x = mat1.r[2].x * mat2.r[0].x + mat1.r[2].y * mat2.r[1].x + mat1.r[2].z * mat2.r[2].x + mat1.r[2].w * mat2.r[3].x;
  res.r[2].y = mat1.r[2].x * mat2.r[0].y + mat1.r[2].y * mat2.r[1].y + mat1.r[2].z * mat2.r[2].y + mat1.r[2].w * mat2.r[3].y;
  res.r[2].z = mat1.r[2].x * mat2.r[0].z + mat1.r[2].y * mat2.r[1].z + mat1.r[2].z * mat2.r[2].z + mat1.r[2].w * mat2.r[3].z;
  res.r[2].w = mat1.r[2].x * mat2.r[0].w + mat1.r[2].y * mat2.r[1].w + mat1.r[2].z * mat2.r[2].w + mat1.r[2].w * mat2.r[3].w;
  
  res.r[3].x = mat1.r[3].x * mat2.r[0].x + mat1.r[3].y * mat2.r[1].x + mat1.r[3].z * mat2.r[2].x + mat1.r[3].w * mat2.r[3].x;
  res.r[3].y = mat1.r[3].x * mat2.r[0].y + mat1.r[3].y * mat2.r[1].y + mat1.r[3].z * mat2.r[2].y + mat1.r[3].w * mat2.r[3].y;
  res.r[3].z = mat1.r[3].x * mat2.r[0].z + mat1.r[3].y * mat2.r[1].z + mat1.r[3].z * mat2.r[2].z + mat1.r[3].w * mat2.r[3].z;
  res.r[3].w = mat1.r[3].x * mat2.r[0].w + mat1.r[3].y * mat2.r[1].w + mat1.r[3].z * mat2.r[2].w + mat1.r[3].w * mat2.r[3].w;

  return res;
}

INLINE GL4DMVector gl4dmMatrixXVector(GL4DMMatrix mat, GL4DMVector vec) {
  GL4DMVector res;

  res.x = mat.r[0].x * vec.x + mat.r[0].y * vec.y + mat.r[0].z * vec.z + mat.r[0].w * vec.w;
  res.y = mat.r[1].x * vec.x + mat.r[1].y * vec.y + mat.r[1].z * vec.z + mat.r[1].w * vec.w;
  res.z = mat.r[2].x * vec.x + mat.r[2].y * vec.y + mat.r[2].z * vec.z + mat.r[2].w * vec.w;
  res.w = mat.r[3].x * vec.x + mat.r[3].y * vec.y + mat.r[3].z * vec.z + mat.r[3].w * vec.w;
  
  return res;
}

INLINE GL4DMVector gl4dmVectorXVector(GL4DMVector vec1, GL4DMVector vec2) {
  GL4DMVector res;
  
  res.x = vec1.x * vec2.x;
  res.y = vec1.y * vec2.y;
  res.z = vec1.z * vec2.z;
  res.w = vec1.w * vec2.w;
  
  return res;
}

INLINE GL4DMMatrix gl4dmMatrixTranspose(GL4DMMatrix mat) {
  GL4DMMatrix res;

  res.r[0].x = mat.r[0].x;
  res.r[0].y = mat.r[1].x;
  res.r[0].z = mat.r[2].x;
  res.r[0].w = mat.r[3].x;
  
  res.r[1].x = mat.r[0].y;
  res.r[1].y = mat.r[1].y;
  res.r[1].z = mat.r[2].y;
  res.r[1].w = mat.r[3].y;
  
  res.r[2].x = mat.r[0].z;
  res.r[2].y = mat.r[1].z;
  res.r[2].z = mat.r[2].z;
  res.r[2].w = mat.r[3].z;
  
  res.r[3].x = mat.r[0].w;
  res.r[3].y = mat.r[1].w;
  res.r[3].z = mat.r[2].w;
  res.r[3].w = mat.r[3].w;

  return res;
}
  
INLINE GL4DMVector gl4dmVector3Cross(GL4DMVector vec1, GL4DMVector vec2) {
  GL4DMVector res;
  
  res.x = vec1.y * vec2.z - vec1.z * vec2.y;
  res.y = vec1.z * vec2.x - vec1.x * vec2.z;
  res.z = vec1.x * vec2.y - vec1.y * vec2.x;
  res.w = 1.0f;
  
  return res;
}

INLINE float gl4dmVector2Dot(GL4DMVector vec1, GL4DMVector vec2) {
  return vec1.x * vec2.x + vec1.y * vec2.y;
}

INLINE float gl4dmVector3Dot(GL4DMVector vec1, GL4DMVector vec2) {
  return gl4dmVector2Dot(vec1, vec2) + vec1.z * vec2.z;
}

INLINE float gl4dmVector4Dot(GL4DMVector vec1, GL4DMVector vec2) {
  return gl4dmVector3Dot(vec1, vec2) + vec1.w * vec2.w;
}
  
INLINE GL4DMVector gl4dmVector2Normalize(GL4DMVector vec) {
  GL4DMVector res;
  float length = sqrt(SQUARE(vec.x) + SQUARE(vec.y));
  
  res.x = vec.x / length;
  res.y = vec.y / length;
  res.z = 0.0f;
  res.w = 0.0f;
  
  return res;
}
  
INLINE GL4DMVector gl4dmVector3Normalize(GL4DMVector vec) {
  GL4DMVector res;
  float length = sqrt(SQUARE(vec.x) + SQUARE(vec.y) + SQUARE(vec.z));
  
  res.x = vec.x / length;
  res.y = vec.y / length;
  res.z = vec.z / length;
  res.w = 0.0f;
  
  return res;
}
  
INLINE GL4DMVector gl4dmVector4Normalize(GL4DMVector vec) {
  GL4DMVector res;
  float length = sqrt(SQUARE(vec.x) + SQUARE(vec.y) + SQUARE(vec.z) + SQUARE(vec.w));
  
  res.x = vec.x / length;
  res.y = vec.y / length;
  res.z = vec.z / length;
  res.w = vec.w / length;
  
  return res;
}
  
INLINE GL4DMMatrix gl4dmMatrixIdentity(void) {
  GL4DMMatrix res;
  
  memset(&res, 0, sizeof res);
  
  res.r[0].x = 1.0f;
  res.r[1].y = 1.0f;
  res.r[2].z = 1.0f;
  res.r[3].w = 1.0f;
  
  return res;
}
  
INLINE GL4DMMatrix gl4dmMatrixTranslate(float x, float y, float z) {
  GL4DMMatrix res = gl4dmMatrixIdentity();
 
  res.r[0].w = x;
  res.r[1].w = y;
  res.r[2].w = z;
 
  return res;
}
  
INLINE GL4DMMatrix gl4dmMatrixRotate(float angle, float x, float y, float z) {
  GL4DMMatrix res;
  float c = cosf(angle);
  float s = sinf(angle);
  GL4DMVector axe = {x, y, z, 1.0f};
  
  memset(&res, 0, sizeof res);
  
  axe = gl4dmVector3Normalize(axe);
  
  res.r[0].x = (SQUARE(axe.x) * (1.0f - c)) + c;
  res.r[0].y = (axe.x * axe.y * (1.0f - c)) - (axe.z * s);
  res.r[0].z = (axe.x * axe.z * (1.0f - c)) + (axe.y * s);
  res.r[1].x = (axe.y * axe.x * (1.0f - c)) + (axe.z * s);
  res.r[1].y = (SQUARE(axe.y) * (1.0f - c)) + c;
  res.r[1].z = (axe.y * axe.z * (1.0f - c)) - (axe.x * s);
  res.r[2].x = (axe.x * axe.z * (1.0f - c)) - (axe.y * s);
  res.r[2].y = (axe.y * axe.z * (1.0f - c)) + (axe.x * s);
  res.r[2].z = (SQUARE(axe.z) * (1.0f - c)) + c;
  res.r[3].w = 1.0f;
  
  return res;
}
  
INLINE GL4DMMatrix gl4dmMatrixScale(float x, float y, float z) {
  GL4DMMatrix res;
 
  memset(&res, 0, sizeof res);
 
  res.r[0].x = x;
  res.r[1].y = y;
  res.r[2].z = z;
  res.r[3].w = 1.0f;
 
  return res;
}
  
INLINE GL4DMMatrix gl4dmMatrixLookAt(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ) {
  GL4DMMatrix res;
  GL4DMVector f = {centerX - eyeX, centerY - eyeY, centerZ - eyeZ, 1.0f};
  GL4DMVector up = {upX, upY, upZ, 1.0f};
  GL4DMVector s;
  GL4DMVector u;
  
  memset(&res, 0, sizeof res);
  
  f = gl4dmVector3Normalize(f);
  up = gl4dmVector3Normalize(up);
  s = gl4dmVector3Normalize(gl4dmVector3Cross(f, up));
  u = gl4dmVector3Cross(s, f);
  
  res.r[0].x = s.x;
  res.r[0].y = s.y;
  res.r[0].z = s.z;
  res.r[1].x = u.x;
  res.r[1].y = u.y;
  res.r[1].z = u.z;
  res.r[2].x = -f.x;
  res.r[2].y = -f.y;
  res.r[2].z = -f.z;
  res.r[3].w = 1.0f;
  
  res = gl4dmMatrixXMatrix(res, gl4dmMatrixTranslate(-eyeX, -eyeY, -eyeZ));
  
  return res;
}
  
INLINE GL4DMMatrix gl4dmMatrixPerspective(float fovy, float aspect, float znear, float zfar) {
  GL4DMMatrix res;
  float f = COTANGENT(fovy / 2.0f);
 
  memset(&res, 0, sizeof res);
 
  res.r[0].x = f / aspect;
  res.r[1].y = f;
  res.r[2].z = (zfar + znear) / (znear - zfar);
  res.r[2].w = (2.0f * zfar * znear) / (znear - zfar);
  res.r[3].z = -1.0f;
 
  return res;
}
  
INLINE GL4DMMatrix gl4dmMatrixOrtho(float left, float right, float bottom, float top, float nearVal, float farVal) {
  GL4DMMatrix res;
 
  memset(&res, 0, sizeof res);
 
  res.r[0].x = 2.0f / (right - left);
  res.r[1].y = 2.0f / (top - bottom);
  res.r[2].z = -2.0f / (farVal - nearVal);
  res.r[3].w = 1.0f;
  res.r[0].w = -((right + left) / (right - left));
  res.r[1].w = -((top + bottom) / (top - bottom));
  res.r[2].w = -((farVal + nearVal) / (farVal - nearVal));
 
  return res;
}

INLINE GL4DMMatrix gl4dmMatrixInverse(GL4DMMatrix mat) {
  GL4DMMatrix res;
  float det;
  int i;
  //0
  res.r[0].x =
    mat.r[1].y * mat.r[2].z * mat.r[3].w - 
    mat.r[1].y * mat.r[3].z * mat.r[2].w - 
    mat.r[1].z * mat.r[2].y * mat.r[3].w + 
    mat.r[1].z * mat.r[3].y * mat.r[2].w +
    mat.r[1].w * mat.r[2].y * mat.r[3].z - 
    mat.r[1].w * mat.r[3].y * mat.r[2].z;

  //1
  res.r[0].y =
   -mat.r[0].y * mat.r[2].z * mat.r[3].w + 
    mat.r[0].y * mat.r[3].z * mat.r[2].w + 
    mat.r[0].z * mat.r[2].y * mat.r[3].w - 
    mat.r[0].z * mat.r[3].y * mat.r[2].w - 
    mat.r[0].w * mat.r[2].y * mat.r[3].z + 
    mat.r[0].w * mat.r[3].y * mat.r[2].z;

  //2
  res.r[0].z =
    mat.r[0].y * mat.r[1].z * mat.r[3].w - 
    mat.r[0].y * mat.r[3].z * mat.r[1].w - 
    mat.r[0].z * mat.r[1].y * mat.r[3].w + 
    mat.r[0].z * mat.r[3].y * mat.r[1].w + 
    mat.r[0].w * mat.r[1].y * mat.r[3].z - 
    mat.r[0].w * mat.r[3].y * mat.r[1].z;

  //3
  res.r[0].w =
   -mat.r[0].y * mat.r[1].z * mat.r[2].w + 
    mat.r[0].y * mat.r[2].z * mat.r[1].w +
    mat.r[0].z * mat.r[1].y * mat.r[2].w - 
    mat.r[0].z * mat.r[2].y * mat.r[1].w - 
    mat.r[0].w * mat.r[1].y * mat.r[2].z + 
    mat.r[0].w * mat.r[2].y * mat.r[1].z;

  //4
  res.r[1].x =
   -mat.r[1].x * mat.r[2].z * mat.r[3].w + 
    mat.r[1].x * mat.r[3].z * mat.r[2].w + 
    mat.r[1].z * mat.r[2].x * mat.r[3].w - 
    mat.r[1].z * mat.r[3].x * mat.r[2].w - 
    mat.r[1].w * mat.r[2].x * mat.r[3].z + 
    mat.r[1].w * mat.r[3].x * mat.r[2].z;

  //5
  res.r[1].y =
    mat.r[0].x * mat.r[2].z * mat.r[3].w - 
    mat.r[0].x * mat.r[3].z * mat.r[2].w - 
    mat.r[0].z * mat.r[2].x * mat.r[3].w + 
    mat.r[0].z * mat.r[3].x * mat.r[2].w + 
    mat.r[0].w * mat.r[2].x * mat.r[3].z - 
    mat.r[0].w * mat.r[3].x * mat.r[2].z;

  //6
  res.r[1].z =
   -mat.r[0].x * mat.r[1].z * mat.r[3].w + 
    mat.r[0].x * mat.r[3].z * mat.r[1].w + 
    mat.r[0].z * mat.r[1].x * mat.r[3].w - 
    mat.r[0].z * mat.r[3].x * mat.r[1].w - 
    mat.r[0].w * mat.r[1].x * mat.r[3].z + 
    mat.r[0].w * mat.r[3].x * mat.r[1].z;

  //7
  res.r[1].w =
    mat.r[0].x * mat.r[1].z * mat.r[2].w - 
    mat.r[0].x * mat.r[2].z * mat.r[1].w - 
    mat.r[0].z * mat.r[1].x * mat.r[2].w + 
    mat.r[0].z * mat.r[2].x * mat.r[1].w + 
    mat.r[0].w * mat.r[1].x * mat.r[2].z - 
    mat.r[0].w * mat.r[2].x * mat.r[1].z;

  //8
  res.r[2].x =
    mat.r[1].x * mat.r[2].y * mat.r[3].w - 
    mat.r[1].x * mat.r[3].y * mat.r[2].w - 
    mat.r[1].y * mat.r[2].x * mat.r[3].w + 
    mat.r[1].y * mat.r[3].x * mat.r[2].w + 
    mat.r[1].w * mat.r[2].x * mat.r[3].y - 
    mat.r[1].w * mat.r[3].x * mat.r[2].y;

  //9
  res.r[2].y =
   -mat.r[0].x * mat.r[2].y * mat.r[3].w + 
    mat.r[0].x * mat.r[3].y * mat.r[2].w + 
    mat.r[0].y * mat.r[2].x * mat.r[3].w - 
    mat.r[0].y * mat.r[3].x * mat.r[2].w - 
    mat.r[0].w * mat.r[2].x * mat.r[3].y + 
    mat.r[0].w * mat.r[3].x * mat.r[2].y;

  //10
  res.r[2].z =
    mat.r[0].x * mat.r[1].y * mat.r[3].w - 
    mat.r[0].x * mat.r[3].y * mat.r[1].w - 
    mat.r[0].y * mat.r[1].x * mat.r[3].w + 
    mat.r[0].y * mat.r[3].x * mat.r[1].w + 
    mat.r[0].w * mat.r[1].x * mat.r[3].y - 
    mat.r[0].w * mat.r[3].x * mat.r[1].y;

  //11
  res.r[2].w =
   -mat.r[0].x * mat.r[1].y * mat.r[2].w + 
    mat.r[0].x * mat.r[2].y * mat.r[1].w + 
    mat.r[0].y * mat.r[1].x * mat.r[2].w - 
    mat.r[0].y * mat.r[2].x * mat.r[1].w - 
    mat.r[0].w * mat.r[1].x * mat.r[2].y + 
    mat.r[0].w * mat.r[2].x * mat.r[1].y;

  //12
  res.r[3].x =
   -mat.r[1].x * mat.r[2].y * mat.r[3].z + 
    mat.r[1].x * mat.r[3].y * mat.r[2].z + 
    mat.r[1].y * mat.r[2].x * mat.r[3].z - 
    mat.r[1].y * mat.r[3].x * mat.r[2].z - 
    mat.r[1].z * mat.r[2].x * mat.r[3].y + 
    mat.r[1].z * mat.r[3].x * mat.r[2].y;

  //13
  res.r[3].y =
    mat.r[0].x * mat.r[2].y * mat.r[3].z - 
    mat.r[0].x * mat.r[3].y * mat.r[2].z - 
    mat.r[0].y * mat.r[2].x * mat.r[3].z + 
    mat.r[0].y * mat.r[3].x * mat.r[2].z + 
    mat.r[0].z * mat.r[2].x * mat.r[3].y - 
    mat.r[0].z * mat.r[3].x * mat.r[2].y;

  //14
  res.r[3].z =
   -mat.r[0].x * mat.r[1].y * mat.r[3].z + 
    mat.r[0].x * mat.r[3].y * mat.r[1].z + 
    mat.r[0].y * mat.r[1].x * mat.r[3].z - 
    mat.r[0].y * mat.r[3].x * mat.r[1].z - 
    mat.r[0].z * mat.r[1].x * mat.r[3].y + 
    mat.r[0].z * mat.r[3].x * mat.r[1].y;

  //15
  res.r[3].w =
    mat.r[0].x * mat.r[1].y * mat.r[2].z - 
    mat.r[0].x * mat.r[2].y * mat.r[1].z - 
    mat.r[0].y * mat.r[1].x * mat.r[2].z + 
    mat.r[0].y * mat.r[2].x * mat.r[1].z + 
    mat.r[0].z * mat.r[1].x * mat.r[2].y - 
    mat.r[0].z * mat.r[2].x * mat.r[1].y;

  det = mat.r[0].x * res.r[0].x + mat.r[1].x * res.r[0].y + mat.r[2].x * res.r[0].z + mat.r[3].x * res.r[0].w;

  if (det == 0) return mat;

  det = 1.0f / det;
  
  for (i = 0; i < 4; i++) {
    res.r[i].x *= det;
    res.r[i].y *= det;
    res.r[i].z *= det;
    res.r[i].w *= det;
  }

  return res;    
}

INLINE void gl4dmPrintVector(GL4DMVector vec) {
  printf("%f %f %f %f\n", vec.x, vec.y, vec.z, vec.w);
}

INLINE void gl4dmPrintMatrix(GL4DMMatrix mat) {
  int i;
  for (i = 0; i < 4; i++) gl4dmPrintVector(mat.r[i]);  
}
