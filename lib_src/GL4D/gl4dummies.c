/*!\file gl4dummies.c
 *
 * \brief gestion simplifiée de GL sous différents OS avec aide sur la
 * vidéo (OpenCV ?) et le cryptage automatique des shaders (à
 * implémenter sur l'ensemble des OS)
 *
 * \author Farès BELHADJ amsi@grafeet.fr (ou amsi@ai.univ-paris8.fr)
 * \date March 28, 2008 - February 07, 2014
 */

#if !defined(_WIN32)
#  include <unistd.h>
#endif
#include "gl4dummies.h"
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>

static int kmpSearch(const char * p, const char * t);

#if defined(_WIN32)
#  define getProcAddress(name) (void *) wglGetProcAddress((LPCSTR)name)
#else
#  if defined(__APPLE__)
#    define getProcAddress(name) (*SDL_GL_GetProcAddress)(name)
#  else
#    if defined(__sgi) || defined(__sun)
       void* dlGetProcAddress (const GLubyte* name);
#      define getProcAddress(name) dlGetProcAddress(name)
#    else /* __linux */
#      define getProcAddress(name) (*SDL_GL_GetProcAddress)(name)
#    endif
#  endif
#endif

/*!\brief recherche une extension dans glGetString(GL_EXTENSIONS)
 *
 * \return vrai si l'extension est trouvée
 */
int GL4DAPIENTRY gl4dExtensionSupported(const char * extension) {
  const char * extensions = (const char *)glGetString(GL_EXTENSIONS);
  return kmpSearch(extension, extensions) < (int)strlen(extensions) ? 1 : 0;
}

/*!\brief se charge de desalouer la memoire eventuellement utilisee
 * par gl4dummies
 */
void gl4dQuit(void) {
  kmpSearch(NULL, NULL);
}

/*!\brief recopie un fichier texte en mémoire et renvoie un pointeur
 * vers la donnée recopiée.
 *
 * \return un pointeur vers le texte recopié en mémoire. La mémoire
 * doit être libérée par free.
 */
