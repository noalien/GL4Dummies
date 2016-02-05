/*!\file gl4duw_SDL2.c
 *
 * \brief Fonctions haut-niveau de GL4Dummies pour la gestion des fenêtres avec SDL2
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date Februry 01 2016
 */
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "bin_tree.h"
#include "gl4duw_SDL2.h"


/*!\brief type lié à la structure de données struct window_t. */
typedef struct window_t window_t;

/*!\brief structure de données englobant le nom de la fenêtre, le
 *  pointeur vers sa structure SDL, son contexte OpenGL et les
 *  pointeurs vers ses fonctions callback. */
struct window_t {
  char * name;
  SDL_Window * window;
  SDL_GLContext glContext;
  void (*resize)(int w, int h);
  void (*keydown)(int keycode);
  void (*keyup)(int keycode);
  void (*idle)(void);
  void (*display)(void);
};

/*!\brief arbre binaire contenant l'ensemble des fenêtres créées. */
static bin_tree_t  * _btWindows = NULL;
/*!\brief adresse du pointeur vers le noeud référençant la dernière fenêtre créée. */
static bin_tree_t  ** _lastWindown = NULL;
/*!\brief la fenêtre \ref window_t courante (coté code, \see gl4duwBindWindow). */
static window_t * _curWindow = NULL;
/*!\brief la fenêtre \ref window_t ayant le focus (\see manageEvents). */
static window_t * _focusedWindow = NULL;
/*!\brief les paramètres par défaut du contexte OpenGL */
static int _glMajorVersion = 3, _glMinorVersion = 2, _glProfileMask = SDL_GL_CONTEXT_PROFILE_CORE, _glDoubleBuffer = 1, _glDepthSize = 16;
/*!\brief flag permettant de savoir si la lib a déjà été initialisée */
static int _hasInit = 0;

static inline int        initGL4DUW(int argc, char ** argv);
static inline window_t * newWindow(const char * name, SDL_Window * win, SDL_GLContext oglc);
static inline int        windowCmpFunc(const void * w1, const void * w2);
static inline void       freeWindows(void * window);
static inline void       quit(void);
static inline void       resize(window_t * win);
static inline void       manageEvents(void);
static inline void       mainLoopBody(void * window, void ** data);

/*!\brief fonction fictive liée au callback de resize de la fenêtre. */
static inline void fake_resize(int w, int h) {}
/*!\brief fonction fictive liée au callback de touche clavier enfoncée. */
static inline void fake_keydown(int keycode) {}
/*!\brief fonction fictive liée au callback de touche clavier relachée. */
static inline void fake_keyup(int keycode) {}
/*!\brief fonction fictive liée au callback de l'état idle de la fenêtre. */
static inline void fake_idle(void) {}
/*!\brief fonction fictive liée au callback de display. */
static inline void fake_display(void) {}

/*!\brief modifie les paramètre par défaut du contexte OpenGL.
 *
 * Cette fonction est à appeler avant \ref gl4duwCreateWindow si vous
 * souhaitez que ses paramètres soient pris en compte. Si vous
 * souhaitez modifier plus de paramètres voir la fonction
 * SDL_GL_SetAttribute.
 *
 * \param glMajorVersion version majeure d'OpenGL, par défaut vaut 3.
 * \param glMinorVersion version mineure d'OpenGL, par défaut vaut 2.
 * \param glProfileMask modifie le profile d'OpenGL, par défaut vaut
 * SDL_GL_CONTEXT_PROFILE_CORE mais peut aussi prendre
 * SDL_GL_CONTEXT_PROFILE_COMPATIBILITY, SDL_GL_CONTEXT_PROFILE_ES.
 * \param glDoubleBuffer modifie l'état actif ou non du double buffer,
 * par défaut vaut 1 (vrai).
 * \param glDepthSize modifie la dimension (nombre de bits utilisés)
 * du buffer de profondeur, par défaut vaut 16.
 * \see gl4duwCreateWindow
 */
void gl4duwSetGLAttributes(int glMajorVersion, int glMinorVersion, int glProfileMask, int glDoubleBuffer, int glDepthSize) {
  _glMajorVersion = glMajorVersion;
  _glMinorVersion = glMinorVersion;
  _glProfileMask  = glProfileMask;
  _glDoubleBuffer = glDoubleBuffer;
  _glDepthSize    = glDepthSize;
}

