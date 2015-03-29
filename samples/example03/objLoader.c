/*!\file objLoader.c
 *
 * \brief Sample d'utilisation de la bibliothèque GL4Dummies avec SDL2
 * et en OpenGL 3.3+, lecture et utilisation d'un obj (wavefront) avec
 * utilisation de la bibliothèque glm de Nate Robins
 *
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 *
 * \date October 30 2014
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <GL4D/gl4du.h>
#include "glm/glm.h"

/*
 * Prototypes des fonctions statiques contenues dans ce fichier C
 */
static SDL_Window * initWindow(int w, int h, SDL_GLContext * poglContext);
static void quit(void);
static void initGL(SDL_Window * win);
static void initData(void);
static void resizeGL(SDL_Window * win);
static void loop(SDL_Window * win);
static void manageEvents(SDL_Window * win);
static void draw(GLfloat a0);

static GLMmodel * _model = NULL;
static GLuint _objVao = 0, _objBD = 0, _objBI = 0;
/*!\brief pointeur vers la (future) fenêtre SDL */
static SDL_Window * _win = NULL;
static int _windowWidth = 800, _windowHeight = 600;
/*!\brief pointeur vers le (futur) contexte OpenGL */
static SDL_GLContext _oglContext = NULL;
/*!\brief identifiant du (futur) vertex array object */
static GLuint _vao = 0;
/*!\brief identifiant du (futur) buffer de data */
static GLuint _buffer = 0;
/*!\brief identifiant du (futur) GLSL program */
static GLuint _pId = 0;
static GLuint _pId2 = 0;
/*!\brief identifiant de la texture */
static GLuint _pause = 0;
enum kyes_t {
  KLEFT = 0,
  KRIGHT,
  KUP,
  KDOWN
};

static GLuint _keys[] = {0, 0, 0, 0};

typedef struct cam_t cam_t;
struct cam_t {
  GLfloat x, z;
  GLfloat theta;
};

static cam_t _cam = {0, 50, 0};

/*!\brief La fonction principale initialise la bibliothèque SDL2,
 * demande la création de la fenêtre SDL et du contexte OpenGL par
 * l'appel à \ref initWindow, initialise OpenGL avec \ref initGL et
 * lance la boucle (infinie) principale.
 */
int main(int argc, char ** argv) {
  char temp[BUFSIZ], * fn = "../share/GL4Dummies/data/obama.obj";
  if(SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "Erreur lors de l'initialisation de SDL :  %s", SDL_GetError());
    return -1;
  }
  atexit(SDL_Quit);
  gl4duInit(argc, argv);
  if((_win = initWindow(_windowWidth, _windowHeight, &_oglContext))) {
    initGL(_win);
    _pId = gl4duCreateProgram("<vs>../share/GL4Dummies/shaders/basic.vs", "<fs>../share/GL4Dummies/shaders/basic.fs", NULL);
    _pId2 = gl4duCreateProgram("<vs>../share/GL4Dummies/shaders/basic.vs", "<fs>../share/GL4Dummies/shaders/sobel.fs", NULL);
    gl4duMakeBinRelativePath(temp, sizeof temp, fn);
    if( (_model = glmReadOBJ(fn)) == NULL && (_model = glmReadOBJ(temp)) == NULL) {
      fprintf(stderr, "Impossible d'ouvrir le fichier %s\n", fn);
      return -2;
    }
    initData();
    loop(_win);
  } else
    fprintf(stderr, "Erreur lors de la creation de la fenetre\n");
  return 0;
}

/*!\brief Cette fonction est appelée au moment de sortir du programme
 *  (atexit), elle libère la fenêtre SDL \ref _win et le contexte
 *  OpenGL \ref _oglContext.
 */
static void quit(void) {
  if(_model) {
    glmDelete(_model);
    _model = NULL;
  }
  if(_vao)
    glDeleteVertexArrays(1, &_vao);
  if(_buffer)
    glDeleteBuffers(1, &_buffer);
  if(_objVao)
    glDeleteVertexArrays(1, &_objVao);
  if(_objBD)
    glDeleteBuffers(1, &_objBD);
  if(_objBI)
    glDeleteBuffers(1, &_objBI);
  if(_oglContext)
    SDL_GL_DeleteContext(_oglContext);
  if(_win)
    SDL_DestroyWindow(_win);
  gl4duClean(GL4DU_ALL);
}

/*!\brief Cette fonction créé la fenêtre SDL de largeur \a w et de
 *  hauteur \a h, le contexte OpenGL \a poglContext et stocke le
 *  pointeur dans poglContext. Elle retourne le pointeur vers la
 *  fenêtre SDL.
 *
 * Le contexte OpenGL créé est en version 3 pour
 * SDL_GL_CONTEXT_MAJOR_VERSION, en version 2 pour
 * SDL_GL_CONTEXT_MINOR_VERSION et en SDL_GL_CONTEXT_PROFILE_CORE
 * concernant le profile. Le double buffer est activé et le buffer de
 * profondeur est en 24 bits.
 *
 * \param w la largeur de la fenêtre à créer.
 * \param h la hauteur de la fenêtre à créer.
 * \param poglContext le pointeur vers la case où sera référencé le
 * contexte OpenGL créé.
 * \return le pointeur vers la fenêtre SDL si tout se passe comme
 * prévu, NULL sinon.
 */
