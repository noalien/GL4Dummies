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
  void (*mousebutton)(int button, int state, int x, int y);
  void (*mousemotion)(int x, int y);
  void (*passivemousemotion)(int x, int y);
  void (*idle)(void);
  void (*display)(void);
  int  (*catchSDL_Event)(SDL_Event * event);
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
/*!\brief flag permettant de savoir si la lib gère les événements */
static int _hasManageEvents = 1;

static inline int        initGL4DUW(int argc, char ** argv);
static inline window_t * newWindow(const char * name, SDL_Window * win, SDL_GLContext oglc);
static inline int        windowCmpFunc(const void * w1, const void * w2);
static inline void       freeWindows(void * window);
static inline void       quit(void);
static inline void       resize(window_t * win);
static inline void       manageEvents(void);
static inline void       mainLoopBody(void * window, void ** data);

/*!\brief fonction fictive liée au callback de resize de la fenêtre. */
static inline void fake_resize(int w, int h) {
  (void)w; /* silenced warning */
  (void)h; /* silenced warning */
}
/*!\brief fonction fictive liée au callback de touche clavier enfoncée. */
static inline void fake_keydown(int keycode) {
  (void)keycode; /* silenced warning */
}
/*!\brief fonction fictive liée au callback de touche clavier relachée. */
static inline void fake_keyup(int keycode) {
  (void)keycode; /* silenced warning */
}
/*!\brief fonction fictive liée au callback d'un bouton de souris enfoncé ou relaché. */
static inline void fake_mousebutton(int button, int state, int x, int y) {
  (void)button; /* silenced warning */
  (void)state;  /* silenced warning */
  (void)x;      /* silenced warning */
  (void)y;      /* silenced warning */
}
/*!\brief fonction fictive liée au callback du mouvement de la souris avec bouton enfoncé. */
static inline void fake_mousemotion(int x, int y) {
  (void)x; /* silenced warning */
  (void)y; /* silenced warning */
}
/*!\brief fonction fictive liée au callback du mouvement de la souris sans bouton enfoncé. */
static inline void fake_passivemousemotion(int x, int y) {
  (void)x; /* silenced warning */
  (void)y; /* silenced warning */
}
/*!\brief fonction fictive liée au callback de l'état idle de la fenêtre. */
static inline void fake_idle(void) {}
/*!\brief fonction fictive liée au callback de display. */
static inline void fake_display(void) {}
/*!\brief fonction fictive liée au callback de catchSDL_Event. */
static inline int  fake_catchSDL_Event(SDL_Event * event) {
  (void)event; /* silenced warning */
  return 0;
}

void gl4duwSetGLAttributes(int glMajorVersion, int glMinorVersion, int glProfileMask, int glDoubleBuffer, int glDepthSize) {
  _glMajorVersion = glMajorVersion;
  _glMinorVersion = glMinorVersion;
  _glProfileMask  = glProfileMask;
  _glDoubleBuffer = glDoubleBuffer;
  _glDepthSize    = glDepthSize;
}

