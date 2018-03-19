#version 330
uniform vec4 lumPos;
uniform int phong;
in  vec3 gsoNormal;
in  vec3 gsoModPos;
in  float gsoIdiffus;
in  vec2 gsoTexCoord;
out vec4 fragColor;

void main(void) {
  const vec4 lum_diffus = vec4(1, 0.5, 0.5, 1.0);
  const vec4 lum_amb = vec4(0.5, 0.5, 1, 1.0);
  const vec4 lum_spec = vec4(1);
  const float IlumAmb = 0.15;
  if(phong != 0) {
    /* vecteur vue */
    vec3 V = vec3(0, 0, -1);
    /* Lumière vers sommet */
    vec3 L = normalize(gsoModPos - lumPos.xyz);
    /* reflet de L par rapport à la normale au fragment */
    vec3 R = reflect(L, gsoNormal);
    /* Intensité de speculaire */
    float Ispec = pow(clamp(dot(R, -V), 0, 1), 10);
    /* Intensité lumière diffuse : Phong par rapport à la normale au fragment */
    float IdiffusPhong = dot(gsoNormal, -L);
    fragColor = lum_diffus * IdiffusPhong + lum_amb * IlumAmb + lum_spec * Ispec;
  } else
    fragColor = lum_diffus * gsoIdiffus + lum_amb * IlumAmb;
}
