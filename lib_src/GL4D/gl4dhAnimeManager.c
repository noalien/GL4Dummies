/*!\file gl4dhAnimeManager.c
 *
 * \brief Bibliothèque de gestion des écrans d'animation et de mises à
 * jour en fonction du son.
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date April 29 2014
 */
#include "gl4dhAnimeManager.h"
#include <assert.h>
#include <stdlib.h>

static int  in(void *func, void **funcList, int n);
static void add(void *func, void ***funcListp, int *n, int *s);
static void callAllWithState(GL4DHanime * animations, int state);
static void drawOrUpdateWithAudio(GL4DHanime * animations, int state);

static Uint32 _startingTime = 0;

/*!\brief pointeur vers le tableau d'animations passé à l'init avec
 * \ref gl4dhInit */
static GL4DHanime * _animations = NULL;

/*!\brief identifiant du framebuffer utilisé pour dessiner les
 *  animations */
static GLuint _fbo = 0;
/*!\brief identifiant de la texture liée en écriture au framebuffer
 * \ref _fbo (la texture dans laquelle les animations sont
 * dessinées) */
static GLuint _wTexId = 0, _wdTexId = 0;
/*!\brief largeur et hauteur de la texture \ref _wTexId liée en
 * écriture au framebuffer \ref _fbo */
static int _w = 1, _h = 1;

/*!\brief Demande l'état initialisation à tous les éléments du tableau d'animations.
 * \param animations tableau d'animations se terminant par un élément dont le champ first est à NULL.
 * \param w largeur de la texture dans laquelle sont réalisées les animations.
 * \param h hauteur de la texture dans laquelle sont réalisées les animations.
 * \param callBeforeAllAnimationsInit fonction à appeler avant le
 * lancement des inits d'animations. Ce paramètre peut-etre NULL si
 * rien n'est fait avant.
 */