char * gl4dReadTextFile(const char * filename) {
  int l;
  char * data = NULL;
  FILE * f;
  struct stat buf;
  if(stat(filename, &buf) != 0) {
    fprintf(stderr, "%s:%d:In %s: error %d: %s (%s)\n",
	    __FILE__, __LINE__, __func__, errno, strerror(errno), filename);
    return NULL;
  }
  data = malloc( (buf.st_size + 1) * sizeof * data );
  assert(data);
  if( (f = fopen(filename, "rb")) == NULL ) {
    fprintf(stderr, "%s:%d:In %s: error %d: %s (%s)\n",
	    __FILE__, __LINE__, __func__, errno, strerror(errno), filename);
    free(data);
    return NULL;
  }
#ifdef _WIN32
  if( (l = fread(data, sizeof * data, buf.st_size, f)) == 0) {
#else
  if( (l = fread(data, sizeof * data, buf.st_size, f)) != buf.st_size) {
#endif
    fprintf(stderr, "%s:%d:In %s: une erreur s'est produite lors de la lecture du fichier %s\n",
	    __FILE__, __LINE__, __func__, filename);
    free(data);
    fclose(f);
    return NULL;
  }
  data[l] = '\0';
  fclose(f);
  return data;
}

/*!\brief recherche le shader filename dans le dat décrypté decData
 * et retourne une copie du code.
 */
char * gl4dExtractFromDecData(const char * decData, const char * filename) {
	int i, j;
	char a[BUFSIZ], * r;
	snprintf(a, BUFSIZ, "<shader %s>", filename);
	if((i = kmpSearch(a, decData)) < 0)
		return NULL;
	if((j = kmpSearch("</shader>", &decData[i + strlen(a)])) < 0)
		return NULL;
	r = malloc((j + 1) * sizeof * r);
	assert(r);
	strncpy(r, &decData[i + strlen(a)], j);
	r[j] = 0;
	return r;
}

/*!\brief recherche un motif dans un text
 *
 * Utilise l'algorithme ameliore Knuth-Morris-Pratt. Ici
 * l'initialisation de l'automate est faite si besoin dans la meme
 * fonction. Pour liberer l'automate appeler la fonction avec le
 * premier argument egal a NULL.
 *
 *\return l'indice de la premiere occurence su motif
 */
static int kmpSearch(const char * p, const char * t) {
  int i, j, lt;
  static int lm = 0, * back = NULL;
  static const char * m = NULL;
  if(m != p || !p) {
    if(back){
      free(back);
      back = NULL;
    }
    if(!p) return -1;
    lm = strlen(m = p);
    back = malloc(lm * sizeof * back);
    assert(back);
    for(back[i = 0] = j = -1; i < lm - 1; i++, j++, back[i] = (m[i] == m[j]) ? back[j] : j)
      while( j >= 0 && m[i] != m[j] ) j = back[j];
  }
  lt = strlen(t);
  for(i = j = 0; j < lm && i < lt; i++, j++)
    while( j >= 0 && t[i] != m[j] ) { j = back[j]; }
  if(j == lm) return i - lm;
  return lt;
}

/****************************************************/
/****** Version windows/mingw de gettimeofday *******/
/****************************************************/
#if defined(_WIN32)
#  include <windows.h>
#  include <winsock2.h>
    static int gettimeofday(struct timeval* p, void* tz) {
        ULARGE_INTEGER ul; // As specified on MSDN.
        FILETIME ft;
        // Returns a 64-bit value representing the number of
        // 100-nanosecond intervals since January 1, 1601 (UTC).
        GetSystemTimeAsFileTime(&ft);
        // Fill ULARGE_INTEGER low and high parts.
        ul.LowPart = ft.dwLowDateTime;
        ul.HighPart = ft.dwHighDateTime;
        // Convert to microseconds.
        ul.QuadPart /= 10ULL;
        // Remove Windows to UNIX Epoch delta.
        ul.QuadPart -= 11644473600000000ULL;
        // Modulo to retrieve the microseconds.
        p->tv_usec = (long) (ul.QuadPart % 1000000LL);
        // Divide to retrieve the seconds.
        p->tv_sec = (long) (ul.QuadPart / 1000000LL);
        return 0;
    }
#else
#  include <sys/time.h>
#endif

/*!\brief Le temps initial du programme. A initialiser avec \ref
 * gl4dInitTime0.
 *
 * \see gl4dInitTime0
 */
static struct timeval t0;

static double getElapsedTime_sub1(void);
static double getElapsedTime_sub2(void);

static double (* getET)(void) = getElapsedTime_sub1;

static double getElapsedTime_sub1(void) {
	gl4dInitTime0();
	getET = getElapsedTime_sub2;
	return 0.0;
}

static double getElapsedTime_sub2(void) {
  struct timeval t;
  double diff;
  gettimeofday(&t, (struct timezone*) 0);
  diff = (t.tv_sec - t0.tv_sec) * 1000000
    + (t.tv_usec - t0.tv_usec);
  return diff/1000.;
}

/*!\brief Initialise \a t0.
 */
void gl4dInitTime0(void) {
  gettimeofday(&t0, (struct timezone*) 0);
}

/*!\brief Donne le temps ecoule en millisecondes depuis \a t0.
 *
 * \see initTime0
 *
 * \return le temps ecoule en millisecondes.
 */
double gl4dGetElapsedTime(void) {
	return getET();
}

/*!\brief Une variable de temps. A initialiser avec \ref
 * gl4dInitTime.
 *
 * \see gl4dInitTime
 */
static struct timeval ti;

/*!\brief Initialise \a ti.
 */
void gl4dInitTime(void) {
  gettimeofday(&ti, (struct timezone*) 0);
}

/*!\brief Donne le temps ecoule en millisecondes depuis \a ti.
 *
 * \see gl4dInitTime
 *
 * \return le temps ecoule en millisecondes.
 */
double gl4dGetTime(void) {
  struct timeval t;
  double diff;
  gettimeofday(&t, (struct timezone*) 0);
  diff = (t.tv_sec - ti.tv_sec) * 1000000
    + (t.tv_usec - ti.tv_usec);
  return diff/1000.;
}

/*!\brief Calcule le FPS - Frames Per Second.
 *
 * Mettre un appel a cette fonction juste apres (ou avant) chaque
 * affichage.
 *
 * \see gl4dInitTime0
 * \see getElapsedTime
 *
 * \return le nombre d'images par seconde.
 */
double gl4dGetFps(void) {
  static int f = 0;
  static double t0 = 0, t1 = 0, fps = 0.0;
  t1 = gl4dGetTime(); f++;
  if(t1 - t0 > 1000) {
    fps = 1000.0 * f / (t1 - t0);
    t0 = t1; f = 0;
  }
  return fps;
}

/*!\brief Déplace un fichier.
 *
 * \param src le chemin vers le fichier à  déplacer
 *
 * \param dst le nouveau chemin/nom
 *
 * \return 1 en cas de succès sinon 0.
 *
 * \todo gérer les dossiers, les unlink (liés à l'OS) ...
 *
 * \todo renommer et bouger ailleurs
 */
int mv(const char * src, const char * dst) {
  size_t l;
  char buf[BUFSIZ] = {0};
  FILE * in = fopen(src, "rb"), * out = fopen(dst, "wb") ;
  if(in == NULL || out == NULL)
    return 0;
  while((l = fread(buf, 1, sizeof buf, in)) > 0)
    fwrite(buf, 1, l, out);
  fclose(out); fclose(in);
  if(remove(src))
    return 1;
  return 0;
}

/*!\brief Renvoie le chemin vers le dossier contenant le fichier passé en argument.
 *
 * Fabrique la chaîne allant jusqu'à la dernière occurence du
 * '/'. Cette chaîne doit être libérée avec free après usage.
 *
 * \param path le chemin vers le fichier/dossier
 *
 * \return le chemin vers le dossier contenant le fichier passÃ© en
 * argument. La chaîne renvoyée doit être libérée avec free après
 * usage.
 *
 * \todo renommer et bouger ailleurs
 *
 */
char * pathOf(const char * path) {
  int spos = -1;
  char * tmp, * ptr;
  tmp = malloc((strlen(path) + 1) * sizeof * tmp); assert(tmp); strcpy(tmp, path); //strdup(path);
  ptr = tmp;
  while(*ptr) {
    if(*ptr == '/' || *ptr == '\\')
      spos = ptr - tmp;
    ++ptr;
  }
  tmp[spos >= 0 ? spos : 0] = 0;
  return tmp;
}

/*!\brief Renvoie le nom de fichier sans le chemin.
 *
 * Prend le nom du fichier à  partir de la dernière occurence du
 * '/'. Cette chaîne doit être libérée avec free après usage.
 *
 * \param path le chemin vers le fichier/dossier
 *
 * \return le nom de fichier sans le chemin. La chaîne renvoyée doit
 * être libérée avec free après usage.
 *
 * \todo renommer et bouger ailleurs
 *
 */
char * filenameOf(const char * path) {
  int spos = -1, l;
  const char * ptr = path;
  char * tmp;
  while(*ptr) {
    if(*ptr == '/')
      spos = ptr - path;
    ++ptr;
  }
  tmp = malloc((l = strlen(&path[++spos]) + 1) * sizeof *tmp);
  assert(tmp);
  strncpy(tmp, &path[spos], l - 1); tmp[l - 1] = 0;
  return tmp;
}


/**********************************************************************/
/** CE QUI SUIT A PARTIR DE LA NE CONCERNE QUE WINDOWS               **/
/**********************************************************************/

#if defined(_WIN32)

/*!\brief fait appel a glCreateShader ou glCreateShaderObjectARB selon
 * la disponibilité
 *
 * \return l'identifiant du shader créé
 */
GLuint gl4dCreateShader(GLenum shaderType) {
  GLuint (__stdcall *p)(GLenum);
  if((p = getProcAddress("glCreateShader")))
    return p(shaderType);
  else if((p = getProcAddress("glCreateShaderObjectARB")))
    return p(shaderType);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour creer le shader\n",
	    __FILE__, __LINE__, __func__);
    return 0;
  }
}

/*!\brief fait appel a glShaderSource ou glShaderSourceARB selon la
 * disponibilité
 */
void gl4dShaderSource(GLuint shader, GLint numOfStrings, const char ** strings, GLint * lenOfStrings) {
  void (__stdcall *p)(GLuint, GLint, const char **, GLint *);
  if((p = getProcAddress("glShaderSource"))) {
    p(shader, numOfStrings, strings, lenOfStrings);
  } else if((p = getProcAddress("glShaderSourceARB"))) {
    p(shader, numOfStrings, strings, lenOfStrings);
  } else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour recuperer le code source du shader\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glCompileShader ou glCompileShaderARB selon la
 * disponibilité
 */
void gl4dCompileShader(GLuint shader){
  void (__stdcall *p)(GLuint);
  if((p = getProcAddress("glCompileShader")))
    p(shader);
  else if((p = getProcAddress("glCompileShaderARB")))
    p(shader);
  else
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour compiler le shader\n",
	    __FILE__, __LINE__, __func__);
}

/*!\brief fait appel a glCreateProgram ou glCreateProgramObjectARB
 * selon la disponibilité
 *
 * \return l'identifiant du Program créé
 */
GLuint gl4dCreateProgram(void) {
  GLuint (__stdcall *p)(void);
  if((p = getProcAddress("glCreateProgram")))
    return p();
  else if((p = getProcAddress("glCreateProgramObjectARB")))
    return p();
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour creer un Program\n",
	    __FILE__, __LINE__, __func__);
    return 0;
  }
}

/*!\brief fait appel a glAttachShader ou glAttachObjectARB selon la
 * disponibilité
 */
