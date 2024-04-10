#version 330
out vec4 fragColor;
/* 4 floats venant du CPU, et affectés au vec4 couleur */
uniform vec4 couleur;

uniform sampler2D tex;
uniform int has_tex;

/* je récupère la sortie de mon vertex shader dans ce fragment shader */
in float il;

in vec2 vsoTexCoord;

void main() {
  if(has_tex != 0) {
    fragColor = vec4((il * couleur * texture(tex, vsoTexCoord)).rgb, couleur.a);
  } else {
    fragColor = il * couleur;
  }
}
