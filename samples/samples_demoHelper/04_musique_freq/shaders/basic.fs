#version 330
uniform sampler2D myTexture;
in  vec2 vsoTexCoord;
out vec4 fragColor;

void main(void) {
    fragColor = vec4(vec3(texture(myTexture, vsoTexCoord).r), 1.0);
}
