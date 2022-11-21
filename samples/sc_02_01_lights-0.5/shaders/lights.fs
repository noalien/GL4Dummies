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
     vec3 R = reflect(Ld, N);
     vec3 Vue = vec3(0, 0, -1);
     float ils = pow(clamp(dot(R, -Vue), 0, 1), 10);
     vec4 spec = ils * lcolor; /* sscolor */
     fragColor = ambient + diffus + spec;
}