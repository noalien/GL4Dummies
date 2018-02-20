#version 330
uniform sampler2D tex;
uniform int sky, fog;
uniform float texRepeat;

in  vec2 vsoTexCoord;
out vec4 fragColor;

void main(void) {
  const float SQRT2 = 1.442695, fog_density2 = 0.01;
  const vec4 fog_color = vec4(0.5, 0.5, 0.5, 1.0);
  if(sky != 0) {
    float x = 0.02 + 0.96 * abs(2.0 * vsoTexCoord.x - 1);
    float y = clamp(2.0 * (1.0 - vsoTexCoord.y), 0, 1);
    vec4  texc = vec4(0, 0.5, 0.5, 1.0) + pow(texture(tex, vec2(x, 0.96 * y)).rrrr, vec4(4.0));
    if(fog != 0) {
      float ffactor = 1.0 - pow(y, 30);
      fragColor = mix(fog_color, texc, ffactor);
    } else
      fragColor = texc;
  } else {
    vec4 texc = texture(tex, texRepeat * vsoTexCoord);
    if(fog != 0) {
      float z = gl_FragCoord.z / gl_FragCoord.w;
      float ffactor = exp(-fog_density2 * z * z);
      ffactor = clamp(ffactor, 0.0, 1.0);
      fragColor = mix(fog_color, texc, ffactor);
      fragColor = mix(fog_color, texc, ffactor);
    } else
      fragColor = texc;
  }
}
