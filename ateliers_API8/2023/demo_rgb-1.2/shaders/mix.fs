#version 330
uniform float dt;
uniform sampler2D tex0;
uniform sampler2D tex1;
in  vec2 vsoTexCoord;
out vec4 fragColor;

void main(void) {
  fragColor = mix( texture(tex0, vsoTexCoord),
		   texture(tex1, vsoTexCoord), dt);
}
