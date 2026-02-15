/*!\file light.fs
 * \brief fragment shader progressif sur l'éclairage.
 */
#version 330

/* sortie du frament shader : une couleur */
out vec4 fragColor;
in vec2 vsoTexCoord;

void main(void) {
  fragColor = vec4(vsoTexCoord, 0.0, 1.0);
}
