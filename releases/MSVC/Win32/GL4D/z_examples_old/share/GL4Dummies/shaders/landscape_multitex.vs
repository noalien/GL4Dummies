#version 330

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 lumpos;
uniform vec3 viewpos;

layout (location = 0) in vec3 vsiPosition;
layout (location = 1) in vec3 vsiNormal;
layout (location = 2) in vec2 vsiTexCoord;
layout (location = 3) in vec3 vsiTangent;
layout (location = 4) in vec3 vsiBitangent;
 
out vec2 vsoTexCoord;
out vec3 vsoNormal;

out mat3 TBN;

out vec3 tangentLightPos; 
out vec3 tangentViewPos;
out vec3 tangentFragPos;
out float height;

void main(void) {
  
  gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vsiPosition, 1.0);
  
  vec3 fragPos=vec3(modelMatrix*vec4(vsiPosition,1.0));
  vsoTexCoord = vsiTexCoord;
  mat3 nMatrix= mat3(transpose(inverse(modelMatrix)));
  vsoNormal = normalize( nMatrix* vsiNormal);

  TBN=transpose(mat3(normalize(nMatrix*vsiTangent),normalize(nMatrix*vsiBitangent),normalize(nMatrix*vsiNormal)));
  
  tangentLightPos=/*TBN*/lumpos;
  tangentViewPos=/*TBN*/viewpos;
  tangentFragPos=/*TBN*/fragPos;

  height=vsiPosition.y;
  
}

