#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 proj;
uniform mat4 mod;
uniform mat4 view;

void main() {
     gl_Position = proj * view * mod * vec4(pos, 1.0);
}