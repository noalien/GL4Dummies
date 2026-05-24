/*!\file light.fs
 * \brief fragment shader progressif sur l'éclairage.
 */
#version 330

uniform sampler2D tex;

/* sortie du frament shader : une couleur */
out vec4 fragColor;
in vec2 vsoTexCoord;
in vec3 vsoModPos;
in vec3 vsoModNormal;

void main(void) {
  const vec4 Lp = vec4(2.0, 1.5, 5.0, 1.0);
  vec3 Ld = normalize(vsoModPos - Lp.xyz);
  float diffus = clamp(dot(vsoModNormal, -Ld), 0.0, 1.0);
  fragColor = diffus * texture(tex, 8.0 * vsoTexCoord);
}