static SDL_Window * initWindow(int w, int h, SDL_GLContext * poglContext) {
  SDL_Window * win = NULL;
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  if( (win = SDL_CreateWindow("Fenetre GL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			      w, h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE |
			      SDL_WINDOW_SHOWN)) == NULL )
    return NULL;
  if( (*poglContext = SDL_GL_CreateContext(win)) == NULL ) {
    SDL_DestroyWindow(win);
    return NULL;
  }
  fprintf(stderr, "Version d'OpenGL : %s\n", glGetString(GL_VERSION));
  fprintf(stderr, "Version de shaders supportes : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
  atexit(quit);
  return win;
}

/*!\brief Cette fonction initialise les paramètres OpenGL.
 *
 * \param win le pointeur vers la fenêtre SDL pour laquelle nous avons
 * attaché le contexte OpenGL.
 */
static void initGL(SDL_Window * win) {
  glClearColor(0.0f, 0.4f, 0.9f, 0.0f);
  glEnable(GL_DEPTH_TEST);
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  resizeGL(win);
}

/*\todo utiliser les groups des modèles pour affecter des materiaux */
static void initData(void) {
  int i, j;
  GLfloat s = 1.0, data[] = {
    /* 4 coordonnées de sommets */
    -s, -s, 0.0f,
    s , -s, 0.0f,
    -s,  s, 0.0f,
    s ,  s, 0.0f,
    /* 4 normales */
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    /* 4 coordonnées de texture, une par sommet */
    0.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f
  };
  GLfloat * all = NULL;
  GLuint * ti;
  glmVertexNormals(_model, 90, GL_FALSE);
  all = malloc(3 * 2 * (_model->numvertices + 1) * sizeof *all);
  assert(all);
  memcpy(all, _model->vertices, 3 * _model->numvertices * sizeof *all);
  for(i = 0; i < (int)_model->numtriangles; i++) {
    GLMtriangle * triangle = &(_model->triangles[i]);
    for(j = 0; j < 3; j++)
      memcpy(&all[3 * _model->numvertices + 3 * triangle->vindices[j]], &_model->normals[3 * triangle->nindices[j]], 3 * sizeof *all);
  }
  if(!_objVao)
    glGenVertexArrays(1, &_objVao);
  glBindVertexArray(_objVao);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
  if(!_objBD)
    glGenBuffers(1, &_objBD);
  if(!_objBI)
    glGenBuffers(1, &_objBI);
  glBindBuffer(GL_ARRAY_BUFFER, _objBD);
  glBufferData(GL_ARRAY_BUFFER,
  	       3 * (2 * _model->numvertices) * sizeof *all, all, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
  glVertexAttribPointer(1, 3, GL_FLOAT,  GL_TRUE, 0, (const void *)(3 * _model->numvertices * sizeof *all));

  free(all);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _objBI);

  ti = malloc(_model->numtriangles * 3 * sizeof *ti);
  assert(ti);
  for(i = 0; i < (int)_model->numtriangles; i++) {
    GLMtriangle * triangle = &(_model->triangles[i]);
    memcpy(&ti[3 * i], triangle->vindices, 3 * sizeof *ti);
    ti[3 * i] = triangle->vindices[0];
    ti[3 * i + 1] = triangle->vindices[1];
    ti[3 * i + 2] = triangle->vindices[2];
  }
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
  	       _model->numtriangles * 3 * sizeof *ti, ti, GL_STATIC_DRAW);
  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  free(ti);

  glGenVertexArrays(1, &_vao);
  glBindVertexArray(_vao);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);

  glGenBuffers(1, &_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, _buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof data, data, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE,  0, (const void *)(4 * 3 * sizeof *data));
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (const void *)(4 * 6 * sizeof *data));
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

/*!\brief Cette fonction paramétrela vue (viewPort) OpenGL en fonction
 * des dimensions de la fenêtre SDL pointée par \a win.
 *
 * \param win le pointeur vers la fenêtre SDL pour laquelle nous avons
 * attaché le contexte OpenGL.
 */
static void resizeGL(SDL_Window * win) {
  SDL_GetWindowSize(win, &_windowWidth, &_windowHeight);
  glViewport(0, 0, _windowWidth, _windowHeight);
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.5, 0.5, -0.5 * _windowHeight / _windowWidth, 0.5 * _windowHeight / _windowWidth, 1.0, 1000.0);
}

/*!\brief Boucle infinie principale : gère les évènements, dessine,
 * imprime le FPS et swap les buffers.
 *
 * \param win le pointeur vers la fenêtre SDL pour laquelle nous avons
 * attaché le contexte OpenGL.
 */
