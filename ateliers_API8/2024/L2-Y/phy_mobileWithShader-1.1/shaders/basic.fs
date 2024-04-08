#version 330
uniform sampler1D mobiles;
uniform float step;
uniform int voronoi;
in  vec2 vsoTexCoord;
out vec4 fragColor;

vec4 balle(void) {
  float i;
  for(i = step / 2.0; i < 1.0; i += 2.0 * step) {
    vec3 po = texture(mobiles, i + step).xyz;
    float d = min(length(vsoTexCoord - po.xy) - po.z, 0);
    if(d < 0)
      return /* -(d / po.z) * */texture(mobiles, i);
  }
  if(vsoTexCoord.y > 0.90)
    return texture(mobiles, vsoTexCoord.x);
  return fragColor = vec4(1, 0, 0, 1);
}

vec4 voronoif(void) {
  float i;
  float imin = step / 2.0;
  float dmin = length(vsoTexCoord - texture(mobiles, imin + step).xy);
  for(i = imin + 2.0 * step; i < 1.0; i += 2.0 * step) {
    vec3 po = texture(mobiles, i + step).xyz;
    float d = length(vsoTexCoord - po.xy);
    if(d < dmin) {
      dmin = d;
      imin = i;
    }
  }
  if(dmin < 0.005)
    return vec4(0);
  return texture(mobiles, imin);
} 

void main(void) {
  if(voronoi != 0)
    fragColor = voronoif();
  else
    fragColor = balle();
  //fragColor = vec4(0, 0, 1, 0);
}
