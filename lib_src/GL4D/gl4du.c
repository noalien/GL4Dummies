/*!\file gl4du.c
 *
 * \brief The GL4Dummies Utilities
 *
 * \author Farès BELHADJ amsi@grafeet.fr (ou amsi@ai.univ-paris8.fr)
 * \date March 28, 2008
 *
 * \todo ajouter, pour pas mal de fonctions, une gestion des erreurs à
 * la errno ...
 */

#include "gl4du.h"
#include "bin_tree.h"
#include <sys/stat.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "aes.h"
#if !defined(_MSC_VER)
#  include <errno.h>
#  include <limits.h>
#endif

typedef struct shader_t shader_t;
typedef struct program_t program_t;
/*!\brief type de données lié à la structure struct _GL4DUMatrix. */
typedef struct _GL4DUMatrix _GL4DUMatrix;

struct shader_t {
  GLuint id;
  GLenum shadertype;
  char * filename;
  time_t mod_time;
  unsigned todelete:1;
  int nprograms, sprograms;
  program_t ** programs;
  shader_t * next;
};

struct program_t {
  GLuint id;
  int nshaders, sshaders;
  shader_t ** shaders;
  program_t * next;
};

/*!\brief structure de données pour une pile de "une matrice 4x4". */
struct _GL4DUMatrix {
  char * name;  /* nom unique de matrice */
  GLenum type;  /* le type peut etre GLfloat ou GLdouble */
  size_t size;  /* une matrice 4x4, donc la taille d'un élément est : 16 * la taille du type */
  size_t nmemb; /* le nombre de copies de la matrice strockables dans la pile (taille de la pile) */
  GLuint top;   /* le haut de la pile de "une matrice" */
  void * data;  /* les données de la pile de "une matrice" */
};
/*!\brief liste de vertex et fragment shaders. Chaque shader est
 * composé d'un id (GL), du type, du nom de fichier et de la date de
 * modification du fichier.
 */
static shader_t * shaders_list = NULL;

/*!\brief liste de programs. Chaque program est composé d'un id (GL),
 * il est lié à une liste de shaders.
 */
static program_t * programs_list = NULL;

/*!\brief arbre binaire contenant l'ensemble des matrices \a
 * _GL4DUMatrix gérées. */
static bin_tree_t  *  _gl4duMatrices = NULL;
/*!\brief la matrice \a _GL4DUMatrix courante. */
static _GL4DUMatrix * _gl4dCurMatrix = NULL;
/*!\brief le noeud vers la dernière matrice \a _GL4DUMatrix créée. */
static bin_tree_t ** _gl4dLastMatrixn = NULL;

static shader_t **  findfnInShadersList(const char * filename);
static shader_t **  findidInShadersList(GLuint id);
static shader_t **  addInShadersList(GLenum shadertype, const char * filename);
static shader_t **  addInShadersListFED(const char * decData, GLenum shadertype, const char * filename);
static void         deleteFromShadersList(shader_t ** shp);
static program_t ** findInProgramsList(GLuint id);
static program_t ** addInProgramsList(GLuint id);
static void         deleteFromProgramsList(program_t ** pp);
static void         attachShader(program_t * prg, shader_t * sh);
static void         detachShader(program_t * prg, shader_t * sh);

static inline _GL4DUMatrix * newGL4DUMatrix(GLenum type, const char * name);
static inline void freeGL4DUMatrix(void * matrix);
static inline int matrixCmpFunc(const void * m1, const void * m2);
static inline bin_tree_t ** findMatrix(const char * name);
static inline void * matrixData(_GL4DUMatrix * matrix);

/*!\brief stocke le chemin relatif à partir duquel le binaire a été exécuté. Est initialisée dans
 *  \a gl4dInit.
 *
 * \see gl4dInit
 */
static char _pathOfMe[BUFSIZ] = {0};
static int  _pathOfMeInit = 0;

#if defined(_WIN32)
#  include <windows.h>
#elif defined(__FreeBSD__)
#  include <sys/types.h>
#  include <libutil.h>
#  include <unistd.h>
#elif defined(__MACOSX__)
#  include <sys/types.h>
#  include <unistd.h>
#  include <libproc.h>
#else /* autres unices */
#  include <unistd.h>
#endif

static void findPathOfMe(const char * argv0) {
  char buf[BUFSIZ] = {0};
#if defined(_WIN32)
  /* tous les compilateurs sous windows ? */
  GetModuleFileNameA(NULL, buf, sizeof buf);
#elif defined(__FreeBSD__)
  struct kinfo_proc *proc = kinfo_getproc(getpid());
  if(proc) {
    strncpy(buf, proc->ki_comm, sizeof buf);
    free(proc);
  } else {
    fprintf(stderr, "%s (%s:%d) - error while kinfo_getproc(getpid()), trying with readlink\n",
	    __func__, __FILE__, __LINE__);
    if(readlink("/proc/curproc/file", buf, sizeof buf) <= 0) {
      fprintf(stderr, "%s (%s:%d) - finding exec path failed with readlink\n",
	      __func__, __FILE__, __LINE__);
      /* sinon essayer sysctl CTL_KERN KERN_PROC KERN_PROC_PATHNAME -1 ??? */
    }
  }
#elif defined(__MACOSX__)
  pid_t pid = getpid();
  if(proc_pidpath(pid, buf, sizeof buf) <= 0) {
    fprintf(stderr, "%s (%s:%d) - proc_pidpath(%d ...) error: %s\n",
	    __func__, __FILE__, __LINE__, pid, strerror(errno));
    /* essayer _NSGetExecutablePath() (man 3 dyld) ??? */
  }
#else /* autres unices */
  if(readlink("/proc/self/exe", buf, sizeof buf) <= 0 &&       /*  (Linux)  */
     readlink("/proc/curproc/file", buf, sizeof buf) <= 0 &&    /* (BSD ?) */
     readlink("/proc/curproc/exe", buf, sizeof buf) <= 0 &&    /* (NetBSD) */
     readlink("/proc/self/path/a.out", buf, sizeof buf) <= 0  /* (Solaris) sinon strncpy(buf, getexecname(), sizeof buf) ?? */)
    fprintf(stderr, "%s (%s:%d) - finding exec path failed with readlink\n",
	    __func__, __FILE__, __LINE__);
#endif
  strncpy(_pathOfMe, pathOf(strlen(buf) > 0 ? buf : argv0), sizeof _pathOfMe);
  _pathOfMeInit = 1;
}

/*!\brief Initialise la bibliothèque.
 *
 * récupère le chemin relatif à partir duquel le binaire a été exécuté
 * et le stocke dans \a _pathOfMe.
 * \see _pathOfMe
 */
void gl4duInit(int argc, char ** argv) {
  findPathOfMe(argc > 0 ? argv[0] : "");
}

/*!\brief Ajoute \a _pathOfMe au chemin \a filename passé en argument
 * et stocke l'ensemble dans \a dst.
 *
 * \see _pathOfMe
 */
void gl4duMakeBinRelativePath(char * dst, size_t dst_size, const char * filename) {
  snprintf(dst, dst_size, "%s/%s", _pathOfMe, filename);
  if(!_pathOfMeInit) fprintf(stderr, "Binary path unknown, you should call gl4duInit(argc, argv) at program init\n");
}

/*!\brief imprime s'il existe l'infoLog de la compilation du Shader
 * \a object dans \a fp.
 */
