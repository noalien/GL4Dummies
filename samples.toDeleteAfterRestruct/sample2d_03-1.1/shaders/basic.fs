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
    float d = length(vsoTexCoord - po.xy) - po.z;
    if(d < 0)
      return -(d / po.z) * texture(mobiles, i);
  }
  if(vsoTexCoord.y > 0.95)
    return texture(mobiles, vsoTexCoord.x);
  return vec4(1);
}

vec4 voronoif_simple(void) {
  float mini = step / 2.0;
  vec3  po = texture(mobiles, step + mini).xyz;
  float mind = length(vsoTexCoord - po.xy);
  for(float i = mini + 2.0 * step; i < 1.0; i += 2.0 * step) {
    po = texture(mobiles, i + step).xyz;
    float d = length(vsoTexCoord - po.xy);
    if(mind > d) {
      mind = d;
      mini = i;
    }
  }
  if(mind < 0.003)
    return vec4(vec3(0), 1);
  return texture(mobiles, mini);
} 

vec4 voronoif_ombre(void) {
  float mini = step / 2.0;
  vec3  po = texture(mobiles, step + mini).xyz;
  float mind = length(vsoTexCoord - po.xy), old_mind = 1, ombre;
  for(float i = mini + 2.0 * step; i < 1.0; i += 2.0 * step) {
    po = texture(mobiles, i + step).xyz;
    float d = length(vsoTexCoord - po.xy);
    if(mind > d) {
      old_mind = mind;
      mind = d;
      mini = i;
    }
  }
  ombre = pow(((old_mind - mind) / old_mind), 0.5);
  if(ombre < 0.4) ombre = 0;
  return  ombre * texture(mobiles, mini);
} 

void main(void) {
  if(voronoi != 0)
    fragColor = voronoif_ombre();
  else
    fragColor = balle();
  //fragColor = vec4(0, 0, 1, 0);
}
