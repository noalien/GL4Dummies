/*!\file dsquare.c
 *
 * \brief Generation d'un terrain a l'aide d'un diamond square
 * + lissage du terrain
 * \author Nicolas HENRIQUES, henriques.nicolas@hotmail.fr
 *
 * \date August 19 2016
 */
// #include "fifo.h"
#include "png_export.h"
#include "hm_view.h"

/*
 * Prototypes des fonctions statiques importantes contenues dans ce fichier C
 */
// static float bezierdegre2simple(float depart, float middle, float arrivee,
// float t);
static int resize_2n1(const uint taille);
static float value_simple(float height, float range);
// void ark_bezier_algo(float ** point_tab, int taille, float nb_motifs);
static void diamond_square(int taille, const char *filename, int choice,
                           int nb_args, char **args);
static void diamond_square_algo(int taille, const char *filename, int choice,
                                int nb_args, char **args,
                                float deplacement_diamond,
                                float deplacement_square);
static void sharpness_recalculation(float **point_tab, int taille,
                                    float min_height, float max_height);

float bezierdegre2simple(float depart, float middle, float arrivee, float t) {
  return (1 - t) * (1 - t) * depart + 2 * t * (1 - t) * middle + t * t * arrivee;
}

/*!\brief structure de 3 elements */
struct f3 {
  float a;
  float b;
  float c;
};
typedef struct f3 f3;


// static float modfloat(float nb, float mod) {
//  while (nb > mod) {
//    nb -= mod;
//  }
//  return nb;
// }

/*!\brief Cette fonction est censee post-traiter le terrain
 *  mais le resultat est assez décevant
 *  a ne pas utiliser
 */
/* void ark_bezier_algo(float **point_tab, int taille, int nb_motifs) { */
/*   // printf("taille=%d\n", taille); */
/*   int x_it; */
/*   int y_it; */
/*   float max_height, min_height; */
/*   f3 *courbes = malloc(taille * sizeof(f3)); */
/*   // float ** point_tab = malloc(taille * sizeof(float*)); */
/*   // for (y_it = 0; y_it < taille; ++y_it) */
/*   //   point_tab[y_it] = malloc(taille * sizeof(float)); */
/*   min_height = 1; */
/*   max_height = -1; */
/*   for (y_it = 0; y_it < taille; y_it += 1) { */
/*     courbes[y_it].a = (double)rand() / RAND_MAX * 2 - 1; */
/*     courbes[y_it].b = (double)rand() / RAND_MAX * 2 - 1; */
/*     courbes[y_it].c = (double)rand() / RAND_MAX * 2 - 1; */
/*     // printf("%f %f %f\n", courbes[y_it].a, courbes[y_it].b, courbes[y_it].c); */
/*     for (x_it = 0; x_it < taille; x_it += 1) { */
/*       point_tab[x_it][y_it] += */
/*           (bezierdegre2simple( */
/*                courbes[y_it].a, courbes[y_it].b, courbes[y_it].c, */
/*                modfloat((float)x_it / (float)taille * (float)nb_motifs, */
/*                         1) /\*(float)x_it / taille)*\/) + */
/*            sin((float)x_it / taille)) / */
/*           3; */
/*     } */
/*   } */
/*   for (x_it = 0; x_it < taille; x_it += 1) { */
/*     courbes[x_it].a = (double)rand() / RAND_MAX * 2 - 1; */
/*     courbes[x_it].b = (double)rand() / RAND_MAX * 2 - 1; */
/*     courbes[x_it].c = (double)rand() / RAND_MAX * 2 - 1; */
/*     for (y_it = 0; y_it < taille; y_it += 1) { */
/*       point_tab[x_it][y_it] += */
/*           (bezierdegre2simple(courbes[x_it].a, courbes[x_it].b, courbes[x_it].c, */
/*                               modfloat((float)y_it / taille * nb_motifs, 1)) + */
/*            sin((float)y_it / taille)) / */
/*           3; */
/*       if (min_height > point_tab[x_it][y_it]) */
/*         min_height = point_tab[x_it][y_it]; */
/*       if (max_height < point_tab[x_it][y_it]) */
/*         max_height = point_tab[x_it][y_it]; */
/*     } */
/*   } */
/* } */

// static void print_mat(int taille, float * mat[taille]) {
//  int it;
//  int it2;
//  for (it = 0; it < taille; ++it) {
//    for (it2 = 0; it2 < taille; ++it2) {
//      printf("[%3d;%3d]=%3f  ", it, it2, mat[it][it2]);
//    }
//    printf("\n\n");
//  }
// }