void gl4duPrintShaderInfoLog(GLuint object, FILE * fp) {
  char * log = NULL;
  int maxLen = 0, len = 0;
  glGetShaderiv(object, GL_INFO_LOG_LENGTH, &maxLen);
  if(maxLen > 1) {
    log = malloc(maxLen * sizeof * log);
    assert(log);
    glGetShaderInfoLog(object, maxLen, &len, log);
    fprintf(fp, "%s (%d): %s (ID = %d)\n%s\n", __FILE__, __LINE__, __func__, object, log);
    free(log);
  }
}

/*!\brief imprime s'il existe l'infoLog de l'édition de liens pour le
 * Program \a object dans \a fp.
 */
void gl4duPrintProgramInfoLog(GLuint object, FILE * fp) {
  char * log = NULL;
  int maxLen = 0, len = 0;
  glGetProgramiv(object, GL_INFO_LOG_LENGTH, &maxLen);
  if(maxLen > 1) {
    log = malloc(maxLen * sizeof * log);
    assert(log);
    glGetProgramInfoLog(object, maxLen, &len, log);
    fprintf(fp, "%s (%d): %s (ID = %d)\n%s\n", __FILE__, __LINE__, __func__, object, log);
    free(log);
  }
}

/*!\brief imprime dans le fichier pointé par \a fp le
 * Frame-Per-Second.
 *
 * Cette fonction doit être appelée dans la loop.
 */
void gl4duPrintFPS(FILE * fp) {
  double t;
  static double t0 = 0;
  static unsigned int f = 0;
  f++;
  t = gl4dGetElapsedTime();
  if(t - t0 > 2000.0) {
    fprintf(fp, "%8.2f\n", (1000.0 * f / (t - t0)));
    t0 = t;
    f  = 0;
  }
}

/*!\brief retourne l'identifiant du shader décrit dans \a filename.
 *
 * Soit le shader existe déjà et l'identifiant est juste retourné avec
 * \ref findfnInShadersList, soit il est créer en utilisant \ref
 * gl4duCreateShader, glShaderSource et glCompileShader. Il se peut
 * que le shader soit mis à jour si la date de modification du fichier
 * est differente de celle stoquée dans \ref shaders_list. Dans ce cas le
 * shader est détruit puis recréé et la modification doit être
 * répercuté sur tous les PROGRAMs liés.
 *
 * \return l'identifiant du shader décrit dans \a filename.
 */
GLuint gl4duCreateShader(GLenum shadertype, const char * filename) {
  char temp[BUFSIZ << 1];
  shader_t ** sh = findfnInShadersList(filename);
  if(*sh) return (*sh)->id;
  gl4duMakeBinRelativePath(temp, sizeof temp, filename);
  // la ligne précédente fait ça snprintf(temp, sizeof temp, "%s/%s", _pathOfMe, filename);
  sh = findfnInShadersList(temp);
  if(*sh) return (*sh)->id;
  sh = addInShadersList(shadertype, filename);
  if(!sh) {
    fprintf(stderr, "trying with another path (%s)\n", temp);
    sh = addInShadersList(shadertype, temp);
  }
  return (sh) ? (*sh)->id : 0;
}

/*!\brief retourne l'identifiant du shader décrit dans \a filename.
 * Version FED de la précédente
 * \todo commenter
 * \todo ajouter la gestion des chemins relatifs à l'emplacement du binaire comme pour \a gl4duCreateShader.
 */
GLuint gl4duCreateShaderFED(const char * decData, GLenum shadertype, const char * filename) {
  shader_t ** sh = findfnInShadersList(filename);
  if(*sh) return (*sh)->id;
  sh = addInShadersListFED(decData, shadertype, filename);
  return (sh) ? (*sh)->id : 0;
}

/*!\brief retourne l'identifiant du shader décrit dans \a filename.
 *
 * Soit le shader existe et l'identifiant est retourné soit la
 * fonction retourne 0.
 *
 * \return l'identifiant du shader décrit dans \a filename ou 0 s'il n'a
 * pas été chargé.
 */
GLuint gl4duFindShader(const char * filename) {
  shader_t ** sh = findfnInShadersList(filename);
  return (*sh) ? (*sh)->id : 0;
}

/*!\brief supprime le shader dont l'identifiant openGL est \a id de la
 * liste de shaders \ref shaders_list.
 */
void gl4duDeleteShader(GLuint id) {
  shader_t ** sh = findidInShadersList(id);
  if(*sh) {
    if((*sh)->nprograms > 0)
      (*sh)->todelete = 1;
    else
      deleteFromShadersList(sh);
  }
}

/*!\brief créé un program à partir d'une liste (variable) de nom de
 * fichiers shaders et renvoie l'identifiant openGL du program créé.
 *
 * Les arguments sont les noms des fichiers shaders précédés d'un tag
 * indiquant le type de shader : \<vs\> pour un vertex shader et \<fs\>
 * pour un fragment shader et \<gs\> pour un geometry shader.
 *
 * \return l'identifiant openGL du program créé sinon 0. Dans ce
 * dernier cas les shaders créés (compilés) avant l'échec ne sont pas
 * supprimés ; un appel à \ref gl4duCleanUnattached peut s'en charger.
 */
GLuint gl4duCreateProgram(const char * firstone, ...) {
  va_list  pa;
  const char * filename;
  program_t ** prg;
  GLuint sId, pId = glCreateProgram();
  if(!pId) return pId;
  prg = addInProgramsList(pId);

  filename = firstone;
  va_start(pa, firstone);
  fprintf(stderr, "%s (%d): Creation du programme %d a l'aide des Shaders :\n", __FILE__, __LINE__, pId);
  do {
    if(!strncmp("<vs>", filename, 4)) { /* vertex shader */
      if(!(sId = gl4duCreateShader(GL_VERTEX_SHADER, &filename[4]))) goto gl4duCreateProgram_ERROR;
      attachShader(*prg, *findidInShadersList(sId));
      fprintf(stderr, "\t\t%s : vertex shader\n", &filename[4]);
    } else if(!strncmp("<fs>", filename, 4)) { /* fragment shader */
      if(!(sId = gl4duCreateShader(GL_FRAGMENT_SHADER, &filename[4]))) goto gl4duCreateProgram_ERROR;
      attachShader(*prg, *findidInShadersList(sId));
      fprintf(stderr, "\t\t%s : fragment shader\n", &filename[4]);
    }
#ifndef __ANDROID__
    else if(!strncmp("<gs>", filename, 4)) { /* geometry shader */
      if(!(sId = gl4duCreateShader(GL_GEOMETRY_SHADER, &filename[4]))) goto gl4duCreateProgram_ERROR;
      attachShader(*prg, *findidInShadersList(sId));
      fprintf(stderr, "\t\t%s : geometry shader\n", &filename[4]);
    }
#endif
    else { /* ??? shader */
      fprintf(stderr, "%s (%d): %s: erreur de syntaxe dans \"%s\"\n",
	      __FILE__, __LINE__, __func__, filename);
    }
  } while((filename = va_arg(pa, const char *)) != NULL);
  va_end(pa);
  glLinkProgram(pId);
  gl4duPrintProgramInfoLog(pId, stderr);
  return pId;
 gl4duCreateProgram_ERROR:
  va_end(pa);
  deleteFromProgramsList(prg);
  return 0;
}