void gl4dhInit(GL4DHanime * animations, int w, int h, void (*callBeforeAllAnimationsInit)(void)) {
  _animations = animations;
  _w = w; _h = h;
  if(!_fbo)
    glGenFramebuffers(1, &_fbo);
  if(!_wTexId)
    glGenTextures(1, &_wTexId);
  glBindTexture(GL_TEXTURE_2D, _wTexId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  if(!_wdTexId)
    glGenTextures(1, &_wdTexId);
  glBindTexture(GL_TEXTURE_2D, _wdTexId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, w, h, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
  glBindTexture(GL_TEXTURE_2D, 0);

  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,  _wTexId,  0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _wdTexId, 0);
  if(callBeforeAllAnimationsInit)
    callBeforeAllAnimationsInit();
  callAllWithState(_animations, GL4DH_INIT);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/*!\brief demande l'état libération à tous les éléments du tableau d'animations.
 */
void gl4dhClean(void) {
  if(_animations) {
    callAllWithState(_animations, GL4DH_FREE);
    _animations = NULL;
  }
  if(_wTexId) {
    glDeleteTextures(1, &_wTexId);
    _wTexId = 0;
  }
  if(_wdTexId) {
    glDeleteTextures(1, &_wdTexId);
    _wdTexId = 0;
  }
  if(_fbo) {
    glDeleteFramebuffers(1, &_fbo);
    _fbo = 0;
  }
}

/*!\brief donne le top-départ de la démo. Cette fonction doit être
 * appelée juste avant le début de la boucle infinie d'affichage (soit
 * juste avant le premier GL4DH_DRAW obtenu en appelant \ref gl4dhDraw).
 */
void gl4dhStartingSignal(void) {
  _startingTime = SDL_GetTicks();
}

/*!\brief renvoie le nombre de milisecondes passée depuis le
 * top-départ donné par \ref gl4dhStartingSignal.
 */
Uint32 gl4dhGetTicks(void) {
  return SDL_GetTicks() - _startingTime;
}

/*!\brief joue parmi les animations passées en argument l'animation en
 * cours (devant être jouée au temps t).
 */
void gl4dhDraw(void) {
  int vp[4];
  glGetIntegerv(GL_VIEWPORT, vp);
  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,  _wTexId,  0);
  drawOrUpdateWithAudio(_animations, GL4DH_DRAW);

  glUseProgram(0);

  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glBlitFramebuffer(0, 0, _w, _h, vp[0], vp[1], vp[0] + vp[2], vp[1] + vp[3], GL_COLOR_BUFFER_BIT, GL_LINEAR);
  glBlitFramebuffer(0, 0, _w, _h, vp[0], vp[1], vp[0] + vp[2], vp[1] + vp[3], GL_DEPTH_BUFFER_BIT, GL_NEAREST);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

/*!\brief met à jour parmi les animations passées en argument l'animation en
 * cours.
 */
void gl4dhUpdateWithAudio(void) {
  drawOrUpdateWithAudio(_animations, GL4DH_UPDATE_WITH_AUDIO);
}

/*!\brief regarde si le pointeur \a func est un élément du tableau
 * \a funcList 
 * \param func un élément dont il faut tester la présence.
 * \param funcList un tableau d'éléments.
 * \param n le nombre d'éléments dans \a funcList.
 * \return vrai si l'élément est présent, faux sinon.
 */
static int in(void *func, void **funcList, int n) {
  int i;
  for(i = 0; i < n; i++)
    if(func == funcList[i])
      return 1;
  return 0;
}

/*!\brief ajoute le pointeur \a func dans le tableau d'éléments 
 * *\a funcListp 
 * \param func un élément dont il faut tester la présence.
 * \param funcListp le pointeur vers le tableau d'éléments.
 * \param n le pointeur vers le nombre d'éléments dans *\a funcListp.
 * \param s le pointeur vers la taille, en nombre d'éléments, de *\a funcListp.
 */
static void add(void *func, void ***funcListp, int *n, int *s) {
  if(*s == 0) {
    *n = 0;
    *funcListp = calloc((*s = 16), sizeof **funcListp);
    assert(*funcListp);
  } else if(*n == *s) {
    *funcListp = realloc(*funcListp, (*s *= 2) * sizeof **funcListp);
    assert(*funcListp);
  }
  (*funcListp)[(*n)++] = func;
} 

/*!\brief Parcourt un tableau d'animations (possiblement dupliquées)
 * et les appelle une et une seule fois avec l'état \a state.
 * \param animations tableau d'animations se terminant par un élément dont le champ first est à NULL.
 * \param state état à utiliser pour chaque appel.
 */
static void callAllWithState(GL4DHanime * animations, int state) {
  int n = 0, s = 0;
  void **called = NULL;
  assert(animations);
  while(animations->first) {
    if(!in(animations->first, called, n)) {
      animations->first(state);
      add(animations->first, &called, &n, &s);
    }
    if(animations->last && !in(animations->last, called, n)) {
      animations->last(state);
      add(animations->last, &called, &n, &s);
    }
    if(animations->transition && !in(animations->transition, called, n)) {
      animations->transition(NULL, NULL, 0, 0, state);
      add(animations->transition, &called, &n, &s);
    }
    animations++;
  }
  if(called)
    free(called);
}

/*!\brief joue ou met à jour parmi les animations passées en argument
 * l'animation en cours (devant être jouée/mise à jour au temps t).
 *
 * \param animations tableau d'animations se terminant par un élément
 * dont le champ first est à NULL.
 * \param state état devant être appliqué : jouer (GL4DH_DRAW) ou mettre à
 * jour (GL4DH_UPDATE_WITH_AUDIO)
 */
static void drawOrUpdateWithAudio(GL4DHanime * animations, int state) {
  int i;
  static int cur = 0;
  Uint32 t = 0, at = 0;
  if(!animations[cur].first)
    goto noMoreAnimationsFlag;
  for(i = 0; i < cur; i++) at += animations[i].time;
  t = gl4dhGetTicks() - at;
  if(t >= animations[cur].time) {
    t -= animations[cur].time;
    cur = (cur + 1);
    if(!animations[cur].first)
      goto noMoreAnimationsFlag;
  }
  if(animations[cur].transition)
    animations[cur].transition(animations[cur].first, animations[cur].last, animations[cur].time, t, state);
  else
    animations[cur].first(state);
  return;
 noMoreAnimationsFlag:
  if(state == GL4DH_DRAW) 
    exit(0); // mettre cur = 0; pour tourner en boucle
}

