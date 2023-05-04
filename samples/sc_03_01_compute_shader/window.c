/*!\file window.c
 *
 * \brief exemple d'utilisation d'un compute shader avec GL4Dummies
 * \author Farès BELHADJ, amsi@up8.edu
 * \date May 04 2023
 */

#include <GL4D/gl4du.h>
#include <GL4D/gl4dg.h>
#include <GL4D/gl4df.h>
#include <GL4D/gl4duw_SDL2.h>
#include <stdlib.h>
#include <assert.h>

/* Prototypes des fonctions statiques contenues dans ce fichier C */
static void init(void);
static void draw(void);
static void quit(void);
static void printWorkGroupsCapabilities(void);

/*!\brief largeur et hauteur de la fenêtre */
static int _wW = 1024, _wH = 768;
/*!\brief identifiant du GLSL program */
static GLuint _pId = 0;
/*!\brief identifiant pour un quadrilatère GL4Dummies */
static GLuint _quad = 0;
/*!\brief identifiant de textures */
static GLuint _tex = 0;

/*!\brief créé la fenêtre d'affichage, initialise GL et les données,
 * affecte les fonctions d'événements et lance la boucle principale
 * d'affichage.
 */
int main(int argc, char ** argv) {
  gl4duwSetGLAttributes(4, 5, GL4DW_CONTEXT_PROFILE_CORE, 1, 16);
  if(!gl4duwCreateWindow(argc, argv, "GL4Dummies", 20, 20, 
			 _wW, _wH, GL4DW_SHOWN))
    return 1;

  printWorkGroupsCapabilities();

  init();
  atexit(quit);
  gl4duwDisplayFunc(draw);
  gl4duwMainLoop();
  return 0;
}

/*!\brief initialise les paramètres OpenGL et les données. 
 */
void init(void) {
  /* Création du programme shader (voir le dossier shader) */
  _pId = gl4duCreateProgram("<cs>shaders/basic.cs", NULL);
  /* Création du quadrilatère */
  _quad = gl4dgGenQuadf();
  /* Set de la couleur (RGBA) d'effacement OpenGL */
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
  /* activation de la texture 2D */
  glEnable(GL_TEXTURE_2D);
  /* dans quelle partie de l'écran on effectue le rendu */
  glViewport(0, 0, _wW, _wH);

  /* générer deux identifiants de texture */
  glGenTextures(1, &_tex);
  /* lier un identifiant de texture comme texture 2D (1D ou 3D
   * possibles) */
  glBindTexture(GL_TEXTURE_2D, _tex);
  /* paramétrer la texture, voir la doc de la fonction glTexParameter
   * sur
   * https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexParameter.xhtml */
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glGenerateMipmap(GL_TEXTURE_2D);  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, _wW, _wH, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  /* dé-lier la texture 2D */
  glBindTexture(GL_TEXTURE_2D, 0);
}

/*!\brief Cette fonction dessine dans le contexte OpenGL actif. */
void draw(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* inspiré du code récupéré à cette adresse :
     https://zestedesavoir.com/tutoriels/1554/introduction-aux-compute-shaders/ */
  glUseProgram(_pId);
  glBindTexture(GL_TEXTURE_2D, _tex);
  glBindImageTexture (0, _tex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
  glDispatchCompute(_wW / 16, _wH / 16, 1); /* les 16 sont dans le compute shader */
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  glBindImageTexture (0, 0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
  glBindTexture(GL_TEXTURE_2D, 0);
  glUseProgram(0);
  gl4dfConvTex2Frame(_tex);
}

/*!\brief appelée au moment de sortir du programme (atexit), elle
 *  libère les éléments OpenGL utilisés.*/
void quit(void) {
  /* suppression des deux textures en GPU */
  if(_tex) {
    glDeleteTextures(1, &_tex);
    _tex = 0;
  }
  /* nettoyage des éléments utilisés par la bibliothèque GL4Dummies */
  gl4duClean(GL4DU_ALL);
}

/* récupérée à cette adresse :
   https://zestedesavoir.com/tutoriels/1554/introduction-aux-compute-shaders/ */
void printWorkGroupsCapabilities(void) {
  int workgroup_count[3];
  int workgroup_size[3];
  int workgroup_invocations;

  glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &workgroup_count[0]);
  glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &workgroup_count[1]);
  glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &workgroup_count[2]);

  printf ("Taille maximale des workgroups:\n\tx:%u\n\ty:%u\n\tz:%u\n",
  workgroup_size[0], workgroup_size[1], workgroup_size[2]);

  glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &workgroup_size[0]);
  glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &workgroup_size[1]);
  glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &workgroup_size[2]);

  printf ("Nombre maximal d'invocation locale:\n\tx:%u\n\ty:%u\n\tz:%u\n",
  workgroup_size[0], workgroup_size[1], workgroup_size[2]);

  glGetIntegerv (GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &workgroup_invocations);
  printf ("Nombre maximum d'invocation de workgroups:\n\t%u\n", workgroup_invocations);
}