/*!\brief créé un program à partir d'une liste (variable) de nom de
 * fichiers shaders encapsulés dans un fichier crypté préalablement
 * décrypté en ram et renvoie l'identifiant openGL du program créé.
 *
 * \param encData fichier contenant un ensemble de shaders cryptés.
 *
 * \param firstone premier des n paramètres de la présente fonction à
 * arguments variables ; ces arguments donnent les noms de fichiers
 * contenus dans l'archive cryptée \a endCata.  Ces noms de fichiers
 * shaders sont précédés d'un tag indiquant le type de shader : \<vs\>
 * pour un vertex shader et \<fs\> pour un fragment shader et \<gs\>
 * pour un geometry shader.
 *
 * \return l'identifiant openGL du program créé sinon 0. Dans ce
 * dernier cas les shaders créés (compilés) avant l'échec ne sont pas
 * supprimés ; un appel à \ref gl4duCleanUnattached peut s'en charger.
 */
GLuint gl4duCreateProgramFED(const char * encData, const char * firstone, ...) {
  static const char * prevEncData = NULL;
  static char * decData = NULL;
  va_list  pa;
  const char * filename;
  program_t ** prg;
  GLuint sId, pId = glCreateProgram();
  if(!pId) return pId;
  if(prevEncData != encData) {
    if(decData)
      free(decData);
    decData = aes_from_tar(prevEncData = encData);
  }
  prg = addInProgramsList(pId);

  filename = firstone;
  va_start(pa, firstone);
  fprintf(stderr, "%s (%d): Creation du programme %d a l'aide des Shaders :\n", __FILE__, __LINE__, pId);
  do {
    if(!strncmp("<vs>", filename, 4)) { /* vertex shader */
      if(!(sId = gl4duCreateShaderFED(decData, GL_VERTEX_SHADER, &filename[4]))) goto gl4duCreateProgram_ERROR;
      attachShader(*prg, *findidInShadersList(sId));
      fprintf(stderr, "\t\t%s : vertex shader\n", &filename[4]);
    } else if(!strncmp("<fs>", filename, 4)) { /* fragment shader */
      if(!(sId = gl4duCreateShaderFED(decData, GL_FRAGMENT_SHADER, &filename[4]))) goto gl4duCreateProgram_ERROR;
      attachShader(*prg, *findidInShadersList(sId));
      fprintf(stderr, "\t\t%s : fragment shader\n", &filename[4]);
    }
#ifndef __ANDROID__
    else if(!strncmp("<gs>", filename, 4)) { /* geometry shader */
      if(!(sId = gl4duCreateShaderFED(decData, GL_GEOMETRY_SHADER, &filename[4]))) goto gl4duCreateProgram_ERROR;
      attachShader(*prg, *findidInShadersList(sId));
      fprintf(stderr, "\t\t%s : geometry shader\n", &filename[4]);
    }
#endif
    else { /* ??? shader */
      fprintf(stderr, "%s (%d): %s: erreur de syntaxe dans \"%s\"\n",
	      __FILE__, __LINE__, __func__, filename);
    }
  } while((filename = va_arg(pa, const char *)) != NULL);
  va_end(pa);
  glLinkProgram(pId);
  gl4duPrintProgramInfoLog(pId, stderr);
  return pId;
 gl4duCreateProgram_ERROR:
  va_end(pa);
  deleteFromProgramsList(prg);
  return 0;
}

/*!\brief supprime le program dont l'identifiant openGL est \a id de la
 * liste de programs \ref programs_list.
 */
void gl4duDeleteProgram(GLuint id) {
  program_t ** prg = findInProgramsList(id);
  if(*prg) deleteFromProgramsList(prg);
}

/*!\brief supprime tous les programs et/ou tous les shaders.
 */
void gl4duClean(GL4DUenum what) {
  if(what & GL4DU_PROGRAM) {
    program_t ** ptr = &programs_list;
    while(*ptr)
      deleteFromProgramsList(ptr);
  }
  if(what & GL4DU_SHADER) {
    shader_t ** ptr = &shaders_list;
    while(*ptr)
      deleteFromShadersList(ptr);
  }
  if(what & GL4DU_MATRICES)
    btFree(&_gl4duMatrices, freeGL4DUMatrix);
}

/*!\brief supprime programs et/ou shaders non liés.
 */
void gl4duCleanUnattached(GL4DUenum what) {
  if(what & GL4DU_PROGRAM) {
    program_t ** ptr = &programs_list;
    while(*ptr) {
      if((*ptr)->nshaders <= 0) /* ne devrait pas être négatif */
	deleteFromProgramsList(ptr);
      else
	ptr = &((*ptr)->next);
    }
  }
  if(what & GL4DU_SHADER) {
    shader_t ** ptr = &shaders_list;
    while(*ptr) {
      if((*ptr)->nprograms <= 0) /* ne devrait pas être négatif */
	deleteFromShadersList(ptr);
      else
	ptr = &((*ptr)->next);
    }
  }
}

/*!\brief parcours la liste des shaders \ref shaders_list et vérifie
 * s'il y a besoin de mettre à jour le shader (recompiler et relinker).
 *
 * La vérification est effectuée sur la date de modification du
 * fichier representant le shader.
 *
 * \return 1 s'il y a eu une mise à jour (recompilation et relink)
 * sinon 0.
 *
 * \todo peut être ajouter un test sur le temps passé pour ne
 * parcourir la liste qu'un fois tous les 1/10 de secondes ????
*/
int gl4duUpdateShaders(void) {
  GLenum ot;
  char * fn;
  int maj = 0, i, n;
  struct stat buf;
  program_t ** p;
  shader_t ** ptr = &shaders_list;
#ifdef COMMERCIAL_V
  return 0;
#endif
  while(*ptr) {
    if(stat((*ptr)->filename, &buf) != 0) {
      fprintf(stderr, "%s:%d:In %s: erreur %d: %s\n",
	      __FILE__, __LINE__, __func__, errno, strerror(errno));
      return 0;
    }
    if((*ptr)->mod_time != buf.st_mtime) {
      if((n = (*ptr)->nprograms)) {
	p = malloc(n * sizeof * p);
	assert(p);
	memcpy(p, (*ptr)->programs, n * sizeof * p);
	for(i = 0; i < n; i++)
	  detachShader(p[i], *ptr);
	ot = (*ptr)->shadertype;
	fn = strdup((*ptr)->filename);
	deleteFromShadersList(ptr);
	ptr = addInShadersList(ot, fn);
	for(i = 0; i < n; i++) {
	  attachShader(p[i], *ptr);
	  glLinkProgram(p[i]->id);
	}
	free(p);
	free(fn);
      } else {
	ot = (*ptr)->shadertype;
	fn = strdup((*ptr)->filename);
	deleteFromShadersList(ptr);
	ptr = addInShadersList(ot, fn);
      }
      maj = 1;
    } else
      ptr = &((*ptr)->next);
  }
  return maj;
}

/*!\brief recherche un shader à partir du nom de fichier dans la liste
 * \ref shaders_list.
 *
 * Si le shader n'existe pas la fonction retourne NULL;
 *
 * \param filename le nom (le chemin entier (relatif)) du fichier
 * contenant le shader
 *
 * \return le pointeur de pointeur vers le shader.
 */
static shader_t ** findfnInShadersList(const char * filename) {
  shader_t ** ptr = &shaders_list;
  while(*ptr) {
    if(!strcmp(filename, (*ptr)->filename))
      return ptr;
    ptr = &((*ptr)->next);
  }
  return ptr;
}

/*!\brief recherche un shader à partir de son identifiant openGL dans
 * la liste \ref shaders_list.
 *
 * Si le shader n'existe pas la fonction retourne NULL;
 *
 * \param id l'identifiant openGL du shader.
 *
 * \return le pointeur de pointeur vers le shader.
 */
static shader_t ** findidInShadersList(GLuint id) {
  shader_t ** ptr = &shaders_list;
  while(*ptr) {
    if(id == (*ptr)->id)
      return ptr;
    ptr = &((*ptr)->next);
  }
  return ptr;
}

