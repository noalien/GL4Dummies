#version 330
uniform sampler2D myTexture;
uniform vec4 couleur;
in  vec2 vsoTexCoord;
out vec4 fragColor;

void main(void) {
  fragColor = couleur * texture(myTexture, vsoTexCoord.st);
}