void gl4dAttachShader(GLuint program, GLuint shader) {
  void (__stdcall *p)(GLuint, GLuint);
  if((p = getProcAddress("glAttachShader")))
    p(program, shader);
  else if((p = getProcAddress("glAttachObjectARB")))
    p((GLhandleARB)program, (GLhandleARB)shader);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour attacher le shader\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glLinkProgram ou glLinkProgramARB selon la
 * disponibilité
 */
void gl4dLinkProgram(GLuint program) {
  void (__stdcall *p)(GLuint);
  if((p = getProcAddress("glLinkProgram")))
    p(program);
  else if((p = getProcAddress("glLinkProgramARB")))
    p((GLhandleARB)program);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour lier (linker) le Program\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glUseProgram ou glUseProgramObjectARB selon la
 * disponibilité
 */
void gl4dUseProgram(GLuint program) {
  void (__stdcall *p)(GLuint);
  if((p = getProcAddress("glUseProgram")))
    p(program);
  else if((p = getProcAddress("glUseProgramObjectARB")))
    p((GLhandleARB)program);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour exécuter (use) le Program\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glGetShaderiv ou glGetObjectParameterivARB
 * selon la disponibilité
 */
void gl4dGetShaderiv(GLuint object, GLenum type, GLint *param) {
  void (__stdcall *p)(GLuint, GLenum, GLint *);
  if((p = getProcAddress("glGetShaderiv")))
    p(object, type, param);
  else if((p = getProcAddress("glGetObjectParameterivARB")))
    p((GLhandleARB)object, type, param);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour tester la compilation du Shader\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glGetProgramiv ou glGetObjectParameterivARB
 * selon la disponibilité
 */
void gl4dGetProgramiv(GLuint object, GLenum type, GLint *param) {
  void (__stdcall *p)(GLuint, GLenum, GLint *);
  if((p = getProcAddress("glGetProgramiv")))
    p(object, type, param);
  else if((p = getProcAddress("glGetObjectParameterivARB")))
    p((GLhandleARB)object, type, param);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour tester le link du Program\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glGetShaderInfoLog ou glGetInfoLogARB selon la
 * disponibilité
 */
void gl4dGetShaderInfoLog(GLuint object, GLint maxLen, GLint * len, char * log) {
  void (__stdcall *p)(GLuint, GLint, GLint *, char *);
  if((p = getProcAddress("glGetShaderInfoLog")))
    p(object, maxLen, len, log);
  else if((p = getProcAddress("glGetInfoLogARB")))
    p((GLhandleARB)object, maxLen, len, log);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour les logs de compilation du Shader\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glGetProgramInfoLog ou glGetInfoLogARB selon la
 * disponibilité
 */
void gl4dGetProgramInfoLog(GLuint object, GLint maxLen, GLint * len, char * log) {
  void (__stdcall *p)(GLuint, GLint, GLint *, char *);
  if((p = getProcAddress("glGetProgramInfoLog")))
    p(object, maxLen, len, log);
  else if((p = getProcAddress("glGetInfoLogARB")))
    p((GLhandleARB)object, maxLen, len, log);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour les logs de link du Program\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glDetachShader ou glDetachObjectARB selon la
 * disponibilité
 */
void gl4dDetachShader(GLuint program, GLuint shader) {
  void (__stdcall *p)(GLuint, GLuint);
  if((p = getProcAddress("glDetachShader")))
    p(program, shader);
  else if((p = getProcAddress("glDetachObjectARB")))
    p((GLhandleARB)program, (GLhandleARB)shader);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour detacher le Shader\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glDeleteShader ou glDeleteObjectARB selon la
 * disponibilité
 */
void gl4dDeleteShader(GLuint object) {
  void (__stdcall *p)(GLuint);
  if((p = getProcAddress("glDeleteShader")))
    p(object);
  else if((p = getProcAddress("glDeleteObjectARB")))
    p((GLhandleARB)object);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour supprimer le Shader\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glDeleteProgram ou glDeleteObjectARB selon la
 * disponibilité
 */
void gl4dDeleteProgram(GLuint object) {
  void (__stdcall *p)(GLuint);
  if((p = getProcAddress("glDeleteProgram")))
    p(object);
  else if((p = getProcAddress("glDeleteObjectARB")))
    p((GLhandleARB)object);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour supprimer le Program\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glIsProgram ou glIsProgramARB selon la
 * disponibilité
 */
GLboolean gl4dIsProgram(GLuint program) {
  GLboolean (__stdcall *p)(GLuint);
  if((p = getProcAddress("glIsProgram")))
    return p(program);
  else if((p = getProcAddress("glIsProgramARB")))
    return p((GLhandleARB)program);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour is Program\n",
	    __FILE__, __LINE__, __func__);
    return GL_FALSE;
  }
}

/*!\brief fait appel a glIsShader ou glIsShaderARB selon la
 * disponibilité
 */
GLboolean gl4dIsShader(GLuint shader) {
  GLboolean (__stdcall *p)(GLuint);
  if((p = getProcAddress("glIsShader")))
    return p(shader);
  else if((p = getProcAddress("glIsShaderARB")))
    return p((GLhandleARB)shader);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour is Shader\n",
	    __FILE__, __LINE__, __func__);
    return GL_FALSE;
  }
}

/*!\brief fait appel a glValidateProgram ou glValidateProgramARB selon la
 * disponibilité
 */
void gl4dValidateProgram(GLuint program) {
  void (__stdcall *p)(GLuint);
  if((p = getProcAddress("glValidateProgram")))
    p(program);
  else if((p = getProcAddress("glValidateProgramARB")))
    p((GLhandleARB)program);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour valider Program\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glGetUniformLocation ou glGetUniformLocationARB
 * selon la disponibilité
 */
GLint gl4dGetUniformLocation(GLuint program, const char * name) {
  GLint (__stdcall *p)(GLuint, const char *);
  if((p = getProcAddress("glGetUniformLocation")))
    return p(program, name);
  else if((p = getProcAddress("glGetUniformLocationARB")))
    return p((GLhandleARB)program, name);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Uniform Location\n",
	    __FILE__, __LINE__, __func__);
    return -1;
  }
}

/*!\brief fait appel a glBindFragDataLocation ou
 * glBindFragDataLocationEXT selon la disponibilité
 */
void gl4dBindFragDataLocation(GLuint program, GLuint colorNumber, const GLchar * name) {
  void (__stdcall *p)(GLuint, GLuint, const GLchar *);
  if((p = getProcAddress("glBindFragDataLocation"))) {
    p(program, colorNumber, name);
  } else if((p = getProcAddress("glBindFragDataLocationEXT"))) {
    p(program, colorNumber, name);
  } else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Uniform Location\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glBlitFramebuffer ou
 * glBlitFramebufferEXT selon la disponibilité
 */
void gl4dBlitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter) {
  void (__stdcall *p)(GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum);
  if((p = getProcAddress("glBlitFramebuffer"))) {
    p(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
  } else if((p = getProcAddress("glBlitFramebufferEXT"))) {
    p(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
  } else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour BlitFramebuffer\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glGenFramebuffers ou
 * glGenFramebuffersEXT selon la disponibilité
 */
void gl4dGenFramebuffers(GLsizei n, GLuint * ids) {
  void (__stdcall *p)(GLsizei, GLuint *);
  if((p = getProcAddress("glGenFramebuffers")))
    p(n, ids);
  else if((p = getProcAddress("glGenFramebuffersEXT")))
    p(n, ids);
  else
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour GenFramebuffers\n",
	    __FILE__, __LINE__, __func__);
}

/*!\brief fait appel a glBindFramebuffer ou
 * glBindFramebufferEXT selon la disponibilité
 */
void gl4dBindFramebuffer(GLenum target, GLuint framebuffer) {
  void (__stdcall *p)(GLenum, GLuint);
  if((p = getProcAddress("glBindFramebuffer")))
    p(target, framebuffer);
  else if((p = getProcAddress("glBindFramebufferEXT")))
    p(target, framebuffer);
  else
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour BindFramebuffer\n",
	    __FILE__, __LINE__, __func__);
}

/*!\brief fait appel a glFramebufferTexture2D ou
 * glFramebufferTexture2DEXT selon la disponibilité
 */
void gl4dFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) {
  void (__stdcall *p)(GLenum, GLenum, GLenum, GLuint, GLint);
  if((p = getProcAddress("glFramebufferTexture2D")))
    p(target, attachment, textarget, texture, level);
  else if((p = getProcAddress("glFramebufferTexture2DEXT")))
    p(target, attachment, textarget, texture, level);
  else
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour FramebufferTexture2D\n",
	    __FILE__, __LINE__, __func__);
}

/*!\brief fait appel a glDrawBuffers ou
 * glDrawBuffersARB selon la disponibilité
 */
void gl4dDrawBuffers(GLsizei n, const GLenum * bufs) {
  void (__stdcall *p)(GLsizei, const GLenum *);
  if((p = getProcAddress("glDrawBuffers")))
    p(n, bufs);
  else if((p = getProcAddress("glDrawBuffersARB")))
    p(n, bufs);
  else
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour DrawBuffers\n",
	    __FILE__, __LINE__, __func__);
}

/*!\brief fait appel a glActiveTexture ou
 * glActiveTextureARB selon la disponibilité
 */
void gl4dActiveTexture(GLenum texture) {
  void (__stdcall *p)(GLenum);
  if((p = getProcAddress("glActiveTexture")))
    p(texture);
  else if((p = getProcAddress("glActiveTextureARB")))
    p(texture);
  else
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour ActiveTexture\n",
	    __FILE__, __LINE__, __func__);
}

/*!\brief fait appel a glBindBuffer ou
 * glBindBufferARB selon la disponibilité
 */
void gl4dBindBuffer(GLenum target, GLuint buffer) {
  void (__stdcall *p)(GLenum, GLuint);
  if((p = getProcAddress("glBindBuffer")))
    p(target, buffer);
  else if((p = getProcAddress("glBindBufferARB")))
    p(target, buffer);
  else
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour BindBuffer\n",
	    __FILE__, __LINE__, __func__);
}

/*!\brief fait appel a glBufferData ou
 * glBufferDataARB selon la disponibilité
 */
void gl4dBufferData(GLenum target, GLsizeiptr size, const GLvoid * data, GLenum usage) {
  void (__stdcall *p)(GLenum, GLsizeiptr, const GLvoid *, GLenum);
  if((p = getProcAddress("glBufferData")))
    p(target, size, data, usage);
  else if((p = getProcAddress("glBufferDataARB")))
    p(target, size, data, usage);
  else
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour BufferData\n",
	    __FILE__, __LINE__, __func__);
}

