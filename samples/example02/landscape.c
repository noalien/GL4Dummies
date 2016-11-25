/*!\file landscape.c
 *
 * \brief Sample d'utilisation de la bibliothèque GL4Dummies avec SDL2
 * et en OpenGL 3.3+ , génération et affichage d'un maillage de terrain.
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
/*!\brief identifiant de la texture */
static GLuint _tId = 0;
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

static cam_t _cam = {0, 30, 0};

static void triangle_edge(Uint8 *im, int x, int y, int w, int h, int width) {
  int v;
  int p[9][2], i, w_2 = w >> 1, w_21 = w_2 + (w&1), h_2 = h >> 1, h_21 = h_2 + (h&1);
  p[0][0] = x;       p[0][1] = y;
  p[1][0] = x + w;   p[1][1] = y;
  p[2][0] = x + w;   p[2][1] = y + h;
  p[3][0] = x;       p[3][1] = y + h;
  p[4][0] = x + w_2; p[4][1] = y;
  p[5][0] = x + w;   p[5][1] = y + h_2;
  p[6][0] = x + w_2; p[6][1] = y + h;
  p[7][0] = x;       p[7][1] = y + h_2;
  p[8][0] = x + w_2; p[8][1] = y + h_2;
  for(i = 4; i < 8; i++) {
    if(im[p[i][0] + p[i][1] * width])
      continue;
    v = (((int)im[p[i - 4][0] + p[i - 4][1] * width]) +
         ((int)im[p[(i - 3) % 4][0] + p[(i - 3) % 4][1] * width])) >> 1;
    v += gl4dmSURand() * w_2;
    v = MIN(MAX(v, 1), 255);
    im[p[i][0] + p[i][1] * width] = (Uint8)v;
  }
  if(!im[p[i][0] + p[i][1] * width]) {
    v = (((int)im[p[0][0] + p[0][1] * width]) +
         ((int)im[p[1][0] + p[1][1] * width]) +
         ((int)im[p[2][0] + p[2][1] * width]) +
         ((int)im[p[3][0] + p[3][1] * width])) >> 2;
    v += gl4dmSURand() * w_2 * sqrt(2);
    v = MIN(MAX(v, 1), 255);
    im[p[8][0] + p[8][1] * width] = (Uint8)v;
  }
  if(w_2 > 1 || h_2 > 1)
    triangle_edge(im, p[0][0], p[0][1], w_2, h_2, width);
  if(w_21 > 1 || h_2 > 1)
    triangle_edge(im, p[4][0], p[4][1], w_21, h_2, width);
  if(w_21 > 1 || h_21 > 1)
    triangle_edge(im, p[8][0], p[8][1], w_21, h_21, width);
  if(w_2 > 1 || h_21 > 1)
    triangle_edge(im, p[7][0], p[7][1], w_2, h_21, width);
}

/*!\brief La fonction principale initialise la bibliothèque SDL2,
 * demande la création de la fenêtre SDL et du contexte OpenGL par
 * l'appel à \ref initWindow, initialise OpenGL avec \ref initGL et
 * lance la boucle (infinie) principale.
 */
