#version 330
uniform vec4 lumPos;
uniform int phong, normalMap, nightDay, specular;
uniform vec2 steps;
uniform sampler2D eday, enight, ecloud, egloss, ebump;
in  vec3 gsoNormal;
in  vec3 gsoModPos;
in  float gsoIdiffus;
in  vec2 gsoTexCoord;
out vec4 fragColor;

vec2 G[9] = vec2[9](vec2(-1, -1), vec2(0, -2), vec2(1, -1),
		    vec2(-2,  0), vec2(0,  0), vec2(2, 0),
		    vec2(-1,  1), vec2(0,  2), vec2(1,  1));

vec2 offset[9] = vec2[9](vec2(-steps.x,  steps.y), vec2(0,  steps.y), vec2(steps.x,  steps.y),
			 vec2(-steps.x,  0),       vec2(0,  0),       vec2(steps.x, 0),
			 vec2(-steps.x, -steps.y), vec2(0, -steps.y), vec2(steps.x, -steps.y));

vec2 sobel(sampler2D map) {
  vec2 g = vec2(0);
  for(int i = 0; i < 9; i++)
    g += G[i] * texture(map, gsoTexCoord + offset[i]).r;
  return g;
}

void main(void) {
  const vec4 lum_diffus = vec4(1, 1, 0.9, 1);
  const vec4 lum_amb = vec4(0.8, 0.8, 1, 1);
  const vec4 lum_spec = vec4(1, 1, 0.75, 1);
  const float Iamb = 0.15;
  /* Lumière vers sommet */
  vec3 L = normalize(gsoModPos - lumPos.xyz);
  float Idiffuse = 0, Ispec = 0;
  vec4 color = vec4(1);
  vec3 N = normalize(gsoNormal);
  if(phong != 0) {
    vec3 B = cross(normalize(vec3(N.x, 0, N.z)), vec3(0, 1, 0));
    vec3 T = cross(N, B);
    if(normalMap != 0) {
      vec2 v = 0.75 * sobel(ebump);
      N = normalize(N + v.x * B + v.y * T);
    }
    Idiffuse = clamp(dot(N, -L), 0, 1);
  } else
    Idiffuse = gsoIdiffus;
  if(specular != 0) {
    /* vecteur vue */
    vec3 V = vec3(0, 0, -1);
    /* reflet de L par rapport à la normale au fragment */
    vec3 R = reflect(L, N);
    /* Intensité de speculaire */
    Ispec = (0.3 + 0.7 * texture(egloss, gsoTexCoord).r) * pow(clamp(dot(R, -V), 0, 1), 10);
  }
  if(nightDay != 0)
    color = mix(texture(enight, gsoTexCoord), texture(eday, gsoTexCoord), Idiffuse);
  else
    color = vec4(1.0);
  fragColor = lum_diffus * color * Idiffuse + lum_amb * Iamb * color + lum_spec * Ispec;
}
