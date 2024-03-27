#version 330

uniform vec4 lumpos;

uniform sampler2D myTexture;
uniform int hasTexture;
uniform int sky;
uniform vec4 diffuse_color;
uniform vec4 specular_color;
uniform vec4 ambient_color;
uniform vec4 emission_color;
uniform float shininess;

in vec2 vsoTexCoord;
in vec3 vsoNormal;
in vec4 vsoModPosition;

out vec4 fragColor;

void main(void) {
  if(sky != 0) {
    float i = texture(myTexture, vsoTexCoord).r;
    i = pow(i, 1.0);
    fragColor = vec4(vec3(i), 1.0) + vec4(0.0, 1.0, 1.0, 1.0);
    return;
  }
  vec3 lum  = normalize(vsoModPosition.xyz - lumpos.xyz);
  float diffuse = clamp(dot(normalize(vsoNormal), -lum), 0.0, 1.0);
  /* pour du toon shading */
  /* diffuse = float(int(diffuse * 4.0 - 0.01)) / 3.0; */
  vec3 lightDirection = vec3(lumpos - vsoModPosition);
  
  vec4 specularReflection = specular_color * pow(max(0.0, dot(normalize(reflect(-lightDirection, vsoNormal)), normalize(vec3(-vsoModPosition)))), shininess);
  
  vec4 diffuseReflection = ambient_color*0.2 +diffuse_color * diffuse;
  fragColor = diffuseReflection + specularReflection;
  if(hasTexture != 0)
    fragColor *= texture(myTexture, vsoTexCoord);
}


