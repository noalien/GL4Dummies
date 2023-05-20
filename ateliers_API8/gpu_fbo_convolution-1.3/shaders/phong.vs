#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 proj, modView;
uniform vec2 pas;

uniform sampler2D tex;

out vec2 tCoord;
out vec3 vsoNormal;
out vec4 modPos;

vec2 offset[7] = vec2[7]( vec2(pas.x, 0), vec2(pas.x,  pas.y), vec2(0,  pas.y), vec2(-pas.x,  0), vec2(-pas.x, -pas.y), vec2(0, -pas.y), vec2(0,  0) );

float hauteur(vec2 tc) {
  return texture(tex, tc).r / 2.0;
}

vec3 tnormale(vec3 u, vec3 v) {
  return normalize(cross(u, v));
}

vec3 position(vec3 p, vec2 tc, vec2 os) {
  return p + vec3(os.x * 2.0, hauteur(tc + vec2(os.x, -os.y)), -(os.y * 2.0));
}

vec3 normale(vec3 pi, vec2 tc) {
  vec3 n =vec3(0);
  vec3 p[7];
  for(int i = 0; i < 7; ++i)
    p[i] = position(pi, tc, offset[i]);
  for(int i = 0; i < 6; ++i) {
    //vec3 a = p[6];
    vec3 b = p[i];
    vec3 c = p[(i + 1) % 6];
    n += tnormale(b - p[6], c - b);
  }
  return n / 6.0;
}

void main() {
  vec3 p = position(pos, texCoord, offset[6]);
  vsoNormal = normalize((transpose(inverse(modView)) * vec4(normale(pos, texCoord), 0.0)).xyz);
  modPos = modView * vec4(p, 1.0);
  gl_Position = proj * modPos;
  tCoord = texCoord;
}
