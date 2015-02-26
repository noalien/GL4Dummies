/*!\file sample01.c
 *
 * \brief Sample d'utilisation de la bibliothèque GL4Dummies avec SDL2
 * et en OpenGL 3.3+ 
 * 
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 *
 * \date October 30 2014
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <gl4du.h>


#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/imgproc_c.h>

static CvSize _s2tSize = {1, 1};
static IplImage * _s2tImg = NULL,  * _s2tUDimg = NULL;
static CvCapture * _s2tCapture = NULL;

static void stream2texFree(void) {
  /* Ne doit pas être fait, la donnée renvoyée par cvQueryFrame ne
   * doit pas être libérée par l'utilisateur
   *
   * if(_s2tImg)
   *   cvReleaseImage(&_s2tImg); 
   */
  if(_s2tUDimg)
    cvReleaseImage(&_s2tUDimg); 
  if(_s2tCapture) {
    cvReleaseCapture(&_s2tCapture);
    _s2tCapture = NULL;
  }
}

void gl4dutStream2texInit(const char * input) {
  static int firstTime = 1;
  _s2tCapture = (input) ? cvCreateFileCapture(input) : cvCaptureFromCAM(CV_CAP_ANY);
  _s2tSize.width  = (int)cvGetCaptureProperty(_s2tCapture, CV_CAP_PROP_FRAME_WIDTH);
  _s2tSize.height = (int)cvGetCaptureProperty(_s2tCapture, CV_CAP_PROP_FRAME_HEIGHT);
  fprintf(stderr, "%s(%d): %s: dimensions du flux %dx%d\n", __FILE__, __LINE__, __func__, _s2tSize.width, _s2tSize.height);
  if(firstTime) {
    atexit(stream2texFree);
    firstTime = 0;
  }
}

int gl4dutStream2texGrab(GLuint texId) {
  GLubyte d[] = {0x00, 0x00, 0xFF};
  if(!_s2tCapture || !texId) return 0;
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texId);
  if((_s2tImg = cvQueryFrame(_s2tCapture))) {
    int i, j, i1 , i2;
    CvMemStorage * memSto = cvCreateMemStorage(0);
    IplImage *img = cvCreateImage(cvGetSize(_s2tImg), IPL_DEPTH_8U, 1);
    unsigned char * dd = (unsigned char *)_s2tImg->imageData, * nd;
    if(_s2tUDimg == NULL)
      _s2tUDimg = cvCreateImage(cvGetSize(_s2tImg), _s2tImg->depth, 4);
    assert(_s2tUDimg);
    nd = (unsigned char *)_s2tUDimg->imageData;
    for(i = 0; i < _s2tImg->height; i++) {
      i1 = _s2tUDimg->widthStep * (i/*_s2tUDimg->height - i - 1*/);//4 * (_s2tImg->height - i - 1) * _s2tImg->width;
      i2 = _s2tImg->widthStep * i;//_s2tImg->nChannels * i * _s2tImg->width;
      for(j = 0; j < _s2tImg->width; j++) {
	nd[i1 + _s2tUDimg->nChannels * j] = dd[i2 + _s2tImg->nChannels * j];
	nd[i1 + _s2tUDimg->nChannels * j + 1] = dd[i2 + _s2tImg->nChannels * j + 1];
	nd[i1 + _s2tUDimg->nChannels * j + 2] = dd[i2 + _s2tImg->nChannels * j + 2];
	nd[i1 + _s2tUDimg->nChannels * j + 3] = _s2tImg->nChannels == 4 ? dd[i2 + _s2tImg->nChannels * j + 3] : 255;
      }
    }
    cvCvtColor(_s2tImg, img, CV_RGB2GRAY);
    cvSmooth(img, img, CV_GAUSSIAN, 5, 5, 0, 0);
    CvSeq * res = cvHoughCircles(img, memSto, CV_HOUGH_GRADIENT, 2, _s2tImg->width / 8, 128, 96, 10, 100);//, double param1=100, double param2=100, int min_radius=0, int max_radius=0 );
    cvReleaseImage(&img);
    //CvSeq* cvHoughCircles(CvArr* image, void* circle_storage, int method, double dp, double min_dist, double param1=100, double param2=100, int min_radius=0, int max_radius=0 );
    for(i = 0; i < res->total; i++) {
      float * p = (float *)cvGetSeqElem(res, i);
      CvPoint pt = cvPoint(cvRound(p[0]), cvRound(p[1]));
      cvCircle(_s2tUDimg, pt, cvRound(p[2]), CV_RGB(0xff, 0, 0), 1, 8, 0);
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _s2tUDimg->width, _s2tUDimg->height, 0, GL_BGRA, GL_UNSIGNED_BYTE, _s2tUDimg->imageData);
    return 1;
  }
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, d);
  return 0;
}





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

static cam_t _cam = {0, 0, 0};

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
  if((_win = initWindow(_windowWidth, _windowHeight, &_oglContext))) {
    initGL(_win);
    gl4dutStream2texInit(NULL);
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

static void initData(void) {
  GLubyte c[] = {0, 0, 255};
  GLfloat s = 10.0, data[] = { 
    /* 4 coordonnées de sommets */
    -s, 0.0f, -s,
    s , 0.0f, -s,
    -s, 0.0f,  s,
    s , 0.0f,  s,
    /* 4 normales */
    0.0f, 1.0f, 0.0f, 
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    /* 4 coordonnées de texture, une par sommet */
    0.0f, 0.0f, 
    1.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f
  };
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

  glGenTextures(1, &_tId);
  glBindTexture(GL_TEXTURE_2D, _tId);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, c);
  glBindTexture(GL_TEXTURE_2D, 0);
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
  GLfloat a = 0.0, dt = 0.0, dtheta = GL4DM_PI, pas = 5.0;
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
  GLfloat * mv, temp[4] = {5 * sin(a0), 0.5, -5, 1.0}, lumpos[4];

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(_pId);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _tId);
  gl4dutStream2texGrab(_tId);
  glUniform1i(glGetUniformLocation(_pId, "myTexture"), 0);

  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();
  gl4duLookAtf(_cam.x, 1.0, _cam.z, _cam.x - sin(_cam.theta), 1.0, _cam.z - cos(_cam.theta), 0.0, 1.0, 0.0);

  mv = gl4duGetMatrixData();
  MMAT4XVEC4(lumpos, mv, temp);
  glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lumpos);

  gl4duSendMatrices();
  glBindVertexArray(_vao);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);

  gl4duPushMatrix();
  gl4duTranslatef(2, 1, -7);
  gl4duRotatef(90, 1.0, 0.0, 0.0);
  //gl4duRotatef(-100.0f * a0, 0.0, 0.0, 1.0);
  gl4duScalef(0.1, 0.1, 0.1);

  gl4duSendMatrix();
  glBindVertexArray(_vao);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
  gl4duPopMatrix();
}

