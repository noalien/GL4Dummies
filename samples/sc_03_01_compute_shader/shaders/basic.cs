/*
  Auteur : denissalem (https://zestedesavoir.com/@denissalem)
  code récupéré ici : https://zestedesavoir.com/tutoriels/1554/introduction-aux-compute-shaders/
*/

#version 430

layout (local_size_x = 16, local_size_y = 16) in;

layout (rgba32f, binding = 0) uniform image2D img_output;

void main() {
  // Aucun tableau de donnée n'étant passé au moment de la création de la texture,
  // c'est le compute shader qui va dessiner à l'intérieur de l'image associé
  // à la texture.

  // gl_LocalInvocationID.xy * gl_WorkGroupID.xy == gl_GlobalInvocationID
  ivec2 coords = ivec2(gl_GlobalInvocationID);

  // Pour mettre en evidence. Les groupes de travail locaux on dessine un damier.
  vec4 pixel;
  if ( ((gl_WorkGroupID.x & 1u) != 1u) != ((gl_WorkGroupID.y & 1u) == 1u)) {
    pixel = vec4(1.0,.5,.0,1.0);
  }
  else {
    pixel = vec4(.0,.5,1.0,1.0);
  }

  imageStore(img_output, coords, pixel);
}
