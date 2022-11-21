#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 proj;
uniform mat4 mod;
uniform mat4 view;

out vec4 mvpos;
out vec3 N;

void main() {
     N = (inverse(transpose(mod)) * vec4(normal, 0)).xyz;
     mvpos = view * mod * vec4(pos, 1.0);
     gl_Position = proj * mvpos;
}