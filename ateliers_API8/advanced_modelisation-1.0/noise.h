/*!\file noise.h
 *
 * \brief Headers des fonctions bruit de Perlin appliqué en
 * GPU. Nécessite la préparation de données et envoi sous forme de
 * texture.
 *
 * \author Farès BELHADJ, amsi@up8.edu
 * \date May 12, 2023
 */
#include <GL4D/gl4dummies.h>
extern void initNoiseTextures(void);
extern void useNoiseTextures(GLuint pid, int shift);
extern void unuseNoiseTextures(int shift);
extern void freeNoiseTextures(void);