/*!\brief fait appel a glGenBuffers ou
 * glGenBuffersARB selon la disponibilité
 */
void gl4dGenBuffers(GLsizei n, GLuint * buffers) {
  void (__stdcall *p)(GLsizei, GLuint *);
  if((p = getProcAddress("glGenBuffers")))
    p(n, buffers);
  else if((p = getProcAddress("glGenBuffersARB")))
    p(n, buffers);
  else
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour GenBuffers\n",
	    __FILE__, __LINE__, __func__);
}

/*!\brief fait appel a glDeleteBuffers ou
 * glDeleteBuffersARB selon la disponibilité
 */
void gl4dDeleteBuffers(GLsizei n, const GLuint * buffers) {
  void (__stdcall *p)(GLsizei, const GLuint *);
  if((p = getProcAddress("glDeleteBuffers")))
    p(n, buffers);
  else if((p = getProcAddress("glDeleteBuffersARB")))
    p(n, buffers);
  else
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour DeleteBuffers\n",
	    __FILE__, __LINE__, __func__);
}

/*!\brief fait appel a glBlendFuncSeparate ou
 * glBlendFuncSeparateARB selon la disponibilité
 */
void gl4dBlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha) {
  void (__stdcall *p)(GLenum, GLenum, GLenum, GLenum);
  if((p = getProcAddress("glBlendFuncSeparate")))
    p(srcRGB, dstRGB, srcAlpha, dstAlpha);
  else if((p = getProcAddress("glBlendFuncSeparateARB")))
    p(srcRGB, dstRGB, srcAlpha, dstAlpha);
  else
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour BlendFuncSeparate\n",
	    __FILE__, __LINE__, __func__);
}

/*!\brief fait appel a glUniform1i ou glUniform1iARB selon la
 * disponibilité
 */
