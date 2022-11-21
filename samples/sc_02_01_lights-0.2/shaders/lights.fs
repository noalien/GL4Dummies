#version 330
out vec4 fragColor;
uniform vec4 scolor;
uniform vec4 lcolor;

void main() {
     vec4 ambient = 0.15f * lcolor * scolor;
     fragColor = ambient;
}