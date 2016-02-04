/*!\file gl4duw_SDL2.c
 *
 * \brief Fonctions haut-niveau de GL4Dummies pour la gestion des fenêtres avec SDL2
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date Februry 01 2016
 *
 * \todo Actuellement ne gère qu'une seule fenetre, devra permettre
 * d'en gérer plusieurs, chacune avec son display, resize ... à la
 * manière de glut.
 */
#include <stdio.h>
#include "bin_tree.h"
#include "gl4duw_SDL2.h"

typedef struct window_t window_t;

struct window_t {
  char * name;
  SDL_Window * window;
  SDL_GLContext glContext;
};

/*!\brief arbre binaire contenant l'ensemble des fenêtres crées. */
static bin_tree_t  * _btWindows = NULL;

/*!\brief les paramètre par défaut du contexte OpenGL */
static int _glMajorVersion = 3, _glMinorVersion = 2, _glProfileMask = SDL_GL_CONTEXT_PROFILE_CORE, _glDoubleBuffer = 1, _glDepthSize = 16;
/*!\brief flag permettant de savoir si la lib a déjà été initialisée */
static int _hasInit = 0;
/*!\brief fenetre en cours */
static SDL_Window * _current_win = NULL;
static SDL_GLContext _current_oglContext = NULL;

static void quit(void);
static int  initGL4DUW(int argc, char ** argv);

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
  if(_current_win) return GL_FALSE; /* \todo à enlever quand la gestion de multiples fenetres sera effective */
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
  if( (_current_oglContext = SDL_GL_CreateContext(win)) == NULL ) {
    SDL_DestroyWindow(win);
    fprintf(stderr, "%s (%d): %s:\n\tErreur lors de la creation du contexte OpenGL : %s", 
	    __FILE__, __LINE__, __func__, SDL_GetError());
    return GL_FALSE;
  }
  fprintf(stderr, "OpenGL version: %s\n", glGetString(GL_VERSION));
  fprintf(stderr, "Supported shaders version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));  
  (_current_win = win);
  return GL_TRUE;
}

/*!\brief est appelée au moment de sortir du programme (atexit), elle
 *  libère la (les) fenêtre(s) SDL et le(s) contexte(s) OpenGL lié(s).
 */
static void quit(void) {
  if(_current_oglContext)
    SDL_GL_DeleteContext(_current_oglContext);
  if(_current_win)
    SDL_DestroyWindow(_current_win);
}

/*!\brief initialise SDL et GL4Dummies.
 *
 * \param argc nombre d'arguments passés au programme (premier argument de la fonction main).
 * \param argv liste des arguments passés au programme (second argument de la fonction main).
 * \return 0 en cas d'échec, !=0 sinon. 
 */
static int initGL4DUW(int argc, char ** argv) {
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

/* ce qui suit est temporaire en attendant d'avoir une gestion multiple de fenetre */
static void fake_resize(int w, int h) {}
static void fake_keydown(int keycode) {}
static void fake_keyup(int keycode) {}
static void fake_idle(void) {}
static void fake_display(void) {}

static void (*_resize_func)(int w, int h) = fake_resize;
static void (*_keydown_func)(int keycode) = fake_keydown;
static void (*_keyup_func)(int keycode) = fake_keyup;
static void (*_idle_func)(void) = fake_idle;
static void (*_display_func)(void) = fake_display;

static void resize(SDL_Window * win) {
  int w, h;
  SDL_GetWindowSize(win, &w, &h);
  _resize_func(w, h);
}

static void manage_events(void) {
  SDL_Event event;
  while(SDL_PollEvent(&event)) 
    switch (event.type) {
    case SDL_KEYDOWN:
      _keydown_func(event.key.keysym.sym);
      break;
    case SDL_KEYUP:
      _keyup_func(event.key.keysym.sym);
      break;
    case SDL_WINDOWEVENT:
      if(event.window.windowID == SDL_GetWindowID(_current_win)) {
	switch (event.window.event) {
	case SDL_WINDOWEVENT_RESIZED:
	  resize(_current_win);
	  break;
	case SDL_WINDOWEVENT_CLOSE:
	  event.type = SDL_QUIT;
	  SDL_PushEvent(&event);
	  break;
	}
      }
      break;
    case SDL_QUIT:
      exit(0);
    }
}

/*!\brief boucle principale événement/simulation/affichage */
void gl4duwMainLoop(void) {
  for(;;) {
    manage_events();
    _idle_func();
    _display_func();
    gl4duPrintFPS(stderr);
    SDL_GL_SwapWindow(_current_win);
    gl4duUpdateShaders();
  }
}

/*!\brief affecte la fonction appelée lors du resize */
void gl4duwResizeFunc(void (*func)(int, int)) {
  _resize_func = func ? func : fake_resize;
}

/*!\brief affecte la fonction appelée lors de l'événement key down */
void gl4duwKeyDownFunc(void (*func)(int)) {
  _keydown_func = func ? func : fake_keydown;
}

/*!\brief affecte la fonction appelée lors de l'événement key up */
void gl4duwKeyUpFunc(void (*func)(int)) {
  _keyup_func = func ? func : fake_keyup;
}

/*!\brief affecte la fonction appelée lors de l'idle (calcul/simulation avant affichage) */
void gl4duwIdleFunc(void (*func)(void)) {
  _idle_func = func ? func : fake_idle;
}

/*!\brief affecte la fonction appelée lors de l'affichage */
void gl4duwDisplayFunc(void (*func)(void)) {
  _display_func = func ? func : fake_display;
}

