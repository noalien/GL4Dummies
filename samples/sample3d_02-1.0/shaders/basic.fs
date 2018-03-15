#version 330
uniform vec4 lumPos;
uniform int phong;
in  vec3 gsoNormal;
in  vec3 gsoModPos;
in  float gsoIdiffus;
out vec4 fragColor;

void main(void) {
  const vec4 lum_diffus = vec4(1, 0.5, 0.5, 1.0);
  const vec4 lum_amb = vec4(0.5, 0.5, 1, 1.0);
  const float IlumAmb = 0.15;
  if(phong != 0) {
    /* Lumière vers sommet */
    vec3 L = normalize(gsoModPos - lumPos.xyz);
    /* Intensité lumière diffuse : Phong par rapport à la normale au fragment */
    float IdiffusPhong = dot(gsoNormal, -L);
    fragColor = lum_diffus * IdiffusPhong + lum_amb * IlumAmb;
  } else
    fragColor = lum_diffus * gsoIdiffus + lum_amb * IlumAmb;
}
