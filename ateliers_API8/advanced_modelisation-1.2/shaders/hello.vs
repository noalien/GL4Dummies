#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

out vec4 mpos;
out vec3 mnormal;
out vec2 tcoord;
out vec4 smcoord;

uniform mat4 proj, model, view;
uniform mat4 lightViewMatrix, lightProjectionMatrix;
uniform bool  tex_inv;
uniform float tex_zoom;

void main() {
  const mat4 bias = mat4( 0.5, 0.0, 0.0, 0.0,
                          0.0, 0.5, 0.0, 0.0,
                          0.0, 0.0, 0.5, 0.0,
                          0.5, 0.5, 0.5, 1.0 );
  mpos = model * vec4(pos, 1.0);
  mnormal = normalize(transpose(inverse(model)) * vec4(normal, 0.0)).xyz;
  gl_Position = proj * view * mpos;
  smcoord  = bias * lightProjectionMatrix * lightViewMatrix * mpos;
  tcoord  = tex_zoom * vec2(texCoord.x, tex_inv ? (1.0 - texCoord.y) : texCoord.y);
}