/*!\brief ajoute un nouveau shader dans la liste de shaders \ref shaders_list.
 *
 * \return l'adresse du shader ajouté sinon NULL.
 */
static shader_t ** addInShadersList(GLenum shadertype, const char * filename) {
  GLuint id;
  char * txt;
  struct stat buf;
  shader_t * ptr;
  if(!(id = glCreateShader(shadertype))) {
    fprintf(stderr, "%s (%d): %s: impossible de créer le shader\nglCreateShader a retourné 0\n",
	    __FILE__, __LINE__, __func__);
    return NULL;
  }
  if(!(txt = gl4dReadTextFile(filename))) {
    glDeleteShader(id);
    return NULL;
  }
  if(stat(filename, &buf) != 0) {
    fprintf(stderr, "%s:%d:In %s: erreur %d: %s\n",
	    __FILE__, __LINE__, __func__, errno, strerror(errno));
    glDeleteShader(id);
    return NULL;
  }
  ptr = shaders_list;
  shaders_list = malloc(sizeof * shaders_list);
  assert(shaders_list);
  shaders_list->id         = id;
  shaders_list->shadertype = shadertype;
  shaders_list->filename   = strdup(filename);
  shaders_list->mod_time   = buf.st_mtime;
  shaders_list->todelete   = 0;
  shaders_list->nprograms  = 0;
  shaders_list->sprograms  = 2;
  shaders_list->next       = ptr;
  shaders_list->programs   = malloc(shaders_list->sprograms * sizeof shaders_list->programs);
  assert(shaders_list->programs);
  glShaderSource(id, 1, (const char **)&txt, NULL);
  glCompileShader(id);
  gl4duPrintShaderInfoLog(id, stderr);
  free(txt);
  return &shaders_list;
}

/*!\brief ajoute un nouveau shader dans la liste de shaders \ref shaders_list.
 * Version FED
 *
 * \return l'adresse du shader ajouté sinon NULL.
 */
static shader_t ** addInShadersListFED(const char * decData, GLenum shadertype, const char * filename) {
  GLuint id;
  char * txt;
  shader_t * ptr;
  if(!(id = glCreateShader(shadertype))) {
    fprintf(stderr, "%s (%d): %s: impossible de créer le shader\nglCreateShader a retourné 0\n",
	    __FILE__, __LINE__, __func__);
    return NULL;
  }
  if(!(txt = gl4dExtractFromDecData(decData, filename))) {
    glDeleteShader(id);
    return NULL;
  }
  ptr = shaders_list;
  shaders_list = malloc(sizeof * shaders_list);
  assert(shaders_list);
  shaders_list->id         = id;
  shaders_list->shadertype = shadertype;
  shaders_list->filename   = strdup(filename);
  shaders_list->mod_time   = INT_MAX;
  shaders_list->todelete   = 0;
  shaders_list->nprograms  = 0;
  shaders_list->sprograms  = 2;
  shaders_list->next       = ptr;
  shaders_list->programs   = malloc(shaders_list->sprograms * sizeof shaders_list->programs);
  assert(shaders_list->programs);
  glShaderSource(id, 1, (const char **)&txt, NULL);
  glCompileShader(id);
  gl4duPrintShaderInfoLog(id, stderr);
  free(txt);
  return &shaders_list;
}

/*!\brief supprime le shader pointé par \a shp de la liste de shaders
 * \ref shaders_list.
 */
static void deleteFromShadersList(shader_t ** shp) {
  shader_t * ptr = *shp;
  *shp = (*shp)->next;
  free(ptr->filename);
  free(ptr->programs);
  glDeleteShader(ptr->id);
  free(ptr);
}

/*!\brief recherche un program à partir de son identifiant openGL dans
 * la liste \ref programs_list.
 *
 * Si le program n'existe pas la fonction retourne NULL;
 *
 * \return le pointeur de pointeur vers le program.
 */
static program_t ** findInProgramsList(GLuint id) {
  program_t ** ptr = &programs_list;
  while(*ptr) {
    if(id == (*ptr)->id)
      return ptr;
    ptr = &((*ptr)->next);
  }
  return ptr;
}

/*!\brief ajoute un nouveau program dans la liste de program \ref
 * programs_list.
 *
 * \return l'adresse du program ajouté sinon NULL.
 */
static program_t ** addInProgramsList(GLuint id) {
  program_t * ptr = programs_list;
  programs_list = malloc(sizeof * programs_list);
  assert(programs_list);
  programs_list->id       = id;
  programs_list->nshaders = 0;
  programs_list->sshaders = 4;
  programs_list->next     = ptr;
  programs_list->shaders  = malloc(programs_list->sshaders * sizeof * programs_list->shaders);
  assert(programs_list->shaders);
  return &programs_list;
}

/*!\brief supprime le program pointé par \a pp de la liste des programs
 * \ref programs_list.
 */
static void deleteFromProgramsList(program_t ** pp) {
  int i;
  program_t * ptr = *pp;
  *pp = (*pp)->next;
  for(i = 0; i < ptr->nshaders; i++)
    detachShader(ptr, ptr->shaders[i]);
  free(ptr->shaders);
  glDeleteProgram(ptr->id);
  free(ptr);
}

/*!\brief attache (lie) un program à un shader et vice versa.
 */
static void attachShader(program_t * prg, shader_t * sh) {
  int i;
  for(i = 0; i < prg->nshaders; i++)
    if(prg->shaders[i] == sh) return;
  for(i = 0; i < sh->nprograms; i++)
    if(sh->programs[i] == prg) return;
  if(prg->nshaders >= prg->sshaders) { /* ne devrait pas être supérieur */
    prg->shaders = realloc(prg->shaders, (prg->sshaders <<= 1) * sizeof * prg->shaders);
    assert(prg->shaders);
  }
  if(sh->nprograms >= sh->sprograms) { /* ne devrait pas être supérieur */
    sh->programs = realloc(sh->programs, (sh->sprograms <<= 1) * sizeof * sh->programs);
    assert(sh->programs);
  }
  prg->shaders[prg->nshaders++] =  sh;
  sh->programs[sh->nprograms++] = prg;
  glAttachShader(prg->id, sh->id);
}

/*!\brief détache (délie) un program à un shader et vice versa.
 */
static void detachShader(program_t * prg, shader_t * sh) {
  int i;
  glDetachShader(prg->id, sh->id);
  for(i = 0; i < prg->nshaders; i++)
    if(prg->shaders[i] == sh) break;
  for(prg->nshaders--; i < prg->nshaders; i++)
    prg->shaders[i] = prg->shaders[i + 1];
  for(i = 0; i < sh->nprograms; i++)
    if(sh->programs[i] == prg) break;
  for(sh->nprograms--; i < sh->nprograms; i++)
    sh->programs[i] = sh->programs[i + 1];
  if(sh->nprograms <= 0 && sh->todelete) /* ne devrait pas être négatif */
    deleteFromShadersList(findidInShadersList(sh->id));
}

/*!\brief créé une nouvelle pile de "une matrice 4x4" dont le nom
 * est \a name et le type est \a type.
 *
 * La (pile de) matrice créée n'est pas initialisée et le haut de la
 * pile pointe déjà sur une matrice.
 *
 * \param type est le type de données utilisé dans la matrice, peut
 * etre \a GL_FLOAT ou \a GL_DOUBLE.
 *
 * \param name est le nom unique donné à la matrice.
 *
 * \return la nouvelle pile de "une matrice 4x4" créée.
 */