/*!\brief créé une fenêtre SDL avec un contexte OpenGL.
 *
 * Pour modifier les valeurs utilisée par le contexte OpenGL, utiliser
 * la fonction \ref gl4duwSetGLAttributes.
 *
 * \param argc nombre d'arguments passés au programme (premier argument de la fonction main).
 * \param argv liste des arguments passés au programme (second argument de la fonction main).
 * \param title titre de la fenêtre SDL à créer.
 * \param x la postion x de la fenêtre à créer ou SDL_WINDOWPOS_CENTERED, ou SDL_WINDOWPOS_UNDEFINED.
 * \param y la postion y de la fenêtre à créer ou SDL_WINDOWPOS_CENTERED, ou SDL_WINDOWPOS_UNDEFINED.
 * \param width la largeur de la fenêtre à créer.
 * \param height la hauteur de la fenêtre à créer.
 * \param wflags des options sur la fenêtre à créer. Peut être 0 ou
 * une ou toute combinaison (via OR "|") de : SDL_WINDOW_FULLSCREEN,
 * SDL_WINDOW_FULLSCREEN_DESKTOP, SDL_WINDOW_OPENGL,
 * SDL_WINDOW_HIDDEN, SDL_WINDOW_BORDERLESS, SDL_WINDOW_RESIZABLE,
 * SDL_WINDOW_MINIMIZED, SDL_WINDOW_MAXIMIZED,
 * SDL_WINDOW_INPUT_GRABBED, SDL_WINDOW_ALLOW_HIGHDPI.
 * \return GL_TRUE en cas de réussite, GL_FALSE en cas d'échec.
 * \see gl4duwSetGLAttributes
 * \see SDL_CreateWindow
 */
GLboolean gl4duwCreateWindow(int argc, char ** argv, const char * title, int x, int y, 
		       int width, int height, Uint32 wflags) {
  SDL_Window * win = NULL;
  SDL_GLContext oglc = NULL;
  window_t wt = { (char *)title, NULL };
  pair_t pair;
  static int ft = 1;
  pair = btFind(&_btWindows, &wt, windowCmpFunc);
  if(!pair.compResult) {
    fprintf(stderr, "%s (%d): %s:\n\tErreur lors de la creation de la fenetre SDL : une fenetre portant le meme nom existe deja\n", 
	    __FILE__, __LINE__, __func__);    
    return GL_FALSE;
  }
  initGL4DUW(argc, argv);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, _glMajorVersion);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, _glMinorVersion);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, _glProfileMask);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, _glDoubleBuffer);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, _glDepthSize);
  if( (win = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
			      width, height, SDL_WINDOW_OPENGL | wflags)) == NULL ) {
    fprintf(stderr, "%s (%d): %s:\n\tErreur lors de la creation de la fenetre SDL : %s", 
	    __FILE__, __LINE__, __func__, SDL_GetError());
    return GL_FALSE;
  }
  if( (oglc = SDL_GL_CreateContext(win)) == NULL ) {
    SDL_DestroyWindow(win);
    fprintf(stderr, "%s (%d): %s:\n\tErreur lors de la creation du contexte OpenGL : %s", 
	    __FILE__, __LINE__, __func__, SDL_GetError());
    return GL_FALSE;
  }
  _curWindow = newWindow(title, win, oglc);
  _lastWindown = (bin_tree_t **)(btInsert((bin_tree_t **)(pair.ptr), _curWindow, windowCmpFunc).ptr);
  if(ft) {
    fprintf(stderr, "OpenGL version: %s\n", glGetString(GL_VERSION));
    fprintf(stderr, "Supported shaders version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    ft = 0;
  }
  return GL_TRUE;
}

/*!\brief recherche et positionne "courant" une fenêtre en fonction de son titre.
 *
 * La fenêtre courante est celle qui est affectée par les appelles des
 * fonctions telles que \ref gl4duwResizeFunc, \ref gl4duwKeyDownFunc,
 * \ref gl4duwKeyUpFunc, \ref gl4duwIdleFunc, \ref gl4duwDisplayFunc.
 *
 * \param title titre de la fenêtre SDL recherchée.
 * \return GL_TRUE en cas de réussite, GL_FALSE en cas d'échec.
 */
GLboolean gl4duwBindWindow(const char * title) {
  window_t wt = { (char *)title, NULL };
  pair_t pair;
  pair = btFind(&_btWindows, &wt, windowCmpFunc);
  if(pair.compResult)
    return GL_FALSE;
  _curWindow = (window_t *)((*(bin_tree_t **)pair.ptr)->data);
  return GL_TRUE;
}

/*!\brief boucle principale événement/simulation/affichage */
void gl4duwMainLoop(void) {
  for(;;) {
    manageEvents();
    btForAll(_btWindows, mainLoopBody, NULL);
    gl4duPrintFPS(stderr);
    gl4duUpdateShaders();
  }
}

/*!\brief affecte la fonction appelée lors du resize */
void gl4duwResizeFunc(void (*func)(int, int)) {
  _curWindow->resize = func ? func : fake_resize;
}

/*!\brief affecte la fonction appelée lors de l'événement key down */
void gl4duwKeyDownFunc(void (*func)(int)) {
  _curWindow->keydown = func ? func : fake_keydown;
}

/*!\brief affecte la fonction appelée lors de l'événement key up */
void gl4duwKeyUpFunc(void (*func)(int)) {
  _curWindow->keyup = func ? func : fake_keyup;
}

/*!\brief affecte la fonction appelée lors de l'idle (calcul/simulation avant affichage) */
void gl4duwIdleFunc(void (*func)(void)) {
  _curWindow->idle = func ? func : fake_idle;
}

