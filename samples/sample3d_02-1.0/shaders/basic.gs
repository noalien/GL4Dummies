#version 330

uniform int faceNormal;
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform vec4 lumPos;
in  vec3 vsoNormal[];
in  vec3 vsoPosition[];
in  vec3 vsoModPos[];
out vec3 gsoNormal;
out vec3 gsoModPos;
out vec3 gsoColor;

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

void main(void) {
  vec3 normal = vec3(0);
  if(gl_in.length() == 3) {
    if(faceNormal != 0) {
      normal = normalize(cross(vsoPosition[1].xyz - vsoPosition[0].xyz, 
			       vsoPosition[2].xyz - vsoPosition[1].xyz));
      normal = (transpose(inverse(modelViewMatrix)) * vec4(normal, 0.0)).xyz;
    }
    for(int i = 0; i < gl_in.length(); i++) {
      vec3 L = normalize(vsoModPos[i] - lumPos.xyz);
      gl_Position = gl_in[i].gl_Position;
      if(faceNormal != 0) {
	gsoNormal = normal;
	gsoColor = vec3(1) * dot(normal, -L);
      } else {
	gsoNormal = vsoNormal[i];
	gsoColor = vec3(1) * dot(vsoNormal[i], -L);
      }
      gsoModPos = vsoModPos[i];
      EmitVertex();
    }
    EndPrimitive();
  }
}
