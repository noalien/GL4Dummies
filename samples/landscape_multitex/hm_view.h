/*!\file hm_view.h
 *
 * \brief Visualisation d'un terrain généré
 * + calculs (normales, tangentes, camera..)
 * \author Nicolas HENRIQUES, henriques.nicolas@hotmail.fr
 *
 * \date August 19 2016
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "GL4D/gl4du.h"




void view(float ** tab, int taille, float min_height, float max_height, int nb_args, char** args);