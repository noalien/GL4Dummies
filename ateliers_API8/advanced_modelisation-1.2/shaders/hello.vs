#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

out vec4 mpos;
out vec3 mnormal;
out vec2 tcoord;
out vec4 smcoord;


uniform mat4 proj, model, view;

uniform mat4 lightViewMatrix;
uniform mat4 lightProjectionMatrix;


void main() {
  const mat4 bias = mat4( 0.5, 0.0, 0.0, 0.0,
                          0.0, 0.5, 0.0, 0.0,
                          0.0, 0.0, 0.5, 0.0,
                          0.5, 0.5, 0.5, 1.0 );
  mpos = model * vec4(pos, 1.0);
  mnormal = normalize(transpose(inverse(model)) * vec4(normal, 0.0)).xyz;
  gl_Position = proj * view * mpos;
  smcoord  = bias * lightProjectionMatrix * lightViewMatrix * mpos;
  tcoord = vec2(1.0 * texCoord.x, 1.0 * (1.0 - texCoord.y));
}