static void loop(SDL_Window * win) {
  GLfloat a = 0.0f, dt = 0.0f, dtheta = (GLfloat)GL4DM_PI, pas = 5.0f;
  Uint32 t0 = SDL_GetTicks(), t;
  SDL_GL_SetSwapInterval(1);
  for(;;) {
    dt = ((t = SDL_GetTicks()) - t0) / 1000.0;
    t0 = t;
    manageEvents(win);


    if(_keys[KLEFT]) {
      _cam.theta += dt * dtheta;
    }
    if(_keys[KRIGHT]) {
      _cam.theta -= dt * dtheta;
    }
    if(_keys[KUP]) {
      _cam.x += -dt * pas * sin(_cam.theta);
      _cam.z += -dt * pas * cos(_cam.theta);
    }
    if(_keys[KDOWN]) {
      _cam.x += dt * pas * sin(_cam.theta);
      _cam.z += dt * pas * cos(_cam.theta);
    }


    draw(a + 0);
    gl4duPrintFPS(stderr);
    SDL_GL_SwapWindow(win);
    gl4duUpdateShaders();
    if(!_pause)
      a += 0.1 * 2.0 * GL4DM_PI * dt;
  }
}

/*!\brief Cette fonction permet de gérer les évènements clavier et
 * souris via la bibliothèque SDL et pour la fenêtre pointée par \a
 * win.
 *
 * \param win le pointeur vers la fenêtre SDL pour laquelle nous avons
 * attaché le contexte OpenGL.
 */
static void manageEvents(SDL_Window * win) {
  GLint v[2];
  SDL_Event event;
  while(SDL_PollEvent(&event))
    switch (event.type) {
    case SDL_KEYDOWN:
      switch(event.key.keysym.sym) {
      case SDLK_LEFT:
	_keys[KLEFT] = 1;
	break;
      case SDLK_RIGHT:
	_keys[KRIGHT] = 1;
	break;
      case SDLK_UP:
	_keys[KUP] = 1;
	break;
      case SDLK_DOWN:
	_keys[KDOWN] = 1;
	break;
      case SDLK_ESCAPE:
      case 'q':
	exit(0);
      case ' ':
      case 'p':
	_pause = !_pause;
	break;
      case 'w':
	glGetIntegerv(GL_POLYGON_MODE, v);
	if(v[0] == GL_FILL)
	  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
	  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	break;
      default:
	fprintf(stderr, "La touche %s a ete pressee\n",
		SDL_GetKeyName(event.key.keysym.sym));
	break;
      }
      break;
    case SDL_KEYUP:
      switch(event.key.keysym.sym) {
      case SDLK_LEFT:
	_keys[KLEFT] = 0;
	break;
      case SDLK_RIGHT:
	_keys[KRIGHT] = 0;
	break;
      case SDLK_UP:
	_keys[KUP] = 0;
	break;
      case SDLK_DOWN:
	_keys[KDOWN] = 0;
	break;
      }
      break;
    case SDL_WINDOWEVENT:
      if(event.window.windowID == SDL_GetWindowID(win)) {
	switch (event.window.event)  {
	case SDL_WINDOWEVENT_RESIZED:
	  resizeGL(win);
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

/*!\brief Cette fonction dessine dans le contexte OpenGL actif.
 */
static void draw(GLfloat a0) {
  static int ft = 1;
  static GLuint fbo, tex, depthTex;
  GLint v[2];
  GLfloat * mv, temp[4] = {50 * sin(a0), 100.5, 50, 1.0}, lumpos[4];
  if(ft) {
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _windowWidth, _windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glGenTextures(1, &depthTex);
    glBindTexture(GL_TEXTURE_2D, depthTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, _windowWidth, _windowHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
    glGenFramebuffers(1, &fbo);
    ft = 0;
  }

  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex,  0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glUseProgram(_pId);
  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();
  gl4duTranslatef(-_cam.x, -1.0, -_cam.z);
  gl4duRotatef(_cam.theta * 180.0 / GL4DM_PI, 0, 1, 0);
  mv = gl4duGetMatrixData();
  MMAT4XVEC4(lumpos, mv, temp);
  glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lumpos);

  gl4duSendMatrices();
  glBindVertexArray(_objVao);
  glDrawElements(GL_TRIANGLES, 3 * _model->numtriangles, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

  glUseProgram(0);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glBlitFramebuffer(0, 0, _windowWidth, _windowHeight, 0, 0, _windowWidth, _windowHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glGetIntegerv(GL_POLYGON_MODE, v);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glUseProgram(_pId2);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex);
  glUniform1i(glGetUniformLocation(_pId2, "myTexture"), 0);
  glUniform1f(glGetUniformLocation(_pId2, "width"), (GLfloat)_windowWidth);
  glUniform1f(glGetUniformLocation(_pId2, "height"), (GLfloat)_windowHeight);

  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();
  gl4duBindMatrix("projectionMatrix");
  gl4duPushMatrix();
  gl4duLoadIdentityf();
  gl4duSendMatrices();
  glBindVertexArray(_vao);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
  gl4duPopMatrix();
  glPolygonMode(GL_FRONT_AND_BACK, v[0]);
}

