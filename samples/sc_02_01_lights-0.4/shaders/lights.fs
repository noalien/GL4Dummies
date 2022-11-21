#version 330

in vec4 mvpos;
in vec3 N;

out vec4 fragColor;

uniform vec4 scolor;
uniform vec4 lcolor;
uniform vec4 lumpos;

void main() {
     vec4 ambient = 0.15f * lcolor * scolor; /* lacolor, sacolor */
     vec3 Ld = normalize(mvpos.xyz - lumpos.xyz);
     float ild = clamp(dot(normalize(N), -normalize(Ld)), 0, 1);
     vec4 diffus = (ild * lcolor) * scolor; /* ldcolor, sdcolor */
     fragColor = ambient + diffus;
}