GLboolean gl4duwCreateWindow(int argc, char ** argv, const char * title, int x, int y,
		       int width, int height, Uint32 wflags) {
  (void)x; /* silenced warning */
  (void)y; /* silenced warning */
  SDL_Window * win = NULL;
  SDL_GLContext oglc = NULL;
  window_t wt = {(char *)title, NULL, 0,    NULL, NULL, NULL,
                 NULL,          NULL, NULL, NULL, NULL, NULL};
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
  if( (win = SDL_CreateWindow(title, GL4DW_POS_CENTERED, GL4DW_POS_CENTERED,
			      width, height, GL4DW_OPENGL | wflags)) == NULL ) {
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

SDL_Window * gl4duwGetSDL_Window(void) {
  return _curWindow ? _curWindow->window : NULL;
}

GLboolean gl4duwBindWindow(const char * title) {
  window_t wt = {(char *)title, NULL, 0,    NULL, NULL, NULL,
                 NULL,          NULL, NULL, NULL, NULL, NULL};
  pair_t pair;
  pair = btFind(&_btWindows, &wt, windowCmpFunc);
  if(pair.compResult)
    return GL_FALSE;
  _curWindow = (window_t *)((*(bin_tree_t **)pair.ptr)->data);
  SDL_GL_MakeCurrent(_curWindow->window, _curWindow->glContext);
  return GL_TRUE;
}

void gl4duwMainLoop(void) {
  for(;;) {
    if(_hasManageEvents)
      manageEvents();
    btForAll(_btWindows, mainLoopBody, NULL);
    SDL_GL_MakeCurrent(_curWindow->window, _curWindow->glContext);
    gl4duPrintFPS(stderr);
    gl4duUpdateShaders();
  }
}

void gl4duwResizeFunc(void (*func)(int width, int height)) {
  _curWindow->resize = func ? func : fake_resize;
}

void gl4duwKeyDownFunc(void (*func)(int keycode)) {
  _curWindow->keydown = func ? func : fake_keydown;
}

void gl4duwKeyUpFunc(void (*func)(int keycode)) {
  _curWindow->keyup = func ? func : fake_keyup;
}

void gl4duwMouseFunc(void (*func)(int button, int state, int x, int y)) {
  _curWindow->mousebutton = func ? func : fake_mousebutton;
}

void gl4duwMotionFunc(void (*func)(int x, int y)) {
  _curWindow->mousemotion = func ? func : fake_mousemotion;
}

void gl4duwPassiveMotionFunc(void (*func)(int x, int y)) {
  _curWindow->passivemousemotion = func ? func : fake_passivemousemotion;
}

void gl4duwIdleFunc(void (*func)(void)) {
  _curWindow->idle = func ? func : fake_idle;
}

void gl4duwDisplayFunc(void (*func)(void)) {
  _curWindow->display = func ? func : fake_display;
}

void gl4duwCatchSDL_EventFunc(int (*func)(SDL_Event *)) {
  _curWindow->catchSDL_Event = func ? func : fake_catchSDL_Event;
}

void gl4duwEnableManageEvents(void) {
  _hasManageEvents = 1;
}

void gl4duwDisableManageEvents(void) {
  _hasManageEvents = 0;
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
  w->mousebutton = fake_mousebutton;
  w->mousemotion = fake_mousemotion;
  w->passivemousemotion = fake_passivemousemotion;
  w->idle = fake_idle;
  w->display = fake_display;
  w->catchSDL_Event = fake_catchSDL_Event;
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

void gl4duwGetWindowSize(int * w, int * h) {
  SDL_GetWindowSize(_curWindow->window, w, h);
}

/*!\brief dispatche les événement selon le type en utilisant les
 *  callbacks de la fenêtre ayant le focus.
 */
static inline void manageEvents(void) {
  SDL_Event event;
  while(SDL_PollEvent(&event)) {
    if(_focusedWindow && _focusedWindow->catchSDL_Event(&event))
      continue;
    switch (event.type) {
    case SDL_KEYDOWN:
      if(_focusedWindow)
	_focusedWindow->keydown(event.key.keysym.sym);
      break;
    case SDL_KEYUP:
      if(_focusedWindow)
	_focusedWindow->keyup(event.key.keysym.sym);
      break;
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
      if(_focusedWindow)
	_focusedWindow->mousebutton(event.button.button, event.button.state, event.button.x, event.button.y);
      break;
    case SDL_MOUSEMOTION:
      if (_focusedWindow){
        if (event.motion.state & (SDL_BUTTON_LMASK | SDL_BUTTON_MMASK | SDL_BUTTON_RMASK))
          _focusedWindow->mousemotion(event.motion.x, event.motion.y);
        else
          _focusedWindow->passivemousemotion(event.motion.x, event.motion.y);
      }
      break;
    case SDL_WINDOWEVENT: {
      SDL_Window * focusedw = SDL_GetWindowFromID(event.window.windowID);
      window_t wt = {(char *)SDL_GetWindowTitle(focusedw),
                     NULL,
                     0,
                     NULL,
                     NULL,
                     NULL,
                     NULL,
                     NULL,
                     NULL,
                     NULL,
                     NULL,
                     NULL};
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
}

/*!\brief corps de la boucle principale événement/simulation/affichage */
static inline void mainLoopBody(void * window, void ** data) {
  (void)data; /* warning silenced */
  window_t * w = (window_t *)window;
  SDL_GL_MakeCurrent(w->window, w->glContext);
  w->idle();
  w->display();
  SDL_GL_SwapWindow(w->window);
}