/*!\brief affecte la fonction appelée lors de l'affichage */
void gl4duwDisplayFunc(void (*func)(void)) {
  _curWindow->display = func ? func : fake_display;
}

/*!\brief initialise SDL et GL4Dummies.
 *
 * \param argc nombre d'arguments passés au programme (premier argument de la fonction main).
 * \param argv liste des arguments passés au programme (second argument de la fonction main).
 * \return 0 en cas d'échec, !=0 sinon. 
 */
static inline int initGL4DUW(int argc, char ** argv) {
  if(_hasInit) return 1;
  if(SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "%s (%d): %s:\n\tErreur lors de l'initialisation de SDL :  %s", 
	    __FILE__, __LINE__, __func__, SDL_GetError());
    return 0;
  }
  atexit(SDL_Quit);
  atexit(quit);
  gl4duInit(argc, argv);
  _hasInit = 1;
  return 1;
}

/*!\brief créé la fenêtre de type window_t.
 *
 * \param name titre de la fenêtre
 * \param win le pointeur vers la structure de fenêtre SDL
 * \param oglc la référence vers le contexte OpenGL lié à la fenêtre
 * \return le pointeur vers la fenêtre de type window_t créée 
 */
static inline window_t * newWindow(const char * name, SDL_Window * win, SDL_GLContext oglc) {
  window_t * w = malloc(sizeof *w);
  assert(w);
  w->name = strdup(name);
  w->window = win;
  w->glContext = oglc;
  w->resize = fake_resize;
  w->keydown = fake_keydown;
  w->keyup = fake_keyup;
  w->idle = fake_idle;
  w->display = fake_display;
  return w;
}

/*!\brief fonction de comparaison de deux fenêtres \ref window_t en
 * fonction du nom pour insertion dans l'arbre binaire.
 *
 * Cette fonction utilise strcmp.
 *
 * \param w1 première fenêtre (de type window_t).
 *
 * \param w2 seconde fenêtre (de type window_t).
 *
 * \return entier négatif, nul ou positif selon que le nom de la
 * première soit respectivement inférieur, égal ou supérieur à la
 * seconde.
 */
static inline int windowCmpFunc(const void * w1, const void * w2) {
  return strcmp( ((window_t *)w1)->name, ((window_t *)w2)->name );
}

/*!\brief libère la fenêtre
 *
 * \param window est la fenêtre de type \ref window_t.
 */
static inline void freeWindows(void * window) {
  free(((window_t *)window)->name);
  SDL_GL_DeleteContext(((window_t *)window)->glContext);
  SDL_DestroyWindow(((window_t *)window)->window);
  free(window);
}

/*!\brief est appelée au moment de sortir du programme (atexit), elle
 *  libère la (les) fenêtre(s) SDL et le(s) contexte(s) OpenGL lié(s).
 */
static inline void quit(void) {
  btFree(&_btWindows, freeWindows);
}

/*!\brief est appelée au moment du redimensionnement de la fenêtre
 *  \a win, elle récupère la taille de la fenêtre à l'aide de
 *  SDL_GetWindowSize puis appelle la callback resize de la fenêtre.
 * \param win la fenêtre redimensionnée
 */
static inline void resize(window_t * win) {
  int w, h;
  SDL_GetWindowSize(win->window, &w, &h);
  win->resize(w, h);
}

/*!\brief dispatche les événement selon le type en utilisant les
 *  callbacks de la fenêtre ayant le focus.
 */
static inline void manageEvents(void) {
  SDL_Event event;
  while(SDL_PollEvent(&event))
    switch (event.type) {
    case SDL_KEYDOWN:
      if(_focusedWindow)
	_focusedWindow->keydown(event.key.keysym.sym);
      break;
    case SDL_KEYUP:
      if(_focusedWindow)
	_focusedWindow->keyup(event.key.keysym.sym);
      break;
    case SDL_WINDOWEVENT: {
      SDL_Window * focusedw = SDL_GetWindowFromID(event.window.windowID);
      window_t wt = { (char *)SDL_GetWindowTitle(focusedw), NULL };
      pair_t pair;
      pair = btFind(&_btWindows, &wt, windowCmpFunc);
      _focusedWindow = (window_t *)(*(bin_tree_t **)(pair.ptr))->data;
      switch (event.window.event) {
      case SDL_WINDOWEVENT_RESIZED:
	resize(_focusedWindow);
	break;
      case SDL_WINDOWEVENT_CLOSE:
	event.type = SDL_QUIT;
	SDL_PushEvent(&event);
	break;
      }
      break;
    }
    case SDL_QUIT:
      exit(0);
    }
}

/*!\brief corps de la boucle principale événement/simulation/affichage */
static inline void mainLoopBody(void * window, void ** data) {
  window_t * w = (window_t *)window;
  w->idle();
  w->display();
  SDL_GL_SwapWindow(w->window);
}
