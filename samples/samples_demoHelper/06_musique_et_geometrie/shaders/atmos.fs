#version 330
uniform sampler2D ecloud;
in  vec3 vsoModPos;
in  vec2 vsoTexCoord;
out vec4 fragColor;

void main(void) {
  vec4 color = vec4(0, 1, 1, 0.01) * (1.0 - texture(ecloud, vsoTexCoord).r);
  fragColor = color + texture(ecloud, vsoTexCoord).rgbr;
}
