#version 330
in vec4 mpos;
in vec3 mnormal;
in vec2 tcoord;
in vec4 smcoord;
out vec4 fragColor;

uniform vec4 couleur;
uniform vec4 lumpos;
uniform mat4 view;
uniform sampler2D tex;
uniform sampler2D nm;
uniform sampler2D sm;
uniform int use_tex;
uniform int use_nm;

void main() {
  const vec3 V = vec3(0.0, 0.0, -1.0);
  const vec4 acouleur = vec4(0.0, 0.3, 0.0, 1.0);
  vec3 Ld = normalize(mpos.xyz - lumpos.xyz);
  vec3 N = mnormal;
  if(use_nm == 1) {
    vec2 p = (texture(nm, tcoord).rb - vec2(0.5)) * 2.0;
    N = normalize(N + 0.5 * vec3(-p.x, 0.0, p.y));
  }

  vec3 smpcoord = smcoord.xyz / smcoord.w;
  float ild = clamp(dot(N, -Ld), 0.0, 1.0);
  float z = texture(sm, smpcoord.xy).r;
  if(z < smpcoord.z)
    ild = 0.0;
  
  vec3 R = reflect(Ld, N);
  R = normalize((transpose(inverse(view)) * vec4(R, 0.0)).xyz);
  float ils = pow(clamp(dot(R, -V), 0.0, 1.0), 10.0);
  vec4 tmp = ils * vec4(1.0, 1.0, 1.0, 1.0) + 0.15 * acouleur + 0.85 * ild * couleur;
  if(use_tex == 1)
    fragColor = mix(tmp, texture(tex, tcoord), 0.25);
  else
    fragColor = tmp;
}
