#version 330

uniform uint frame;
uniform vec2 step;
uniform float amplitude;
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform sampler2D tex;
layout (location = 0) in vec3 vsiPosition;
layout (location = 1) in vec3 vsiNormal;
layout (location = 2) in vec2 vsiTexCoord;

out vec3 vsoNormal;

float dephase = float(frame) / 250.0;

/* hauteur (y) d'un sommet (xz), ici une sinusoide lié à une distance
 * euclidienne + la hauteur lue dans la texture */
float height(vec2 p) {
  vec2 pp = (p + vec2(1)) / 2.0;
  const float freq = 20.0;
  float amp = amplitude * clamp(1.0 - length(p), 0, 1);
  return amp * sin(-freq * length(p) + dephase)  + texture(tex, pp).r;
}

/* calculs du vecteur normal de la surface décrite par les points (x,
 * hauteur(xz), z) p0 p1 p2 */
vec3 normale(vec2 p0, vec2 p1, vec2 p2) {
  vec3 np0 = vec3(p0.x, height(p0), p0.y);
  vec3 np1 = vec3(p1.x, height(p1), p1.y);
  vec3 np2 = vec3(p2.x, height(p2), p2.y);
  return cross((np1 - np0), (np2 - np0));
}

/* moyenne des normales des 6 triangles autour d'un sommet, p est le
 * sommet (xz) et d est le pas permettant d'aller aux sommets
 * voisins */
vec3 normale(vec2 p, vec2 d) {
  vec3 n1, n2, n3, n4, n5, n6;
  n1 = normalize(normale(vec2(p.x + d.x, p.y), p, vec2(p.x + d.x, p.y + d.y)));
  n2 = normalize(normale(vec2(p.x + d.x, p.y + d.y), p, vec2(p.x, p.y + d.y)));
  n3 = normalize(normale(p, vec2(p.x - d.x, p.y), vec2(p.x, p.y + d.y)));
  n4 = normalize(normale(vec2(p.x - d.x, p.y), p, vec2(p.x - d.x, p.y - d.y)));
  n5 = normalize(normale(vec2(p.x - d.x, p.y - d.y), p, vec2(p.x, p.y - d.y)));
  n6 = normalize(normale(p, vec2(p.x + d.x, p.y), vec2(p.x, p.y - d.y)));
  return (n1 + n2 + n3 + n4 + n5 + n6) / 6.0;
}

void main(void) {
  vec3 pos = vec3(vsiPosition.x,
		  height(vsiPosition.xz),
		  vsiPosition.z);
  vec3 tmp = normale(vsiPosition.xz, step);
  vsoNormal = (transpose(inverse(modelViewMatrix)) * vec4(tmp, 0.0)).xyz;
  gl_Position = projectionMatrix * modelViewMatrix * vec4(pos, 1.0);
}
