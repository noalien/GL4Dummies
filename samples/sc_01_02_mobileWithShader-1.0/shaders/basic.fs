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
      return -(d / po.z) * texture(mobiles, i);
  }
  if(vsoTexCoord.y > 0.95)
    return texture(mobiles, vsoTexCoord.x);
  return fragColor = vec4(1);
}

vec4 voronoif(void) {
  /* A FAIRE */
  return vec4(1);
} 

void main(void) {
  if(voronoi != 0)
    fragColor = voronoif();
  else
    fragColor = balle();
  //fragColor = vec4(0, 0, 1, 0);
}
