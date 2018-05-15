#version 330

uniform sampler2D tex;
in vec2 vsoTexCoord;
in vec3 vsoNormal;
in vec4 vsoModPosition;

out vec4 fragColor;

void main(void) {
  fragColor = texture(tex, -vsoTexCoord);
}


