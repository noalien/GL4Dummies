#version 330
in vec3 vsoNormal;
out vec4 fragColor;

void main(void) {
  fragColor = vec4(vec3(dot(vec3(0, 0, 1), normalize(vsoNormal))), 1.0);
}
