#version 330
in float ild;
out vec4 fragColor;
uniform vec4 scolor;
uniform vec4 lcolor;

void main() {
     vec4 ambient = 0.15f * lcolor * scolor; /* lacolor, sacolor */
     vec4 diffus = (ild * lcolor) * scolor; /* ldcolor, sdcolor */
     fragColor = ambient + diffus;
}