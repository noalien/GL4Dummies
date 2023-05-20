#version 330

uniform sampler2D tex;
uniform vec2 pas;

out vec4 fragColor;
in vec2 tCoord;


void main() {
  vec4 m = vec4(0.0);
  for(int i = -1; i < 2; ++i)
    for(int j = -1; j < 2; ++j)
      m += texture(tex, vec2(tCoord.x + float(j) * pas.x, tCoord.y + float(i) * pas.y));
  m /= 9.0;
  fragColor = m;
}
