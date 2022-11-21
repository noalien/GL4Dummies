#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 proj;
uniform mat4 mod;
uniform mat4 view;
uniform vec4 lumpos;

out float ild;

void main() {
     vec3 Ld = normalize(pos - lumpos.xyz);
     vec3 n = (inverse(transpose(mod)) * vec4(normal, 0)).xyz;
     ild = clamp(dot(normalize(n), -normalize(Ld)), 0, 1);
     gl_Position = proj * view * mod * vec4(pos, 1.0);
}