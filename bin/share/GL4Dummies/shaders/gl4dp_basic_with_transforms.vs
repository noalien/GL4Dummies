#version 330
layout (location = 0) in vec3 vsiPosition;
layout (location = 1) in vec3 vsiNormal;
layout (location = 2) in vec2 vsiTexCoord;
uniform float rotation;
uniform vec2 scale, translate, ptranslate; 
out vec2 vsoTexCoord;

void main(void) {
  mat2 r = mat2(cos(rotation), sin(rotation), -sin(rotation), cos(rotation));
  vec2 v;
  v = r * (vsiPosition.xy - ptranslate) + ptranslate;
  gl_Position = vec4(v, 0.0, 1.0);
  vsoTexCoord = (vsiTexCoord / scale) - sign(scale) * translate;
}
