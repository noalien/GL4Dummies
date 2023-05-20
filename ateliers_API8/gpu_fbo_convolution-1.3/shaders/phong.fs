#version 330

uniform sampler2D tex;
uniform vec2 pas;
uniform mat4 modView;


out vec4 fragColor;
in vec2 tCoord;
in vec3 vsoNormal;
in vec4 modPos;

void main() {
  vec4 Lp = vec4(1.0, 0.75, 0.0, 1.0);
  Lp = modView * Lp;
  vec3 Ld = normalize((modPos - Lp).xyz);
  normalize(vec3(0, 0, -1));
  float il = clamp(dot(vsoNormal, -Ld), 0, 1);
  fragColor = vec4(il);
}
