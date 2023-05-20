#version 330

uniform sampler2D tex;
uniform vec2 pas;

out vec4 fragColor;
in vec2 tCoord;


vec2 G[9] = vec2[9](vec2(-1, -1), vec2(0, -2), vec2(1, -1),
                    vec2(-2,  0), vec2(0,  0), vec2(2, 0),
                    vec2(-1,  1), vec2(0,  2), vec2(1,  1));

vec2 offset[9] = vec2[9](vec2(-pas.x,  pas.y), vec2(0,  pas.y), vec2(pas.x,  pas.y),
                         vec2(-pas.x,  0),         vec2(0,  0),       vec2(pas.x, 0),
                         vec2(-pas.x, -pas.y), vec2(0, -pas.y), vec2(pas.x, -pas.y));

vec3 sobel(sampler2D map) {
  vec3 s[2] = vec3[2](vec3(0), vec3(0));
  for(int i = 0; i < 9; i++) {
    vec3 cv = texture(map, tCoord + offset[i]).rgb;
    s[0] += G[i].x * cv;
    s[1] += G[i].y * cv;
  }
  return vec3(abs(s[0].r) + abs(s[1].r), abs(s[0].g) + abs(s[1].g), abs(s[0].b) + abs(s[1].b)) ;
}



void main() {
  fragColor = vec4(sobel(tex), 1.0);
}
