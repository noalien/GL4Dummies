#version 330
uniform sampler2D myTexture;
uniform vec4 lumpos;
uniform int heightMap;
in vec2 vsoTexCoord;
in vec3 vsoNormal;
in vec4 vsoModPosition;

out vec4 fragColor;


void main(void) {
  float il;
  vec3 lum = normalize(vsoModPosition.xyz - lumpos.xyz);
  il = dot(normalize(vsoNormal), -lum);
  if(heightMap == 1)
    fragColor = il * vec4(texture(myTexture, vsoTexCoord).r);
  else
    fragColor = vec4(il);//mix(vec4(texture(myTexture, vsoTexCoord).rgb, 1.0), vec4(il), 0.3);
}
