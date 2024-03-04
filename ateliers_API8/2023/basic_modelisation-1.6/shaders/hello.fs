#version 330
in vec4 mpos;
in vec3 mnormal;
in vec2 tcoord;
out vec4 fragColor;

uniform vec4 couleur;
uniform vec4 lumpos;
uniform mat4 view;
uniform sampler2D tex;
uniform int use_tex;

void main() {
  const vec3 V = vec3(0.0, 0.0, -1.0);
  const vec4 acouleur = vec4(0.0, 0.3, 0.0, 1.0);
  vec3 Ld = normalize(mpos.xyz - lumpos.xyz);
  float ild = clamp(dot(mnormal, -Ld), 0.0, 1.0);
  vec3 R = reflect(Ld, mnormal);
  R = normalize((transpose(inverse(view)) * vec4(R, 0.0)).xyz);
  float ils = pow(clamp(dot(R, -V), 0.0, 1.0), 10.0);
  vec4 tmp = ils * vec4(1.0, 1.0, 0.0, 1.0) + 0.15 * acouleur + 0.85 * ild * couleur;
  if(use_tex == 1)
    fragColor = mix(tmp, texture(tex, tcoord), 0.07);
  else
    fragColor = tmp;
}
