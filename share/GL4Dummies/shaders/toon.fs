#version 330
uniform sampler2D myTexture;
uniform vec4 lumpos;
uniform int heightMap;
in vec2 vsoTexCoord;
in vec3 vsoNormal;
in vec4 vsoModPosition;

out vec4 fragColor;
const vec4 colors[] = vec4[](
			     vec4(0.0f, 0.0f, 0.0f, 0.1f), 
			     vec4(0.0f, 0.0f, 0.5f, 0.3f), 
			     vec4(0.0f, 0.2f, 0.7f, 0.5f), 
			     vec4(0.0f, 0.5f, 0.8f, 0.7f), 
			     vec4(0.0f, 0.8f, 0.8f, 0.85f), 
			     vec4(0.0f, 1.0f, 1.0f, 1.0f)  );


void main(void) {
  float il;
  vec3 color = vec3(0);
  vec3 lum = normalize(vsoModPosition.xyz - lumpos.xyz);
  il = dot(normalize(vsoNormal), -lum);
  color = vec3(1) * il;
  for(int i = 0; i < colors.length(); i++) {
    if(il < colors[i].a) {
      color = colors[i].rgb;
      break;
    }
  }
  if(heightMap == 1)
    fragColor = vec4(texture(myTexture, vsoTexCoord).r);
  else
    fragColor = mix(vec4(texture(myTexture, vsoTexCoord).rgb, 1.0), vec4(color, 1.0), 0.9);
}
