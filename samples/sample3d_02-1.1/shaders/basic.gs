#version 330

uniform int faceNormal;
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform vec4 lumPos;
/* les sortie du vertex shader deviennent des entrées en mode array
   car nous recevons un triangle en entrée */
in  vec3 vsoNormal[];
in  vec3 vsoPosition[];
in  vec3 vsoModPos[];
in  vec2 vsoTexCoord[];
/* les sorties du geometry vers le fragment */
out vec3 gsoNormal;
out vec3 gsoModPos;
out float gsoIdiffus;
out vec2 gsoTexCoord;

/* en entrée un triangle */
layout(triangles) in;
/* en sortie, une bande de triangles à 3 sommets (càd 1 triangle) */
layout(triangle_strip, max_vertices = 3) out;

void main(void) {
  vec3 normal = vec3(0);
  if(gl_in.length() == 3) {
    if(faceNormal != 0) {
      /* recalculer la normale au sommet et la remplacer par la
	 normale à la face */
      normal = normalize(cross(vsoPosition[1].xyz - vsoPosition[0].xyz, 
			       vsoPosition[2].xyz - vsoPosition[1].xyz));
      normal = (transpose(inverse(modelViewMatrix)) * vec4(normal, 0.0)).xyz;
    }
    for(int i = 0; i < gl_in.length(); i++) {
      /* Lumière vers sommet */
      vec3 L = normalize(vsoModPos[i] - lumPos.xyz);
      gl_Position = gl_in[i].gl_Position;
      if(faceNormal != 0) {
	gsoNormal = normal;
	/* Intensité lumière diffuse : Gouraud par rapport à la normale à la face */
	gsoIdiffus = dot(normal, -L);
      } else {
	gsoNormal = vsoNormal[i];
	/* Intensité lumière diffuse : Gouraud par rapport à la normale au sommet */
	gsoIdiffus = dot(vsoNormal[i], -L);
      }
      gsoModPos = vsoModPos[i];
      gsoTexCoord = vsoTexCoord[i];
      EmitVertex();
    }
    EndPrimitive();
  }
}