int main(int argc, char ** argv) {
  if(SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "Erreur lors de l'initialisation de SDL :  %s", SDL_GetError());
    return -1;
  }
  atexit(SDL_Quit);
  gl4duInit(argc, argv);
  if((_win = initWindow(_windowWidth, _windowHeight, &_oglContext))) {
    initGL(_win);
    _pId = gl4duCreateProgram("<vs>../share/GL4Dummies/shaders/basic.vs", "<fs>../share/GL4Dummies/shaders/basic.fs", NULL);
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
  if(_tId)
    glDeleteTextures(1, &_tId);
  if(_vao)
    glDeleteVertexArrays(1, &_vao);
  if(_buffer)
    glDeleteBuffers(1, &_buffer);
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

static void tnormale(GLfloat * triangle, GLfloat * n) {
  GLfloat * u = triangle;
  GLfloat * v = &triangle[3];
  GLfloat * w = &triangle[6];
  GLfloat uv[3] = {v[0] - u[0], v[1] - u[1], v[2] - u[2]}, vw[3] = {w[0] - v[0], w[1] - v[1], w[2] - v[2]};
  MVEC3CROSS(n, uv, vw);
  MVEC3NORMALIZE(n);
}

static GLfloat hauteur(Uint8 * pixels, int i) {
  GLfloat s2 = 5.0f;
  return s2 * pixels[i] / 255.0;
}

static GLfloat _scote = 10.0f;
static GLfloat cote(int i, int w) {
  GLfloat s = _scote, a;
  a = s * (2.0 * (i / (GLfloat)w) - 1.0);
  return a;
}

static void normale(Uint8 * pixels, int x, int z, GLfloat * n, int w, int h) {
  int dir[][2] = {
    {1, 0},
    {0, -1},
    {1, 1},
    {1, 0},
    {0, 1},
    {1, 1},
    {-1, 0},
    {0, 1},
    {-1, -1},
    {-1, 0},
    {0, -1},
    {-1, -1}
  }, i, k;
  GLfloat t[9], tn[3];

  n[0] = n[1] = n[2] = 0;
  for(i = 0; i < 6; i++) {
    int x1 = x + dir[2 * i][0], z1 = z + dir[2 * i][1],
      x2 = x + dir[2 * i + 1][0], z2 = z + dir[2 * i + 1][1];
    if( x1 < 0 || z1 < 0 || x2 < 0 || z2 < 0 ||
	x1 >= w || z1 >= h || x2 >= w || z2 >= h )
      continue;
    k = 0;
    t[k++] = cote(x, w);
    t[k++] = hauteur(pixels, z * w + x);
    t[k++] = cote(z, h);
    t[k++] = cote(x1, w);
    t[k++] = hauteur(pixels, z1 * w + x1);
    t[k++] = cote(z1, h);
    t[k++] = cote(x2, w);
    t[k++] = hauteur(pixels, z2 * w + x2);
    t[k++] = cote(z2, h);
    tnormale(t, tn);
    n[0] += tn[0];
    n[1] += tn[1];
    n[2] += tn[2];
  }
  MVEC3NORMALIZE(n);
}

static void initData(void) {
  const int w = 256, h = 256;
  int i, j, k, c;
  Uint8 * pixels;
  GLfloat * data;
  pixels = malloc(w * h * sizeof *pixels);
  assert(pixels);
  memset(pixels, 0, w * h * sizeof *pixels);
  pixels[0] = 255;         pixels[w - 1] = 1;
  pixels[(h - 1) * w] = 1; pixels[(h - 1) * w + w - 1] = 1;
  triangle_edge(pixels, 0, 0, w - 1, h - 1, w);

  data = malloc((w - 1) * (h - 1) * 6 * 8 * sizeof * data);
  assert(data);
  for(i = 0, k = 0; i < h - 1; i++) {
    GLfloat z = cote(i, h), zp1 = cote(i + 1, h);
    for(j = 0; j < w - 1; j++) {
      GLfloat x = cote(j, w), xp1 = cote(j + 1, w);
      c = i * w + j;
      //0
      data[k++] = x;
      data[k++] = hauteur(pixels, c);
      data[k++] = z;
      normale(pixels, j, i, &data[k], w, h);
      k += 3;
      data[k++] = 0.5 + x / (2.0f * _scote); data[k++] = 0.5 + z / (2.0f * _scote);
      //1
      data[k++] = xp1;
      data[k++] = hauteur(pixels, c + w + 1);
      data[k++] = zp1;
      normale(pixels, j + 1, i + 1, &data[k], w, h);
      k += 3;
      data[k++] = 0.5 + xp1 / (2.0f * _scote); data[k++] = 0.5 + zp1 / (2.0f * _scote);
      //2
      data[k++] = xp1;
      data[k++] = hauteur(pixels, c + 1);
      data[k++] = z;
      normale(pixels, j + 1, i, &data[k], w, h);
      k += 3;
      data[k++] = 0.5 + xp1 / (2.0f * _scote); data[k++] = 0.5 + z / (2.0f * _scote);
      //0
      data[k++] = x;
      data[k++] = hauteur(pixels, c);
      data[k++] = z;
      normale(pixels, j, i, &data[k], w, h);
      k += 3;
      data[k++] = 0.5 + x / (2.0f * _scote); data[k++] = 0.5 + z / (2.0f * _scote);
      //4
      data[k++] = x;
      data[k++] = hauteur(pixels, c + w);
      data[k++] = zp1;
      normale(pixels, j, i + 1, &data[k], w, h);
      k += 3;
      data[k++] = 0.5 + x / (2.0f * _scote); data[k++] = 0.5 + zp1 / (2.0f * _scote);
      //5
      data[k++] = xp1;
      data[k++] = hauteur(pixels, c + w + 1);
      data[k++] = zp1;
      normale(pixels, j + 1, i + 1, &data[k], w, h);
      k += 3;
      data[k++] = 0.5 + xp1 / (2.0f * _scote); data[k++] = 0.5 + zp1 / (2.0f * _scote);
    }
  }
  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glGenVertexArrays(1, &_vao);
  glBindVertexArray(_vao);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);

  glGenBuffers(1, &_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, _buffer);
  glBufferData(GL_ARRAY_BUFFER, (w - 1) * (h - 1) * 6 * 8 * sizeof *data, data, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof *data, (const void *)0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE,  8 * sizeof *data, (const void *)(3 * sizeof *data));
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof *data, (const void *)(6 * sizeof *data));
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  glGenTextures(1, &_tId);
  glBindTexture(GL_TEXTURE_2D, _tId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, pixels);
  free(pixels);
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
  gl4duPerspectivef(60.0, _windowWidth / (GLfloat) _windowHeight, 1.0, 1000.0);

  //  gl4duFrustumf(-0.5, 0.5, -0.5 * _windowHeight / _windowWidth, 0.5 * _windowHeight / _windowWidth, 1.0, 1000.0);
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
      _cam.theta -= dt * dtheta;
    }
    if(_keys[KRIGHT]) {
      _cam.theta += dt * dtheta;
    }
    if(_keys[KUP]) {
      _cam.x += dt * pas * sin(_cam.theta);
      _cam.z += -dt * pas * cos(_cam.theta);
    }
    if(_keys[KDOWN]) {
      _cam.x += -dt * pas * sin(_cam.theta);
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
  GLfloat * mv, temp[4] = {5 * sin(a0), 10.5, -5, 1.0}, lumpos[4];
  int xm, ym;
  SDL_PumpEvents();
  SDL_GetMouseState(&xm, &ym);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(_pId);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _tId);
  glUniform1i(glGetUniformLocation(_pId, "myTexture"), 0);
  glUniform1i(glGetUniformLocation(_pId, "heightMap"), 1);

  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();
  gl4duLookAtf(_cam.x, 4.0, _cam.z, _cam.x + sin(_cam.theta), 4.0 - (ym - (_windowHeight >> 1)) / (GLfloat)_windowHeight, _cam.z - cos(_cam.theta), 0.0, 1.0, 0.0);

  mv = gl4duGetMatrixData();
  MMAT4XVEC4(lumpos, mv, temp);
  glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lumpos);

  gl4duSendMatrices();
  glBindVertexArray(_vao);
  glDrawArrays(GL_TRIANGLES, 0, 255 * 255 * 6);
  glBindVertexArray(0);

}