static inline _GL4DUMatrix * newGL4DUMatrix(GLenum type, const char * name) {
  _GL4DUMatrix * m = malloc(sizeof *m);
  assert(m);
  m->name  = strdup(name);
  m->type  = type;
  m->size  = 16 * ( (type == GL_FLOAT) ? sizeof (GLfloat) : /* type == GL_DOUBLE */ sizeof (GLdouble) );
  m->nmemb = 16;
  m->top   = 0; /* déjà une matrice en haut de la pile */
  m->data  = malloc(m->size * m->nmemb);
  assert(m->data);
  return m;
}

/*!\brief libère la pile de "une matrice 4x4".
 *
 * \param matrix est la pile de "une matrice 4x4".
 */
static inline void freeGL4DUMatrix(void * matrix) {
  free(((_GL4DUMatrix *)matrix)->name);
  free(((_GL4DUMatrix *)matrix)->data);
  free(matrix);
}

/*!\brief fonction de comparaison de deux matrices en fonction du nom
 * pour insertion dans l'arbre binaire.
 *
 * Cette fonction utilise strcmp.
 *
 * \param m1 première matrice (de type _GL4DUMatrix).
 *
 * \param m2 seconde matrice (de type _GL4DUMatrix).
 *
 * \return entier négatif, nul ou positif selon que le nom de la
 * première soit respectivement inférieur, égal ou supérieur à la
 * seconde.
 */
static inline int matrixCmpFunc(const void * m1, const void * m2) {
  return strcmp( ((_GL4DUMatrix *)m1)->name, ((_GL4DUMatrix *)m2)->name );
}

/*!\brief recherche et renvoie le pointeur de pointeur vers le noeud
 * de l'arbre binaire pointant vers la matrice dont le nom est passé
 * en argument (\a name).
 *
 * \param name le nom de la matrice recherchée.
 *
 * \return le pointeur de pointeur vers le noeud de l'arbre binaire
 * pointant vers la matrice dont le nom est passé en argument (\a
 * name).
 */
static inline bin_tree_t ** findMatrix(const char * name) {
  pair_t pair;
  _GL4DUMatrix m;
  if(_gl4dLastMatrixn && strcmp(((_GL4DUMatrix *)((*_gl4dLastMatrixn)->data))->name, name) == 0)
    return _gl4dLastMatrixn;
  m.name = (char *)name;
  pair = btFind(&_gl4duMatrices, &m, matrixCmpFunc);
  if(!pair.compResult)
    return (bin_tree_t **)pair.ptr;
  return NULL;
}

/*!\brief génère et gère une matrice (pile de "une matrice 4x4") liée
 * au nom \a name et de type \a type.
 *
 * Le type peut etre GL_FLOAT ou GL_DOUBLE.
 *
 * \param type le type de la matrice à créer : GL_FLOAT ou GL_DOUBLE.
 * \param name le nom de la matrice à créer.
 *
 * \return GL_TRUE si la création est réussie, GL_FALSE sinon (apriori
 * si le nom existe déjà).
 */
GLboolean gl4duGenMatrix(GLenum type, const char * name) {
  _GL4DUMatrix m, * p;
  pair_t pair;
  m.name = (char *)name;
  pair = btFind(&_gl4duMatrices, &m, matrixCmpFunc);
  if(pair.compResult) {
    p = newGL4DUMatrix(type, name);
    _gl4dLastMatrixn = (bin_tree_t **)(btInsert((bin_tree_t **)(pair.ptr), p, matrixCmpFunc).ptr);
    return GL_TRUE;
  }
  return GL_FALSE;
}

/*!\brief indique s'il existe une matrice est liée
 * au nom \a name passé en argument.
 *
 * \param name le nom de la matrice à rechercher.
 *
 * \return GL_TRUE si la matrice existe, GL_FALSE sinon.
 */
GLboolean gl4duIsMatrix(const char * name) {
  _GL4DUMatrix m;
  m.name = (char *)name;
  return !btFind(&_gl4duMatrices, &m, matrixCmpFunc).compResult;
}

/*!\brief active (met en current) la matrice liée au nom \a name passé
 * en argument.
 *
 * Si la matrice existe l'active et renvoie GL_TRUE, sinon renvoie
 * GL_FALSE. Si le paramètre \a name est NULL, désactive toute
 * matrice.
 *
 * \param name le nom de la matrice à rechercher pour activer. Si NULL
 * désactive (dé-bind) tout.
 *
 * \return GL_TRUE si la matrice existe (et elle est préalablement
 * activée), GL_FALSE sinon.
 */
GLboolean gl4duBindMatrix(const char * name) {
  bin_tree_t ** bt;
  if(!name) {
    _gl4dCurMatrix = NULL;
    return GL_TRUE;
  }
  bt = findMatrix(name);
  if(bt) {
    _gl4dCurMatrix = (_GL4DUMatrix *)((*bt)->data);
    return GL_TRUE;
  }
  return GL_FALSE;
}

/*!\brief supprime la matrice liée au nom \a name passé en argument.
 *
 * Si la matrice existe la supprime et renvoie GL_TRUE, sinon renvoie
 * GL_FALSE. Si la matrice supprimée est la meme que la matrice
 * courante (active), la matrice courante passe à NULL.
 *
 * \param name le nom de la matrice à rechercher pour suppression.
 *
 * \return GL_TRUE si la matrice existe et est supprimée, GL_FALSE
 * sinon.
 */
GLboolean gl4duDeleteMatrix(const char * name) {
  bin_tree_t ** bt = findMatrix(name);
  if(bt) {
    if(_gl4dCurMatrix == (_GL4DUMatrix *)((*bt)->data))
      _gl4dCurMatrix = NULL;
    btDelete(bt, freeGL4DUMatrix);
    if(_gl4dLastMatrixn == bt)
      _gl4dLastMatrixn = NULL;
    return GL_TRUE;
  }
  return GL_FALSE;
}

/*!\brief empile (sauvegarde) la matrice courante et utilise une
 * nouvelle matrice dont le contenu est le meme que celle empilée.
 *
 * La pile est dynamique, sa taille non limitée.
 */
void gl4duPushMatrix(void) {
  assert(_gl4dCurMatrix);
  if(++_gl4dCurMatrix->top == _gl4dCurMatrix->nmemb) {
    _gl4dCurMatrix->data  = realloc(_gl4dCurMatrix->data, _gl4dCurMatrix->size * (_gl4dCurMatrix->nmemb <<= 1));
    assert(_gl4dCurMatrix->data);
  }
  memcpy(&(((GLubyte *)_gl4dCurMatrix->data)[_gl4dCurMatrix->top * _gl4dCurMatrix->size]),
	 &(((GLubyte *)_gl4dCurMatrix->data)[(_gl4dCurMatrix->top - 1) * _gl4dCurMatrix->size]),
	 _gl4dCurMatrix->size);
}

/*!\brief dépile la matrice courante en restaurant l'état précédemment
 * sauvegardé à l'aide de \ref gl4duPushMatrix.
 */
void gl4duPopMatrix(void) {
  assert(_gl4dCurMatrix);
  if(_gl4dCurMatrix->top)
    --_gl4dCurMatrix->top;
}

/*!\brief utilisée par \ref btForAll pour envoyer la matrice \a m sur
 * le program Id pointé par \a ppId.
 * \todo ajouter la gestion des GLdouble
 */