static int resize_2n1(const uint taille) {
  uint new_taille = 1;
  while (new_taille + 1 < taille) {
    new_taille = new_taille << 1;
  }
  return new_taille + 1;
}

static int combien_2n(const uint taille) {
  uint new_taille = taille - 1;
  uint result = 0;
  while (new_taille > 1) {
    new_taille = new_taille >> 1;
    result += 1;
  }
  return new_taille;
}

static float decheance(float deplacement, int le2n) {
  float H = 0.8;
  return (deplacement + (0.15 / ((0.1 + (float)le2n) / 10.0))) / powf(2.0, H);
}

static float value_simple(float height, float range) {
  return height + (double)rand() / RAND_MAX * range * 2.0 - range;
}

/*!\brief Cette fonction appelle la fonction effectuant le diamond-square
 */
static void diamond_square(int taille, const char *filename, int choice,
                           int nb_args, char **args) {
  fprintf(stdout, "génération de la heightmap\nla taille sera arrondie à la "
                  "taille 2^n+1 supérieure\n");
  taille = resize_2n1(taille);
  diamond_square_algo(taille, filename, choice, nb_args, args, 10, 10);
}

/*!\brief Cette fonction effectue le diamond-square,
 * elle enregistre le tout dans la matrice point_tab
 */
static void diamond_square_algo(int taille, const char *filename, int choice,
                                int nb_args, char **args,
                                float deplacement_diamond,
                                float deplacement_square) {
  printf("taille=%d\n", taille);
  int pas = taille;
  // le nb de cases remplies a la derniere iteration, on declare pour que ce
  // soit hors du stack
  int it;
  int it2;
  int x_it;
  int y_it;
  float parenting_height;
  float **point_tab = malloc(taille * sizeof(float *));
  float max_height, min_height;
  int le2n = combien_2n(taille) + 1;
  for (it = 0; it < taille; ++it) {
    point_tab[it] = malloc(taille * sizeof(float));
  }
  for (it = 0; it < taille; ++it) {
    for (it2 = 0; it2 < taille; ++it2) {
      point_tab[it][it2] = 0;
    }
  } // on met a 0
  point_tab[0][0] = (double)rand() / RAND_MAX * 2 - 1;
  point_tab[0][taille - 1] = (double)rand() / RAND_MAX * 2 - 1;
  point_tab[taille - 1][0] = (double)rand() / RAND_MAX * 2 - 1;
  point_tab[taille - 1][taille - 1] = (double)rand() / RAND_MAX * 2 - 1;
  min_height = point_tab[0][0];
  max_height = point_tab[0][0];
  printf("%f  %f  %f  %f\n", point_tab[0][0], point_tab[0][taille - 1],
         point_tab[taille - 1][0], point_tab[taille - 1][taille - 1]);
  while (pas > 1) {
    pas = pas >> 1;
    // printf("pas=%d\n", pas);

    // DIAMOND CASE
    for (y_it = pas; y_it < taille - 1; y_it += pas << 1) {
      for (x_it = pas; x_it < taille - 1; x_it += pas << 1) {
        it = 0;
        parenting_height = 0;
        if (point_tab[x_it][y_it] == 0) {
          if (x_it + pas < taille && y_it + pas < taille) {
            ++it;
            parenting_height += point_tab[x_it + pas][y_it + pas];
          }
          if (x_it - pas > -1 && y_it + pas < taille) {
            ++it;
            parenting_height += point_tab[x_it - pas][y_it + pas];
          }
          if (x_it + pas < taille && y_it - pas > -1) {
            ++it;
            parenting_height += point_tab[x_it + pas][y_it - pas];
          }
          if (x_it - pas > -1 && y_it - pas > -1) {
            ++it;
            parenting_height += point_tab[x_it - pas][y_it - pas];
          }
          parenting_height /= it;
          point_tab[x_it][y_it] =
              value_simple(parenting_height, deplacement_diamond);
          // printf("%d %d = %f \n", x_it, y_it, point_tab[x_it][y_it]);
          if (min_height > point_tab[x_it][y_it])
            min_height = point_tab[x_it][y_it];
          if (max_height < point_tab[x_it][y_it])
            max_height = point_tab[x_it][y_it];
        }
      }
    }
    // SQUARE CASE
    for (y_it = 0; y_it < taille; y_it += pas) {
      for (x_it = 0; x_it < taille; x_it += pas) {
        it = 0;
        parenting_height = 0;
        if (point_tab[x_it][y_it] == 0) {
          if (x_it + pas < taille) {
            ++it;
            parenting_height += point_tab[x_it + pas][y_it];
          }
          if (y_it + pas < taille) {
            ++it;
            parenting_height += point_tab[x_it][y_it + pas];
          }
          if (x_it - pas > -1) {
            ++it;
            parenting_height += point_tab[x_it - pas][y_it];
          }
          if (y_it - pas > -1) {
            ++it;
            parenting_height += point_tab[x_it][y_it - pas];
          }
          parenting_height /= it;
          point_tab[x_it][y_it] =
              value_simple(parenting_height, deplacement_square);
          if (min_height > point_tab[x_it][y_it])
            min_height = point_tab[x_it][y_it];
          if (max_height < point_tab[x_it][y_it])
            max_height = point_tab[x_it][y_it];
        }
      }
    }
    deplacement_diamond = decheance(deplacement_diamond, le2n);
    deplacement_square = decheance(deplacement_square, le2n);
    le2n--;
    // print_mat(taille, point_tab);
  }
  sharpness_recalculation(point_tab, taille, min_height, max_height); // lissage
  // ark_bezier_algo(point_tab, taille, (float)taille / 3); //detail
  if (choice == 1 || choice == 3)
    export(filename, point_tab, taille, min_height, max_height);
  if (choice == 2 || choice == 3)
    view(point_tab, taille, min_height, max_height, nb_args, args);
}


