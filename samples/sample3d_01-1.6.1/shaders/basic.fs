#version 330
uniform sampler2D tex;

in  vec2 vsoTexCoord;
out vec4 fragColor;

void main(void) {
  fragColor = texture(tex, vsoTexCoord);
}
