#version 330
in float il;
out vec4 fragColor;

uniform vec4 couleur;

void main() {
  fragColor = il * couleur;
}