/*!\brief Cette fonction effectue un post-traitement,
 * elle lisse les extremités "piquantes" générées par le diamond-square
 * ( a desactiver si vous toruvez que le terrain manque de relief)
 */
static void sharpness_recalculation(float **point_tab, int taille,
                                    float min_height, float max_height) {
  int x_it, y_it;
  float voisins;
  int nb_voisins;
  float **diff_tab = malloc(taille * sizeof(float *));
  for (x_it = 0; x_it < taille; ++x_it) {
    diff_tab[x_it] = malloc(taille * sizeof(float));
  }
  for (y_it = 0; y_it < taille; ++y_it) {
    for (x_it = 0; x_it < taille; ++x_it) {
      voisins = 0;
      nb_voisins = 0;
      if (x_it > 0) {
        voisins += point_tab[x_it - 1][y_it];
        nb_voisins++;
      }
      if (y_it > 0) {
        voisins += point_tab[x_it][y_it - 1];
        nb_voisins++;
      }
      if (x_it < taille - 1) {
        voisins += point_tab[x_it + 1][y_it];
        nb_voisins++;
      }
      if (y_it < taille - 1) {
        voisins += point_tab[x_it][y_it + 1];
        nb_voisins++;
      }
      if (x_it > 0 && y_it > 0) {
        voisins += point_tab[x_it - 1][y_it - 1];
        nb_voisins++;
      }
      if (x_it > 0 && y_it < taille - 1) {
        voisins += point_tab[x_it - 1][y_it + 1];
        nb_voisins++;
      }
      if (x_it < taille - 1 && y_it > 0) {
        voisins += point_tab[x_it + 1][y_it - 1];
        nb_voisins++;
      }
      if (x_it < taille - 1 && y_it < taille - 1) {
        voisins += point_tab[x_it + 1][y_it + 1];
        nb_voisins++;
      }
      if (nb_voisins > 0) {
        voisins /= nb_voisins;
        // c est degueulasse
        diff_tab[x_it][y_it] =
            voisins - (((voisins - point_tab[x_it][y_it]) /
                        ((max_height - min_height) /* / 2*/)) -
                       (((voisins - point_tab[x_it][y_it]) /
                         ((max_height - min_height) /* / 2*/)) -
                        1));
      } else
        diff_tab[x_it][y_it] = 0;
    }
  }
  for (y_it = 0; y_it < taille; ++y_it) {
    for (x_it = 0; x_it < taille; ++x_it) {
      point_tab[x_it][y_it] = diff_tab[x_it][y_it];
    }
  }
  free(diff_tab);
}

// export = 1
// view = 2
// both = 3

int main(int nb_args, char** args) {
  srand(time(NULL));
  int choice_value = 0;
  int taille;
  if (!strcmp(args[2], "export") && nb_args > 3)
    choice_value = 1;
  else if (!strcmp(args[2], "view") && nb_args > 2)
    choice_value = 2;
  else if (!strcmp(args[2], "both") && nb_args > 3)
    choice_value = 3;
  else {
    printf("USAGE: ./dsquare [TAILLE] [export|view|both] ([IMG_NAME.png])\n");
    return -1;
  }
  taille = strtol(args[1], NULL, 0);
  if (taille > 3)
    diamond_square(taille, args[3], choice_value, nb_args, args);
  else
    fprintf(stderr, "ERROR: la taille entrée doit être supérieure à 2\n");
  return 0;
}