void sendMatrix(void * m, void **ppId) {
  _GL4DUMatrix * matrix = (_GL4DUMatrix * )m;
  GLint pId = *(GLint *)ppId;
#ifdef __ANDROID__
  /*!\todo voir pourquoi le transpose génère une erreur sous Android */
  GLfloat t[16], * M = matrixData(matrix);
  t[0] = M[0]; t[1] = M[4]; t[2] = M[8]; t[3] = M[12];
  t[4] = M[1]; t[5] = M[5]; t[6] = M[9]; t[7] = M[13];
  t[8] = M[2]; t[9] = M[6]; t[10] = M[10]; t[11] = M[14];
  t[12] = M[3]; t[13] = M[7]; t[14] = M[11]; t[15] = M[15];
  glUniformMatrix4fv(glGetUniformLocation(pId, matrix->name), 1, GL_FALSE, t);
  //__android_log_print(ANDROID_LOG_ERROR, "AndroidGL4D", "PROGRAM %d, Matrix %s : %d\n", pId, matrix->name, glGetUniformLocation(pId, matrix->name));
#else
  glUniformMatrix4fv(glGetUniformLocation(pId, matrix->name), 1, GL_TRUE, matrixData(matrix));
#endif
}

/*!\brief envoie la matrice courante au program shader en cours et en
 * utilisant le nom de la matrice pour obtenir le uniform location.
 */
void gl4duSendMatrix(void) {
  GLint pId;
  assert(_gl4dCurMatrix);
  glGetIntegerv(GL_CURRENT_PROGRAM, &pId);
  sendMatrix(_gl4dCurMatrix, (void **)&pId);
}

/*!\brief envoie toutes matrices au program shader en cours et en
 * utilisant leurs noms pour obtenir le uniform location.
 */
void gl4duSendMatrices(void) {
  GLint pId;
  glGetIntegerv(GL_CURRENT_PROGRAM, &pId);
  btForAll(_gl4duMatrices, sendMatrix, (void **)&pId);
}

/*!\brief Création d'une matrice de projection selon l'ancienne
 * fonction glFrustum et la stocke dans la matrice en cours.
 *
 * La matrice en cours doit être de type GLfloat sinon utiliser \a
 * gl4duFrustumd si le type est GLdouble.
 *
 * \see gl4duFrustumd
 *
 * \todo créer le modèle de gestion de matrices en cours avec piles de matrices
 * \todo rajouter gl4duOrthof et gl4duLookAtf
 */
void gl4duFrustumf(GLfloat  l, GLfloat  r, GLfloat  b, GLfloat  t, GLfloat  n, GLfloat  f) {
  GLfloat frustum[16];
  MFRUSTUM(frustum, l, r, b, t, n, f);
  gl4duMultMatrixf(frustum);
}

/*!\brief Création d'une matrice de projection selon l'ancienne
 * fonction glFrustum et la stocke dans la matrice en cours.
 *
 * La matrice en cours doit être de type GLdouble sinon utiliser \a
 * gl4duFrustumf si le type est GLfloat.
 *
 * \see gl4duFrustumf
 *
 * \todo créer le modèle de gestion de matrices en cours avec piles de matrices
 * \todo rajouter gl4duOrthod et gl4duLookAtd
 */
void gl4duFrustumd(GLdouble l, GLdouble r, GLdouble b, GLdouble t, GLdouble n, GLdouble f) {
  GLdouble frustum[16];
  MFRUSTUM(frustum, l, r, b, t, n, f);
  gl4duMultMatrixd(frustum);
}

/*!\brief Chargement d'une matrice identité dans la matrice en cours.
 *
 * La matrice en cours doit être de type GLfloat sinon utiliser \a
 * gl4duLoadIdentityd si le type est GLdouble.
 *
 * \see gl4duLoadIdentityd
 */
void gl4duLoadIdentityf(void) {
  GLfloat * mat;
  assert(_gl4dCurMatrix);
  assert(_gl4dCurMatrix->type == GL_FLOAT);
  mat = (GLfloat *)&(((GLubyte *)_gl4dCurMatrix->data)[_gl4dCurMatrix->top * _gl4dCurMatrix->size]);
  MIDENTITY(mat);
}

/*!\brief Chargement d'une matrice identité dans la matrice en cours.
 *
 * La matrice en cours doit être de type GLdouble sinon utiliser \a
 * gl4duLoadIdentityf si le type est GLfloat.
 *
 * \see gl4duLoadIdentityf
 */
void gl4duLoadIdentityd(void) {
  GLdouble * mat;
  assert(_gl4dCurMatrix);
  assert(_gl4dCurMatrix->type == GL_DOUBLE);
  mat = (GLdouble *)&(((GLubyte *)_gl4dCurMatrix->data)[_gl4dCurMatrix->top * _gl4dCurMatrix->size]);
  MIDENTITY(mat);
}

/*!\brief Chargement d'une matrice \a matrix dans la matrice en cours.
 *
 * La matrice en cours doit être de type GLfloat sinon utiliser \a
 * gl4duLoadMatrixd si le type est GLdouble.
 *
 * \param matrix la matrice 4x4 à charger.
 *
 * \see gl4duLoadMatrixd
 */
void gl4duLoadMatrixf(const GLfloat * matrix) {
  assert(_gl4dCurMatrix);
  assert(_gl4dCurMatrix->type == GL_FLOAT);
  memcpy(&(((GLubyte *)_gl4dCurMatrix->data)[_gl4dCurMatrix->top * _gl4dCurMatrix->size]), matrix, _gl4dCurMatrix->size);
}

/*!\brief Chargement d'une matrice \a matrix dans la matrice en cours.
 *
 * La matrice en cours doit être de type GLdouble sinon utiliser \a
 * gl4duLoadMatrixf si le type est GLfloat.
 *
 * \param matrix la matrice 4x4 à charger.
 *
 * \see gl4duLoadMatrixf
 */
void gl4duLoadMatrixd(const GLdouble * matrix) {
  assert(_gl4dCurMatrix);
  assert(_gl4dCurMatrix->type == GL_DOUBLE);
  memcpy(&(((GLubyte *)_gl4dCurMatrix->data)[_gl4dCurMatrix->top * _gl4dCurMatrix->size]), matrix, _gl4dCurMatrix->size);
}

/*!\brief Multiplication de la matrice en cours par une matrice \a matrix.
 *
 * La matrice en cours doit être de type GLfloat sinon utiliser \a
 * gl4duMultMatrixd si le type est GLdouble.
 *
 * \param matrix la matrice 4x4 utilisée pour la multiplication (curMatrix x matrix).
 *
 * \see gl4duMultMatrixd
 */
void gl4duMultMatrixf(const GLfloat * matrix) {
  GLfloat * mat, cpy[16];
  assert(_gl4dCurMatrix);
  assert(_gl4dCurMatrix->type == GL_FLOAT);
  mat = (GLfloat *)&(((GLubyte *)_gl4dCurMatrix->data)[_gl4dCurMatrix->top * _gl4dCurMatrix->size]);
  memcpy(cpy, mat, _gl4dCurMatrix->size);
  MMAT4XMAT4(mat, cpy, matrix);
}

/*!\brief Multiplication de la matrice en cours par une matrice \a matrix.
 *
 * La matrice en cours doit être de type GLdouble sinon utiliser \a
 * gl4duMultMatrixf si le type est GLfloat.
 *
 * \param matrix la matrice 4x4 utilisée pour la multiplication (curMatrix x matrix).
 *
 * \see gl4duMultMatrixf
 */
void gl4duMultMatrixd(const GLdouble * matrix) {
  GLdouble * mat, cpy[16];
  assert(_gl4dCurMatrix);
  assert(_gl4dCurMatrix->type == GL_FLOAT);
  mat = (GLdouble *)&(((GLubyte *)_gl4dCurMatrix->data)[_gl4dCurMatrix->top * _gl4dCurMatrix->size]);
  memcpy(cpy, mat, _gl4dCurMatrix->size);
  MMAT4XMAT4(mat, cpy, matrix);
}

