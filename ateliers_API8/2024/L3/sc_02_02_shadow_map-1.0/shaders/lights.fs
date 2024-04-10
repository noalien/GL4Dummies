#version 330

in vec4 mvpos;
in vec3 N;
in vec3 T;
in vec2 texCoord;
in vec4 vsoSMCoord;

out vec4 fragColor;

uniform vec4 scolor;
uniform vec4 lcolor;
uniform vec4 lumpos;
uniform sampler2D smTex;

void main() {
  vec3 n = normalize(N);
  vec4 ambient = 0.15f * lcolor * scolor; /* lacolor, sacolor */
  vec3 Ld = normalize(mvpos.xyz - lumpos.xyz);
  float ild = clamp(dot(n, -Ld), 0.0, 1.0);
  vec4 diffus = (ild * lcolor) * scolor; /* ldcolor, sdcolor */
  vec3 R = normalize(reflect(Ld, n));
  vec3 Vue = -normalize(mvpos.xyz);
  float ils = pow(clamp(dot(R, Vue), 0, 1), 20);
  vec4 spec = ils * lcolor; /* sscolor */
  vec3 projCoords = vsoSMCoord.xyz / vsoSMCoord.w;
  if(texture(smTex, projCoords.xy).r  <  projCoords.z) {
    diffus *= 0.0;
    spec *= 0.0;
  }
  fragColor = ambient + diffus + spec;
}
