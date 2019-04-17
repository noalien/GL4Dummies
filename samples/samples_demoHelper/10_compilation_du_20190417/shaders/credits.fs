#version 330
uniform sampler2D tex;
in  vec2 vsoTexCoord;
out vec4 fragColor;

void main(void) {
  vec4 c = texture(tex, vsoTexCoord);
  fragColor = vec4(c.rgb, length(c.rgb) > 0.0 ? 1.0 : 0.0);
}