/*!\brief Multiplication de la matrice en cours par une matrice de
 * rotation définie par un angle \a angle donné en degrés autour de
 * l'axe (\a x, \a y, \a z).
 *
 * \param angle angle de rotation en degrés.
 * \param x abcsisse de l'axe de rotation.
 * \param y ordonnée de l'axe de rotation.
 * \param z cote de l'axe de rotation.
 */
void gl4duRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z) {
  GLfloat n = sqrtf(x * x + y * y + z * z);
  if ( n > 0.0f ) {
    GLfloat a, s, c, cc, x2, y2, z2, xy, yz, zx, xs, ys, zs;
    GLfloat mat[] = { 0.0f, 0.0f, 0.0f, 0.0f,
		      0.0f, 0.0f, 0.0f, 0.0f,
		      0.0f, 0.0f, 0.0f, 0.0f,
		      0.0f, 0.0f, 0.0f, 1.0f };
    s  = sinf ( a = (angle * (GLfloat)GL4DM_PI / 180.0f) );
    cc = 1.0f - (c = cosf ( a ));
    x /= n;     y /= n;     z /= n;
    x2 = x * x; y2 = y * y; z2 = z * z;
    xy = x * y; yz = y * z; zx = z * x;
    xs = x * s; ys = y * s; zs = z * s;
    mat[0]  = (cc * x2) + c;
    mat[1]  = (cc * xy) - zs;
    mat[2]  = (cc * zx) + ys;
    /* mat[3]  = 0.0f; */
    mat[4]  = (cc * xy) + zs;
    mat[5]  = (cc * y2) + c;
    mat[6]  = (cc * yz) - xs;
    /* mat[7]  = 0.0f; */
    mat[8]  = (cc * zx) - ys;
    mat[9]  = (cc * yz) + xs;
    mat[10] = (cc * z2) + c;
    /* mat[11] = 0.0f; */
    /* mat[12] = 0.0f; mat[13] = 0.0f; mat[14] = 0.0f; mat[15] = 1.0f; */
    gl4duMultMatrixf(mat);
  }
}

/*!\brief Multiplication de la matrice en cours par une matrice de
 * rotation définie par un angle \a angle donné en degrés autour de
 * l'axe (\a x, \a y, \a z).
 *
 * \param angle angle de rotation en degrés.
 * \param x abcsisse de l'axe de rotation.
 * \param y ordonnée de l'axe de rotation.
 * \param z cote de l'axe de rotation.
 */
void gl4duRotated(GLdouble angle, GLdouble x, GLdouble y, GLdouble z) {
  GLdouble n = sqrt(x * x + y * y + z * z);
  if ( n > 0.0 ) {
    GLdouble a, s, c, cc, x2, y2, z2, xy, yz, zx, xs, ys, zs;
    GLdouble mat[] = { 0.0, 0.0, 0.0, 0.0,
		       0.0, 0.0, 0.0, 0.0,
		       0.0, 0.0, 0.0, 0.0,
		       0.0, 0.0, 0.0, 1.0 };
    s  = sin ( a = (angle * GL4DM_PI / 180.0) );
    cc = 1.0 - (c = cos ( a ));
    x /= n;     y /= n;     z /= n;
    x2 = x * x; y2 = y * y; z2 = z * z;
    xy = x * y; yz = y * z; zx = z * x;
    xs = x * s; ys = y * s; zs = z * s;
    mat[0]  = (cc * x2) + c;
    mat[1]  = (cc * xy) - zs;
    mat[2]  = (cc * zx) + ys;
    /* mat[3]  = 0.0; */
    mat[4]  = (cc * xy) + zs;
    mat[5]  = (cc * y2) + c;
    mat[6]  = (cc * yz) - xs;
    /* mat[7]  = 0.0; */
    mat[8]  = (cc * zx) - ys;
    mat[9]  = (cc * yz) + xs;
    mat[10] = (cc * z2) + c;
    /* mat[11] = 0.0; */
    /* mat[12] = 0.0; mat[13] = 0.0; mat[14] = 0.0; mat[15] = 1.0; */
    gl4duMultMatrixd(mat);
  }
}

/*!\brief Multiplication de la matrice en cours par une matrice de
 * translation (\a tx, \a ty, \a tz).
 *
 * \param tx abcsisse de la translation.
 * \param ty ordonnée de la translation.
 * \param tz cote de la translation.
 */
void gl4duTranslatef(GLfloat tx, GLfloat ty, GLfloat tz) {
  GLfloat mat[] = { 1.0f, 0.0f, 0.0f, tx,
		    0.0f, 1.0f, 0.0f, ty,
		    0.0f, 0.0f, 1.0f, tz,
		    0.0f, 0.0f, 0.0f, 1.0f };
  gl4duMultMatrixf(mat);
}

/*!\brief Multiplication de la matrice en cours par une matrice de
 * translation (\a tx, \a ty, \a tz).
 *
 * \param tx abcsisse de la translation.
 * \param ty ordonnée de la translation.
 * \param tz cote de la translation.
 */
void gl4duTranslated(GLdouble tx, GLdouble ty, GLdouble tz) {
  GLdouble mat[] = { 1.0, 0.0, 0.0, tx,
		     0.0, 1.0, 0.0, ty,
		     0.0, 0.0, 1.0, tz,
		     0.0, 0.0, 0.0, 1.0 };
  gl4duMultMatrixd(mat);
}

/*!\brief Multiplication de la matrice en cours par une matrice
 *  d'homothétie (\a sx, \a sy, \a sz).
 *
 * \param sx abcsisse de l'homothétie.
 * \param sy ordonnée de l'homothétie.
 * \param sz cote de l'homothétie.
 */
void gl4duScalef(GLfloat sx, GLfloat sy, GLfloat sz) {
  GLfloat mat[] = { sx  , 0.0f, 0.0f, 0.0f,
		    0.0f,   sy, 0.0f, 0.0f,
		    0.0f, 0.0f,   sz, 0.0f,
		    0.0f, 0.0f, 0.0f, 1.0f };
  gl4duMultMatrixf(mat);
}

/*!\brief Multiplication de la matrice en cours par une matrice
 *  d'homothétie (\a sx, \a sy, \a sz).
 *
 * \param sx abcsisse de l'homothétie.
 * \param sy ordonnée de l'homothétie.
 * \param sz cote de l'homothétie.
 */
void gl4duScaled(GLdouble sx, GLdouble sy, GLdouble sz) {
  GLdouble mat[] = { sx , 0.0, 0.0, 0.0,
		     0.0,  sy, 0.0, 0.0,
		     0.0, 0.0,  sz, 0.0,
		     0.0, 0.0, 0.0, 1.0 };
  gl4duMultMatrixd(mat);
}

