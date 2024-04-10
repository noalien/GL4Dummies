#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 tC;

uniform mat4 lightView;
uniform mat4 lightProj;
uniform mat4 proj;
uniform mat4 mod;
uniform mat4 view;

out vec4 mvpos;
out vec3 N;
out vec4 vsoSMCoord;

void main() {
  N = normalize((inverse(transpose(mod)) * vec4(normal, 0)).xyz);
  mvpos = view * mod * vec4(pos, 1.0);
  gl_Position = proj * mvpos;

  const mat4 bias = mat4( 0.5, 0.0, 0.0, 0.0,
                          0.0, 0.5, 0.0, 0.0,
                          0.0, 0.0, 0.5, 0.0,
                          0.5, 0.5, 0.5, 1.0 );
  vsoSMCoord  = bias * lightProj * lightView * mod * vec4(pos, 1.0);
}
