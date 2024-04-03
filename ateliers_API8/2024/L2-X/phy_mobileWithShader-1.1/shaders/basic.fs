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
      return -(d / po.z) *texture(mobiles, i);
  }
  if(vsoTexCoord.y > 0.95)
    return texture(mobiles, vsoTexCoord.x);
  return fragColor = vec4(1);
}

vec4 voronoif(void) {
  float i;
  float ipp = step / 2.0;
  vec3 po = texture(mobiles, ipp + step).xyz;
  float dpp = length(vsoTexCoord - po.xy);
  float ispp = 0.0;
  float dspp = 2.0;
  for(i = 2.0 * step + step / 2.0; i < 1.0; i += 2.0 * step) {
    vec3 po2;
    po = texture(mobiles, i + step).xyz;
    float d = length(vsoTexCoord - po.xy);
    if(d < dpp) {
      dspp = dpp;
      ispp = ipp;
      dpp = d;
      ipp = i;
    } else if(d < dspp) {
      dspp = d;
      ispp = i;
    }
  }
  if(dpp < 0.005)
    return vec4(0);
  return 1.3 * pow(((dspp - dpp) / dspp), 1.9) * texture(mobiles, ipp);
} 

void main(void) {
  if(voronoi != 0)
    fragColor = voronoif();
  else
    fragColor = balle();
  //fragColor = vec4(0, 0, 1, 0);
}
