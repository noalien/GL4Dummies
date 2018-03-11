#version 330
uniform vec4 lumPos;
uniform int phong;
in  vec3 gsoNormal;
in  vec3 gsoModPos;
in  vec3 gsoColor;
out vec4 fragColor;

void main(void) {
  if(phong != 0) {
    vec3 L = normalize(gsoModPos - lumPos.xyz);
    fragColor = vec4(vec3(dot(gsoNormal, -L)), 1.0);
  } else
    fragColor = vec4(vec3(gsoColor.r), 1.0);
}