void gl4duLookAtf_DNW(GLfloat eyeX,  GLfloat eyeY,  GLfloat eyeZ,  GLfloat centerX,  GLfloat centerY,  GLfloat centerZ,  GLfloat upX,  GLfloat upY,  GLfloat upZ) {
  GLfloat haut[3] = { upX, upY, upZ }, dirVue[] = { centerX - eyeX, centerY - eyeY, centerZ - eyeZ };
  GLfloat mat[] = {
    0.0f,       0.0f,       0.0f,       0.0f,
    0.0f,       0.0f,       0.0f,       0.0f,
    -dirVue[0], -dirVue[1], -dirVue[2], 0.0f,
    0.0f,       0.0f,       0.0f,       1.0f
  };
  //MMAT4TRANSPOSE(mat);
  MVEC3NORMALIZE(dirVue);
  /* Première version
     GLfloat cote[3];
     MVEC3CROSS(cote, dirVue, haut); // j'ai envie de faire haut x dirVue
     MVEC3NORMALIZE(cote);
     mat[0] = cote[0]; mat[1] = cote[1]; mat[2] = cote[2];
     //\todo dire pourquoi (lié à la normalisation de cote)
     MVEC3CROSS(haut, cote, dirVue); // j'ai envie de faire dirVue x coté
     mat[4] = haut[0]; mat[5] = haut[1]; mat[6] = haut[2]; */
  MVEC3CROSS(mat, dirVue, haut);
  MVEC3NORMALIZE(mat);
  MVEC3CROSS(&mat[4], mat, dirVue);
  gl4duMultMatrixf(mat);
  gl4duTranslatef(-eyeX, -eyeY, -eyeZ);
}

/*!\brief Définie des transformations pour simuler un point de vue
 * avec direction de regard et orientation.
 *
 * Version \a GL4Dummies de la fonction gluLookAt pour les types
 * GLfloat (les matrices GL_FLOAT). Pour la version gérant les double
 * voir \see gl4duLookAtd.
 *
 * \param eyeX abcsisse de l'oeil.
 * \param eyeY ordonnée de l'oeil.
 * \param eyeZ cote de l'oeil.
 * \param centerX abcsisse du point observé.
 * \param centerY ordonnée du point observé.
 * \param centerZ cote du point observé.
 * \param upX X du vecteur décrivant l'orientation de la "tete" de l'observateur (vecteur haut)
 * \param upY Y du vecteur décrivant l'orientation de la "tete" de l'observateur (vecteur haut)
 * \param upZ Y du vecteur décrivant l'orientation de la "tete" de l'observateur (vecteur haut)
 */
void gl4duLookAtf(GLfloat eyeX, GLfloat eyeY, GLfloat eyeZ, GLfloat centerX, GLfloat centerY, GLfloat centerZ, GLfloat upX, GLfloat upY, GLfloat upZ) {
  GLfloat forward[3], side[3], up[3];
  GLfloat m[] = {
        1.0f,       0.0f,       0.0f,       0.0f,
        0.0f,       1.0f,       0.0f,       0.0f,
        0.0f,       0.0f,       1.0f,       0.0f,
        0.0f,       0.0f,       0.0f,       1.0f
  };
  forward[0] = centerX - eyeX;
  forward[1] = centerY - eyeY;
  forward[2] = centerZ - eyeZ;
  up[0] = upX;
  up[1] = upY;
  up[2] = upZ;
  MVEC3NORMALIZE(forward);
  /* side = forward x up */
  MVEC3CROSS(side, forward, up);
  MVEC3NORMALIZE(side);
  /* up = side x forward */
  MVEC3CROSS(up, side, forward);
  m[0] = side[0];
  m[4] = side[1];
  m[8] = side[2];
  m[1] = up[0];
  m[5] = up[1];
  m[9] = up[2];
  m[2] = -forward[0];
  m[6] = -forward[1];
  m[10] = -forward[2];
  gl4duMultMatrixf(m);
  gl4duTranslatef(-eyeX, -eyeY, -eyeZ);
}


/*!\brief Définie des transformations pour simuler un point de vue
 * avec direction de regard et orientation.
 *
 * Version \a GL4Dummies de la fonction gluLookAt pour les types
 * GLdouble (les matrices GL_DOUBLE). Pour la version gérant les
 * double voir \see gl4duLookAtf.
 *
 * \param eyeX abcsisse de l'oeil.
 * \param eyeY ordonnée de l'oeil.
 * \param eyeZ cote de l'oeil.
 * \param centerX abcsisse du point observé.
 * \param centerY ordonnée du point observé.
 * \param centerZ cote du point observé.
 * \param upX X du vecteur décrivant l'orientation de la "tete" de l'observateur (vecteur haut)
 * \param upY Y du vecteur décrivant l'orientation de la "tete" de l'observateur (vecteur haut)
 * \param upZ Y du vecteur décrivant l'orientation de la "tete" de l'observateur (vecteur haut)
 */
void gl4duLookAtd(GLdouble eyeX,  GLdouble eyeY,  GLdouble eyeZ,  GLdouble centerX,  GLdouble centerY,  GLdouble centerZ,  GLdouble upX,  GLdouble upY,  GLdouble upZ) {
  GLdouble forward[3], side[3], up[3];
  GLdouble m[] = {
          1.0,       0.0,       0.0,       0.0,
          0.0,       1.0,       0.0,       0.0,
          0.0,       0.0,       1.0,       0.0,
          0.0,       0.0,       0.0,       1.0
  };
  forward[0] = centerX - eyeX;
  forward[1] = centerY - eyeY;
  forward[2] = centerZ - eyeZ;
  up[0] = upX;
  up[1] = upY;
  up[2] = upZ;
  MVEC3NORMALIZE(forward);
  /* side = forward x up */
  MVEC3CROSS(side, forward, up);
  MVEC3NORMALIZE(side);
  /* up = side x forward */
  MVEC3CROSS(up, side, forward);
  m[0] = side[0];
  m[4] = side[1];
  m[8] = side[2];
  m[1] = up[0];
  m[5] = up[1];
  m[9] = up[2];
  m[2] = -forward[0];
  m[6] = -forward[1];
  m[10] = -forward[2];
  gl4duMultMatrixd(m);
  gl4duTranslated(-eyeX, -eyeY, -eyeZ);
}

/*!\brief retourne le pointeur vers les données de la matrice (pile de "une matrice 4x4") courante.
 *
 * En pratique, prend la donnée en haut de la pile. Le type de retour
 * est (void *), il faut le caster dans le type correspondant au type
 * de la matrice.
 *
 * \return retourne le pointeur vers les données de la matrice.
 */
static inline void * matrixData(_GL4DUMatrix * matrix) {
  return (void *)&(((GLubyte *)matrix->data)[matrix->top * matrix->size]);
}

/*!\brief retourne le pointeur vers les données de la matrice (pile de "une matrice 4x4") courante.
 *
 * En pratique, prend la donnée en haut de la pile. Le type de retour
 * est (void *), il faut le caster dans le type correspondant au type
 * de la matrice.
 *
 * \return retourne le pointeur vers les données de la matrice.
 */
void * gl4duGetMatrixData(void) {
  assert(_gl4dCurMatrix);
  return matrixData(_gl4dCurMatrix);
}

/*!\brief Renseigne sur la valeur du paramètre demandé. Le paramètre
 * est envoyé via \a pname et sera stocké dans \a params.
 *
 * \param pname le paramètre demandé.
 * \param params là où sera stockée la/les valeur(s) du paramètre demandé.
 *
 * \return GL_TRUE si l'opération est réussie, GL_FALSE sinon (ex. le
 * \a pname est non géré par cette fonction).
 *
 * \todo compléter cette fonction et bien la documenter (tous les cas gérés).
 *
 * \todo faire la meme mais pour les float, double ...
 */
GLboolean gl4duGetIntegerv(GL4DUenum pname, GLint * params) {
  switch(pname) {
  case GL4DU_MATRIX_TYPE:
    assert(_gl4dCurMatrix);
    *params = (GLint)_gl4dCurMatrix->type;
    return GL_TRUE;
  default:
    return GL_FALSE;
  }
  return GL_FALSE;
}
