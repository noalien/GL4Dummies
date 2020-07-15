/*!\file basic.vs
 *
 * \brief vertex shader basique, un sommet possédant les attributs
 * coordonnées spaciales 3D, vecteur normal (3D) et coordonnée de
 * texture. 
 *
 * \author Farès BELHADJ, amsi@up8.edu
 * \date July 15 2020
 */

/* Vertex shader, directement en relatrion avec le code-CPU */
/* Version GLSL 3.30 */
#version 330

/* variable cablée sur l'attribut 0 du sommet dans le code CPU
 * (glVertexAttribPointer(0, ...) le 0 correspond au location = 0)
 * c'est une coordonnée spaciale 3D */
layout (location = 0) in vec3 vsiPosition;
/* variable cablée sur l'attribut 1 du sommet dans le code CPU
 * (glVertexAttribPointer(1, ...) le 1 correspond au location = 1)
 * c'est une normale 3D (x, y, z) */
layout (location = 1) in vec3 vsiNormal;
/* variable cablée sur l'attribut 2 du sommet dans le code CPU
 * (glVertexAttribPointer(2, ...) le 2 correspond au location = 2)
 * c'est une coordonnée de textures 2D */
layout (location = 2) in vec2 vsiTexCoord;

/* une coordonnée de textures de sortie du vertex shader vers le
 * fragment shader (voir basic.fs, in vec2 vsoTexCoord) */
out vec2 vsoTexCoord;

void main(void) {
  /* gl_Position, nom réservé produisant un sommet GL */
  /* l'entrée vsiPosition est complétée en vecteur 4D (x, y, z, w) où
   * z = 0.0 et w = 1.0 ;  */
  gl_Position = vec4(vsiPosition, 1.0);
  /* coordonnée de texture transmise du vertex shader vers le fragment
   * shader */
  vsoTexCoord = vsiTexCoord;
}
