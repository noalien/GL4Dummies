#version 330
uniform sampler2D tex;
uniform int border;

in  vec2 vsoTexCoord;
out vec4 fragColor;

void main(void) {
  if( border != 0 && (vsoTexCoord.s < 0.02 ||
		      vsoTexCoord.t < 0.02 ||
		      (1 - vsoTexCoord.s) < 0.02 || 
		      (1 - vsoTexCoord.t) < 0.02 ) )
    fragColor = vec4(0.5, 0, 0, 1);
  else
    fragColor = texture(tex, vsoTexCoord);
}