void gl4dUniform1i(GLint location, GLint v0) {
  void (__stdcall *p)(GLint, GLint);
  if((p = getProcAddress("glUniform1i")))
    p(location, v0);
  else if((p = getProcAddress("glUniform1iARB")))
    p(location, v0);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Uniform\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glUniform1f ou glUniform1fARB selon la
 * disponibilité
 */
void gl4dUniform1f(GLint location, GLfloat v0) {
  void (__stdcall *p)(GLint, GLfloat);
  if((p = getProcAddress("glUniform1f")))
    p(location, v0);
  else if((p = getProcAddress("glUniform1fARB")))
    p(location, v0);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Uniform\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glUniform2i ou glUniform2iARB selon la
 * disponibilité
 */
void gl4dUniform2i(GLint location, GLint v0, GLint v1) {
  void (__stdcall *p)(GLint, GLint, GLint);
  if((p = getProcAddress("glUniform2i")))
    p(location, v0, v1);
  else if((p = getProcAddress("glUniform2iARB")))
    p(location, v0, v1);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Uniform\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glUniform2f ou glUniform2fARB selon la
 * disponibilité
 */
void gl4dUniform2f(GLint location, GLfloat v0, GLfloat v1) {
  void (__stdcall *p)(GLint, GLfloat, GLfloat);
  if((p = getProcAddress("glUniform2f")))
    p(location, v0, v1);
  else if((p = getProcAddress("glUniform2fARB")))
    p(location, v0, v1);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Uniform\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glUniform3i ou glUniform3iARB selon la
 * disponibilité
 */
void gl4dUniform3i(GLint location, GLint v0, GLint v1, GLint v2) {
  void (__stdcall *p)(GLint, GLint, GLint, GLint);
  if((p = getProcAddress("glUniform3i")))
    p(location, v0, v1, v2);
  else if((p = getProcAddress("glUniform3iARB")))
    p(location, v0, v1, v2);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Uniform\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glUniform3f ou glUniform3fARB selon la
 * disponibilité
 */
void gl4dUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2) {
  void (__stdcall *p)(GLint, GLfloat, GLfloat, GLfloat);
  if((p = getProcAddress("glUniform3f")))
    p(location, v0, v1, v2);
  else if((p = getProcAddress("glUniform3fARB")))
    p(location, v0, v1, v2);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Uniform\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glUniform4i ou glUniform4iARB selon la
 * disponibilité
 */
void gl4dUniform4i(GLint location, GLint v0, GLint v1, GLint v2, GLint v3) {
  void (__stdcall *p)(GLint, GLint, GLint, GLint, GLint);
  if((p = getProcAddress("glUniform4i")))
    p(location, v0, v1, v2, v3);
  else if((p = getProcAddress("glUniform4iARB")))
    p(location, v0, v1, v2, v3);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Uniform\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glUniform4f ou glUniform4fARB selon la
 * disponibilité
 */
void gl4dUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) {
  void (__stdcall *p)(GLint, GLfloat, GLfloat, GLfloat, GLfloat);
  if((p = getProcAddress("glUniform4f")))
    p(location, v0, v1, v2, v3);
  else if((p = getProcAddress("glUniform4fARB")))
    p(location, v0, v1, v2, v3);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Uniform\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glUniform1iv ou glUniform1ivARB selon la
 * disponibilité
 */
void gl4dUniform1iv(GLint location, GLsizei count, const GLint * values) {
  void (__stdcall *p)(GLint, GLsizei, const GLint *);
  if((p = getProcAddress("glUniform1iv")))
    p(location, count, values);
  else if((p = getProcAddress("glUniform1ivARB")))
    p(location, count, values);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Uniform\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glUniform1fv ou glUniform1fvARB selon la
 * disponibilité
 */
void gl4dUniform1fv(GLint location, GLsizei count, const GLfloat * values) {
  void (__stdcall *p)(GLint, GLsizei, const GLfloat *);
  if((p = getProcAddress("glUniform1fv")))
    p(location, count, values);
  else if((p = getProcAddress("glUniform1fvARB")))
    p(location, count, values);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Uniform\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glUniform2iv ou glUniform2ivARB selon la
 * disponibilité
 */
void gl4dUniform2iv(GLint location, GLsizei count, const GLint * values) {
  void (__stdcall *p)(GLint, GLsizei, const GLint *);
  if((p = getProcAddress("glUniform2iv")))
    p(location, count, values);
  else if((p = getProcAddress("glUniform2ivARB")))
    p(location, count, values);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Uniform\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glUniform2fv ou glUniform2fvARB selon la
 * disponibilité
 */
void gl4dUniform2fv(GLint location, GLsizei count, const GLfloat * values) {
  void (__stdcall *p)(GLint, GLsizei, const GLfloat *);
  if((p = getProcAddress("glUniform2fv")))
    p(location, count, values);
  else if((p = getProcAddress("glUniform2fvARB")))
    p(location, count, values);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Uniform\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glUniform3iv ou glUniform3ivARB selon la
 * disponibilité
 */
void gl4dUniform3iv(GLint location, GLsizei count, const GLint * values) {
  void (__stdcall *p)(GLint, GLsizei, const GLint *);
  if((p = getProcAddress("glUniform3iv")))
    p(location, count, values);
  else if((p = getProcAddress("glUniform3ivARB")))
    p(location, count, values);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Uniform\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glUniform3fv ou glUniform3fvARB selon la
 * disponibilité
 */
void gl4dUniform3fv(GLint location, GLsizei count, const GLfloat * values) {
  void (__stdcall *p)(GLint, GLsizei, const GLfloat *);
  if((p = getProcAddress("glUniform3fv")))
    p(location, count, values);
  else if((p = getProcAddress("glUniform3fvARB")))
    p(location, count, values);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Uniform\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glUniform4iv ou glUniform4ivARB selon la
 * disponibilité
 */
void gl4dUniform4iv(GLint location, GLsizei count, const GLint * values) {
  void (__stdcall *p)(GLint, GLsizei, const GLint *);
  if((p = getProcAddress("glUniform4iv")))
    p(location, count, values);
  else if((p = getProcAddress("glUniform4ivARB")))
    p(location, count, values);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Uniform\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glUniform4fv ou glUniform4fvARB selon la
 * disponibilité
 */
void gl4dUniform4fv(GLint location, GLsizei count, const GLfloat * values) {
  void (__stdcall *p)(GLint, GLsizei, const GLfloat *);
  if((p = getProcAddress("glUniform4fv")))
    p(location, count, values);
  else if((p = getProcAddress("glUniform4fvARB")))
    p(location, count, values);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Uniform\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glUniformMatrix2fv ou glUniformMatrix2fvARB selon la
 * disponibilité
 */
void gl4dUniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat * values) {
  void (__stdcall *p)(GLint, GLsizei, GLboolean, const GLfloat *);
  if((p = getProcAddress("glUniformMatrix2fv")))
    p(location, count, transpose, values);
  else if((p = getProcAddress("glUniformMatrix2fvARB")))
    p(location, count, transpose, values);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour UniformMatrix\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glUniformMatrix3fv ou glUniformMatrix3fvARB selon la
 * disponibilité
 */
void gl4dUniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat * values) {
  void (__stdcall *p)(GLint, GLsizei, GLboolean, const GLfloat *);
  if((p = getProcAddress("glUniformMatrix3fv")))
    p(location, count, transpose, values);
  else if((p = getProcAddress("glUniformMatrix3fvARB")))
    p(location, count, transpose, values);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour UniformMatrix\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glUniformMatrix4fv ou glUniformMatrix4fvARB selon la
 * disponibilité
 */
void gl4dUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat * values) {
  void (__stdcall *p)(GLint, GLsizei, GLboolean, const GLfloat *);
  if((p = getProcAddress("glUniformMatrix4fv")))
    p(location, count, transpose, values);
  else if((p = getProcAddress("glUniformMatrix4fvARB")))
    p(location, count, transpose, values);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour UniformMatrix\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glGetAttribLocation ou glGetAttribLocationARB
 * selon la disponibilité
 */
GLint gl4dGetAttribLocation(GLuint program, const char * name) {
  GLint (__stdcall *p)(GLuint, const char *);
  if((p = getProcAddress("glGetAttribLocation")))
    return p(program, name);
  else if((p = getProcAddress("glGetAttribLocationARB")))
    return p((GLhandleARB)program, name);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Get Attrib Location\n",
	    __FILE__, __LINE__, __func__);
    return -1;
  }
}

/*!\brief fait appel a glBindAttribLocation ou glBindAttribLocationARB
 * selon la disponibilité
 */
void gl4dBindAttribLocation(GLuint program, GLuint index, const char * name) {
  void (__stdcall *p)(GLuint, GLuint, const char *);
  if((p = getProcAddress("glBindAttribLocation")))
    p(program, index, name);
  else if((p = getProcAddress("glBindAttribLocationARB")))
    p((GLhandleARB)program, index, name);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Bind Attrib Location\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glVertexAttrib1s ou glVertexAttrib1sARB selon la
 * disponibilité
 */
void gl4dVertexAttrib1s(GLuint index, GLshort v0) {
  void (__stdcall *p)(GLint, GLshort);
  if((p = getProcAddress("glVertexAttrib1s")))
    p(index, v0);
  else if((p = getProcAddress("glVertexAttrib1sARB")))
    p(index, v0);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Vertex Attrib\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glVertexAttrib1f ou glVertexAttrib1fARB selon la
 * disponibilité
 */
void gl4dVertexAttrib1f(GLuint index, GLfloat v0) {
  void (__stdcall *p)(GLint, GLfloat);
  if((p = getProcAddress("glVertexAttrib1f")))
    p(index, v0);
  else if((p = getProcAddress("glVertexAttrib1fARB")))
    p(index, v0);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Vertex Attrib\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glVertexAttrib1d ou glVertexAttrib1dARB selon la
 * disponibilité
 */
void gl4dVertexAttrib1d(GLuint index, GLdouble v0) {
  void (__stdcall *p)(GLint, GLdouble);
  if((p = getProcAddress("glVertexAttrib1d")))
    p(index, v0);
  else if((p = getProcAddress("glVertexAttrib1dARB")))
    p(index, v0);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Vertex Attrib\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glVertexAttrib2s ou glVertexAttrib2sARB selon la
 * disponibilité
 */
void gl4dVertexAttrib2s(GLuint index, GLshort v0, GLshort v1) {
  void (__stdcall *p)(GLint, GLshort, GLshort);
  if((p = getProcAddress("glVertexAttrib2s")))
    p(index, v0, v1);
  else if((p = getProcAddress("glVertexAttrib2sARB")))
    p(index, v0, v1);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Vertex Attrib\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glVertexAttrib2f ou glVertexAttrib2fARB selon la
 * disponibilité
 */
void gl4dVertexAttrib2f(GLuint index, GLfloat v0, GLfloat v1) {
  void (__stdcall *p)(GLint, GLfloat, GLfloat);
  if((p = getProcAddress("glVertexAttrib2f")))
    p(index, v0, v1);
  else if((p = getProcAddress("glVertexAttrib2fARB")))
    p(index, v0, v1);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Vertex Attrib\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glVertexAttrib2d ou glVertexAttrib2dARB selon la
 * disponibilité
 */
void gl4dVertexAttrib2d(GLuint index, GLdouble v0, GLdouble v1) {
  void (__stdcall *p)(GLint, GLdouble, GLdouble);
  if((p = getProcAddress("glVertexAttrib2d")))
    p(index, v0, v1);
  else if((p = getProcAddress("glVertexAttrib2dARB")))
    p(index, v0, v1);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Vertex Attrib\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glVertexAttrib3s ou glVertexAttrib3sARB selon la
 * disponibilité
 */
void gl4dVertexAttrib3s(GLuint index, GLshort v0, GLshort v1, GLshort v2) {
  void (__stdcall *p)(GLint, GLshort, GLshort, GLshort);
  if((p = getProcAddress("glVertexAttrib3s")))
    p(index, v0, v1, v2);
  else if((p = getProcAddress("glVertexAttrib3sARB")))
    p(index, v0, v1, v2);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Vertex Attrib\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glVertexAttrib3f ou glVertexAttrib3fARB selon la
 * disponibilité
 */
void gl4dVertexAttrib3f(GLuint index, GLfloat v0, GLfloat v1, GLfloat v2) {
  void (__stdcall *p)(GLint, GLfloat, GLfloat, GLfloat);
  if((p = getProcAddress("glVertexAttrib3f")))
    p(index, v0, v1, v2);
  else if((p = getProcAddress("glVertexAttrib3fARB")))
    p(index, v0, v1, v2);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Vertex Attrib\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glVertexAttrib3d ou glVertexAttrib3dARB selon la
 * disponibilité
 */
void gl4dVertexAttrib3d(GLuint index, GLdouble v0, GLdouble v1, GLdouble v2) {
  void (__stdcall *p)(GLint, GLdouble, GLdouble, GLdouble);
  if((p = getProcAddress("glVertexAttrib3d")))
    p(index, v0, v1, v2);
  else if((p = getProcAddress("glVertexAttrib3dARB")))
    p(index, v0, v1, v2);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Vertex Attrib\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glVertexAttrib4s ou glVertexAttrib4sARB selon la
 * disponibilité
 */
void gl4dVertexAttrib4s(GLuint index, GLshort v0, GLshort v1, GLshort v2, GLshort v3) {
  void (__stdcall *p)(GLint, GLshort, GLshort, GLshort, GLshort);
  if((p = getProcAddress("glVertexAttrib4s")))
    p(index, v0, v1, v2, v3);
  else if((p = getProcAddress("glVertexAttrib4sARB")))
    p(index, v0, v1, v2, v3);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Vertex Attrib\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glVertexAttrib4f ou glVertexAttrib4fARB selon la
 * disponibilité
 */
void gl4dVertexAttrib4f(GLuint index, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) {
  void (__stdcall *p)(GLint, GLfloat, GLfloat, GLfloat, GLfloat);
  if((p = getProcAddress("glVertexAttrib4f")))
    p(index, v0, v1, v2, v3);
  else if((p = getProcAddress("glVertexAttrib4fARB")))
    p(index, v0, v1, v2, v3);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Vertex Attrib\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glVertexAttrib4d ou glVertexAttrib4dARB selon la
 * disponibilité
 */
void gl4dVertexAttrib4d(GLuint index, GLdouble v0, GLdouble v1, GLdouble v2, GLdouble v3) {
  void (__stdcall *p)(GLint, GLdouble, GLdouble, GLdouble, GLdouble);
  if((p = getProcAddress("glVertexAttrib4d")))
    p(index, v0, v1, v2, v3);
  else if((p = getProcAddress("glVertexAttrib4dARB")))
    p(index, v0, v1, v2, v3);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Vertex Attrib\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glVertexAttrib4Nub ou glVertexAttrib4NubARB selon la
 * disponibilité
 */
void gl4dVertexAttrib4Nub(GLuint index, GLubyte v0, GLubyte v1, GLubyte v2, GLubyte v3) {
  void (__stdcall *p)(GLint, GLubyte, GLubyte, GLubyte, GLubyte);
  if((p = getProcAddress("glVertexAttrib4Nub")))
    p(index, v0, v1, v2, v3);
  else if((p = getProcAddress("glVertexAttrib4NubARB")))
    p(index, v0, v1, v2, v3);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Vertex Attrib\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glVertexAttrib1sv ou glVertexAttrib1svARB selon la
 * disponibilité
 */
void gl4dVertexAttrib1sv(GLuint index, const GLshort * values) {
  void (__stdcall *p)(GLint, const GLshort *);
  if((p = getProcAddress("glVertexAttrib1sv")))
    p(index, values);
  else if((p = getProcAddress("glVertexAttrib1svARB")))
    p(index, values);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Vertex Attrib\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glVertexAttrib1fv ou glVertexAttrib1fvARB selon la
 * disponibilité
 */
void gl4dVertexAttrib1fv(GLuint index, const GLfloat * values) {
  void (__stdcall *p)(GLint, const GLfloat *);
  if((p = getProcAddress("glVertexAttrib1fv")))
    p(index, values);
  else if((p = getProcAddress("glVertexAttrib1fvARB")))
    p(index, values);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Vertex Attrib\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glVertexAttrib1dv ou glVertexAttrib1dvARB selon la
 * disponibilité
 */
void gl4dVertexAttrib1dv(GLuint index, const GLdouble * values) {
  void (__stdcall *p)(GLint, const GLdouble *);
  if((p = getProcAddress("glVertexAttrib1dv")))
    p(index, values);
  else if((p = getProcAddress("glVertexAttrib1dvARB")))
    p(index, values);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Vertex Attrib\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glVertexAttrib2sv ou glVertexAttrib2svARB selon la
 * disponibilité
 */
void gl4dVertexAttrib2sv(GLuint index, const GLshort * values) {
  void (__stdcall *p)(GLint, const GLshort *);
  if((p = getProcAddress("glVertexAttrib2sv")))
    p(index, values);
  else if((p = getProcAddress("glVertexAttrib2svARB")))
    p(index, values);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Vertex Attrib\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glVertexAttrib2fv ou glVertexAttrib2fvARB selon la
 * disponibilité
 */
void gl4dVertexAttrib2fv(GLuint index, const GLfloat * values) {
  void (__stdcall *p)(GLint, const GLfloat *);
  if((p = getProcAddress("glVertexAttrib2fv")))
    p(index, values);
  else if((p = getProcAddress("glVertexAttrib2fvARB")))
    p(index, values);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Vertex Attrib\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glVertexAttrib2dv ou glVertexAttrib2dvARB selon la
 * disponibilité
 */
void gl4dVertexAttrib2dv(GLuint index, const GLdouble * values) {
  void (__stdcall *p)(GLint, const GLdouble *);
  if((p = getProcAddress("glVertexAttrib2dv")))
    p(index, values);
  else if((p = getProcAddress("glVertexAttrib2dvARB")))
    p(index, values);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Vertex Attrib\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glVertexAttrib3sv ou glVertexAttrib3svARB selon la
 * disponibilité
 */
void gl4dVertexAttrib3sv(GLuint index, const GLshort * values) {
  void (__stdcall *p)(GLint, const GLshort *);
  if((p = getProcAddress("glVertexAttrib3sv")))
    p(index, values);
  else if((p = getProcAddress("glVertexAttrib3svARB")))
    p(index, values);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Vertex Attrib\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glVertexAttrib3fv ou glVertexAttrib3fvARB selon la
 * disponibilité
 */
void gl4dVertexAttrib3fv(GLuint index, const GLfloat * values) {
  void (__stdcall *p)(GLint, const GLfloat *);
  if((p = getProcAddress("glVertexAttrib3fv")))
    p(index, values);
  else if((p = getProcAddress("glVertexAttrib3fvARB")))
    p(index, values);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Vertex Attrib\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glVertexAttrib3dv ou glVertexAttrib3dvARB selon la
 * disponibilité
 */
void gl4dVertexAttrib3dv(GLuint index, const GLdouble * values) {
  void (__stdcall *p)(GLint, const GLdouble *);
  if((p = getProcAddress("glVertexAttrib3dv")))
    p(index, values);
  else if((p = getProcAddress("glVertexAttrib3dvARB")))
    p(index, values);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Vertex Attrib\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glVertexAttrib4bv ou glVertexAttrib4bvARB selon la
 * disponibilité
 */
void gl4dVertexAttrib4bv(GLuint index, const GLbyte * values) {
  void (__stdcall *p)(GLint, const GLbyte *);
  if((p = getProcAddress("glVertexAttrib4bv")))
    p(index, values);
  else if((p = getProcAddress("glVertexAttrib4bvARB")))
    p(index, values);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Vertex Attrib\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glVertexAttrib4sv ou glVertexAttrib4svARB selon la
 * disponibilité
 */
void gl4dVertexAttrib4sv(GLuint index, const GLshort * values) {
  void (__stdcall *p)(GLint, const GLshort *);
  if((p = getProcAddress("glVertexAttrib4sv")))
    p(index, values);
  else if((p = getProcAddress("glVertexAttrib4svARB")))
    p(index, values);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Vertex Attrib\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glVertexAttrib4iv ou glVertexAttrib4ivARB selon la
 * disponibilité
 */
void gl4dVertexAttrib4iv(GLuint index, const GLint * values) {
  void (__stdcall *p)(GLint, const GLint *);
  if((p = getProcAddress("glVertexAttrib4iv")))
    p(index, values);
  else if((p = getProcAddress("glVertexAttrib4ivARB")))
    p(index, values);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Vertex Attrib\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glVertexAttrib4fv ou glVertexAttrib4fvARB selon la
 * disponibilité
 */
void gl4dVertexAttrib4fv(GLuint index, const GLfloat * values) {
  void (__stdcall *p)(GLint, const GLfloat *);
  if((p = getProcAddress("glVertexAttrib4fv")))
    p(index, values);
  else if((p = getProcAddress("glVertexAttrib4fvARB")))
    p(index, values);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Vertex Attrib\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glVertexAttrib4dv ou glVertexAttrib4dvARB selon la
 * disponibilité
 */
void gl4dVertexAttrib4dv(GLuint index, const GLdouble * values) {
  void (__stdcall *p)(GLint, const GLdouble *);
  if((p = getProcAddress("glVertexAttrib4dv")))
    p(index, values);
  else if((p = getProcAddress("glVertexAttrib4dvARB")))
    p(index, values);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Vertex Attrib\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glVertexAttrib4ubv ou glVertexAttrib4ubvARB selon la
 * disponibilité
 */
void gl4dVertexAttrib4ubv(GLuint index, const GLubyte * values) {
  void (__stdcall *p)(GLint, const GLubyte *);
  if((p = getProcAddress("glVertexAttrib4ubv")))
    p(index, values);
  else if((p = getProcAddress("glVertexAttrib4ubvARB")))
    p(index, values);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Vertex Attrib\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glVertexAttrib4usv ou glVertexAttrib4usvARB selon la
 * disponibilité
 */
void gl4dVertexAttrib4usv(GLuint index, const GLushort * values) {
  void (__stdcall *p)(GLint, const GLushort *);
  if((p = getProcAddress("glVertexAttrib4usv")))
    p(index, values);
  else if((p = getProcAddress("glVertexAttrib4usvARB")))
    p(index, values);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Vertex Attrib\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glVertexAttrib4uiv ou glVertexAttrib4uivARB selon la
 * disponibilité
 */
void gl4dVertexAttrib4uiv(GLuint index, const GLuint * values) {
  void (__stdcall *p)(GLint, const GLuint *);
  if((p = getProcAddress("glVertexAttrib4uiv")))
    p(index, values);
  else if((p = getProcAddress("glVertexAttrib4uivARB")))
    p(index, values);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Vertex Attrib\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glVertexAttrib4Nbv ou glVertexAttrib4NbvARB selon la
 * disponibilité
 */
void gl4dVertexAttrib4Nbv(GLuint index, const GLbyte * values) {
  void (__stdcall *p)(GLint, const GLbyte *);
  if((p = getProcAddress("glVertexAttrib4Nbv")))
    p(index, values);
  else if((p = getProcAddress("glVertexAttrib4NbvARB")))
    p(index, values);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Vertex Attrib\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glVertexAttrib4Nsv ou glVertexAttrib4NsvARB selon la
 * disponibilité
 */
void gl4dVertexAttrib4Nsv(GLuint index, const GLshort * values) {
  void (__stdcall *p)(GLint, const GLshort *);
  if((p = getProcAddress("glVertexAttrib4Nsv")))
    p(index, values);
  else if((p = getProcAddress("glVertexAttrib4NsvARB")))
    p(index, values);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Vertex Attrib\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glVertexAttrib4Niv ou glVertexAttrib4NivARB selon la
 * disponibilité
 */
void gl4dVertexAttrib4Niv(GLuint index, const GLint * values) {
  void (__stdcall *p)(GLint, const GLint *);
  if((p = getProcAddress("glVertexAttrib4Niv")))
    p(index, values);
  else if((p = getProcAddress("glVertexAttrib4NivARB")))
    p(index, values);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Vertex Attrib\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glVertexAttrib4Nubv ou glVertexAttrib4NubvARB selon la
 * disponibilité
 */
void gl4dVertexAttrib4Nubv(GLuint index, const GLubyte * values) {
  void (__stdcall *p)(GLint, const GLubyte *);
  if((p = getProcAddress("glVertexAttrib4Nubv")))
    p(index, values);
  else if((p = getProcAddress("glVertexAttrib4NubvARB")))
    p(index, values);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Vertex Attrib\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glVertexAttrib4Nusv ou glVertexAttrib4NusvARB selon la
 * disponibilité
 */
void gl4dVertexAttrib4Nusv(GLuint index, const GLushort * values) {
  void (__stdcall *p)(GLint, const GLushort *);
  if((p = getProcAddress("glVertexAttrib4Nusv")))
    p(index, values);
  else if((p = getProcAddress("glVertexAttrib4NusvARB")))
    p(index, values);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Vertex Attrib\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glVertexAttrib4Nuiv ou glVertexAttrib4NuivARB selon la
 * disponibilité
 */
void gl4dVertexAttrib4Nuiv(GLuint index, const GLuint * values) {
  void (__stdcall *p)(GLint, const GLuint *);
  if((p = getProcAddress("glVertexAttrib4Nuiv")))
    p(index, values);
  else if((p = getProcAddress("glVertexAttrib4NuivARB")))
    p(index, values);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Vertex Attrib\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glVertexAttribPointer ou
 * glVertexAttribPointerARB selon la disponibilité
 */
void gl4dVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer) {
  void (__stdcall *p)(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid *);
  if((p = getProcAddress("glVertexAttribPointer")))
    p(index, size, type, normalized, stride, pointer);
  else if((p = getProcAddress("glVertexAttribPointerARB")))
    p(index, size, type, normalized, stride, pointer);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Vertex Attrib Pointer\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glEnableVertexAttribArray ou
 * glEnableVertexAttribArrayARB selon la disponibilité
 */
void gl4dEnableVertexAttribArray(GLuint index) {
  void (__stdcall *p)(GLint);
  if((p = getProcAddress("glEnableVertexAttribArray")))
    p(index);
  else if((p = getProcAddress("glEnableVertexAttribArrayARB")))
    p(index);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Enable Vertex Attrib Array\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glDisableVertexAttribArray ou
 * glDisableVertexAttribArrayARB selon la disponibilité
 */
void gl4dDisableVertexAttribArray(GLuint index) {
  void (__stdcall *p)(GLint);
  if((p = getProcAddress("glDisableVertexAttribArray")))
    p(index);
  else if((p = getProcAddress("glDisableVertexAttribArrayARB")))
    p(index);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Disable Vertex Attrib Array\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glDeleteVertexArrays si disponible
 */
void gl4dDeleteVertexArrays(GLsizei n, const GLuint * arrays) {
  void (__stdcall *p)(GLsizei, const GLuint *);
  if((p = getProcAddress("glDeleteVertexArrays")))
    p(n, arrays);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Delete Vertex Arrays\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glGenVertexArrays si disponible
 */
void gl4dGenVertexArrays(GLsizei n, GLuint * arrays) {
  void (__stdcall *p)(GLsizei, const GLuint *);
  if((p = getProcAddress("glGenVertexArrays")))
    p(n, arrays);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Gen Vertex Arrays\n",
	    __FILE__, __LINE__, __func__);
  }
}

/*!\brief fait appel a glBindVertexArray si disponible
 */
void gl4dBindVertexArray(GLuint array) {
  void (__stdcall *p)(GLuint);
  if((p = getProcAddress("glBindVertexArray")))
    p(array);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Bind Vertex Array\n",
	    __FILE__, __LINE__, __func__);
  }
}
/*!\brief fait appel a glGetFramebufferAttachmentParameteriv si disponible
 */
void glGetFramebufferAttachmentParameteriv(GLenum target,  GLenum attachment,  GLenum pname,  GLint * params) {
  void (__stdcall *p)(GLenum, GLenum, GLenum, GLint *);
  if((p = getProcAddress("glGetFramebufferAttachmentParameteriv")))
    p(target, attachment, pname, params);
  else {
    fprintf(stderr, "%s:%d:In %s: Aucune procedure pour Get Framebuffer Attachment Parameter\n",
	    __FILE__, __LINE__, __func__);
  }
}
#endif
