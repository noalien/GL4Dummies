/*!\file window.c
 * \brief géométries lumière diffuse et transformations de base en GL4Dummies
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date April 15 2016 */
#include <stdio.h>
#include <GL4D/gl4dp.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4dh.h>
#include <GL4D/gl4df.h>
#include <GL4D/gl4duw_SDL2.h>
#include <audioHelper.h>
#include <SDL_ttf.h>
#include <assert.h>
/* Prototypes des fonctions statiques contenues dans ce fichier C */
static void init(void);
static void draw(void);
/*!\brief identifiant du programme GLSL */
static GLuint _pId = 0, _creditsPId = 0;
/*!\brief quelques objets géométriques */
static GLuint _quad = 0, _cube = 0, _screenId = 0, _textTexId = 0;

static int _rayon = 0;

void transformations(int state) {
  int l, i;
  Uint16 * s;
  double m = 0;
  /* INITIALISEZ VOS VARIABLES */
  switch(state) {
  case GL4DH_INIT:
    init();
    /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    return;
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
    s = (Uint16 *)ahGetAudioStream();
    l = ahGetAudioStreamLength();
    for(i = 0; i < l /2 ; ++i)
      m += s[i] / ((1 << 16) - 1.0);
    _rayon = 255.0 * m / (l / 2);
    return;
  default: /* GL4DH_DRAW */
    draw();
    return;
  }
}


static void ligneO1(int x0, int y0, int x1, int y1) {
  /* u est la différence en x pour recentrer le segment en 0 côté abscisse */
  int u = x1 - x0, x;
  /* v est la différence en y pour recentrer le segment en 0 côté ordonnées */
  int v = y1 - y0;
  if(u > v) { /* cas du premier octant (x augmente tout le temps, y de
               * temps en temps en fonction de la pente */
    /* p est la pente de la droite y = p * x */
    float p = v / (float)u, y;
    for(x = 0, y = 0; x <= u; x++) {
      /* y augmente de manière proportionnelle à la pente mais ce
       * n'est que sa partie entière qui sera dessinée */
      y += p;
      /* nous recentrons le segment en (x0, y0) pour en faire le point
       * de départ */
      if(IN_SCREEN(x0 + x, y0 + ((int)y)))
	gl4dpPutPixel(x0 + x, y0 + ((int)y));
    }
  } else { /* cas du deuxième octant (y augmente tout le temps, x de
            * temps en temps en fonction de la pente. Pour faire
            * simple nous inversions l'usage de x et y et aussi de u
            * et v car nous calculons la version symatrique du cas
            * premier octan par rapport à l'axe y = x */
    float p = u / (float)v, y;
    for(x = 0, y = 0; x <= v; x++) {
      y += p;
      if(IN_SCREEN(x0 + ((int)y), y0 + x))
	gl4dpPutPixel(x0 + ((int)y), y0 + x);
    }
  }
}

/*!\brief fonction appelée à chaque draw par la gl4duwMainLoop. */
static void dessin(void) {
  float a;
  gl4dpSetScreen(_screenId);
  gl4dpClearScreenWith(RGB(100,100,100));
  /* partant du point (0, 0), nous dessinons des segments décrivant un
   * quart de cercle ; nous utilisons la formule trigonométrique */
  for(a = 0; a < M_PI / 2.0f; a += 0.02f) {
    /* choisissons au hasard 3 intensités de composantes primaires de
     * couleur (min est 0, max est 255 car sur un octet non signé). */
    GLubyte r = rand() % 256, g = rand() % 256, b = rand() % 256;
    /* nous mettons la couleur aléatoire */
    gl4dpSetColor(RGB(r, g, b));
    /* dessinons le segment de droite */
    ligneO1(0, 0, 
            _rayon * cos(a), 
            _rayon * sin(a));
  }
  /* mise à jour de l'écran, essayez de mettre cet appel dans la
   * boule ci-dessus. */
  gl4dpUpdateScreen(NULL);
}


static void initText(GLuint * ptId, const char * text) {
  static int firstTime = 1;
  SDL_Color c = {100, 0, 0, 255};
  SDL_Surface * d, * s;
  TTF_Font * font = NULL;
  if(firstTime) {
    /* initialisation de la bibliothèque SDL2 ttf */
    firstTime = 0;
  }
  if(*ptId == 0) {
    /* initialisation de la texture côté OpenGL */
    glGenTextures(1, ptId);
    glBindTexture(GL_TEXTURE_2D, *ptId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  }
  /* chargement de la font */
  if( !(font = TTF_OpenFont("DejaVuSans-Bold.ttf", 128)) ) {
    fprintf(stderr, "TTF_OpenFont: %s\n", TTF_GetError());
    return;
  }
  /* création d'une surface SDL avec le texte */
  d = TTF_RenderUTF8_Blended_Wrapped(font, text, c, 2048);
  if(d == NULL) {
    TTF_CloseFont(font);
    fprintf(stderr, "Erreur lors du TTF_RenderText\n");
    return;
  }
  /* copie de la surface SDL vers une seconde aux spécifications qui correspondent au format OpenGL */
  s = SDL_CreateRGBSurface(0, d->w, d->h, 32, R_MASK, G_MASK, B_MASK, A_MASK);
  assert(s);
  SDL_BlitSurface(d, NULL, s, NULL);
  SDL_FreeSurface(d);
  /* transfert vers la texture OpenGL */
  glBindTexture(GL_TEXTURE_2D, *ptId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, s->pixels);
  fprintf(stderr, "Dimensions de la texture : %d %d\n", s->w, s->h);
  SDL_FreeSurface(s);
  TTF_CloseFont(font);
  glBindTexture(GL_TEXTURE_2D, 0);
}




/*!\brief initialise les paramètres OpenGL et les données */
static void init(void) {
  initText(&_textTexId,
           "Il y a bien longtemps dans une galaxie lointaine, très lointaine...\n\n\n"
           "C'est un temps d'incertitude. De mauvais présages s'emparent de la"
           " galaxie avec l'infuence croissante de Premier Ordre, mais une lueur"
           " d'espoir apparaît grâce aux force héroïques de la"
           " Résistance. \n\n\nAlors qu'ils luttent contre la puissance sombre de"
           " l'ennemi , ces héros courageux peuvent encore éveiller l'habileté et"
           " la force nécessaires pour forger un nouvel avenir...");
  _screenId = gl4dpInitScreenWithDimensions(300, 300);
  glDisable(GL_CULL_FACE);
  glClearColor(1.0f, 0.7f, 0.7f, 0.0f);
  _pId  = gl4duCreateProgram("<vs>shaders/dep3d.vs", "<fs>shaders/dep3d.fs", NULL);
  _creditsPId  = gl4duCreateProgram("<vs>shaders/credits.vs", "<fs>shaders/credits.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  _quad = gl4dgGenQuadf();
  _cube = gl4dgGenCubef();
  
}

/*!\brief dessine dans le contexte OpenGL actif. */
static void draw(void) {
  const GLfloat inclinaison = -85.0;
  static GLfloat t0 = -1;
  GLfloat t, d;
  if(t0 < 0.0f)
    t0 = SDL_GetTicks();
  t = (SDL_GetTicks() - t0) / 1000.0f, d = -2.4f /* du retard pour commencer en bas */ + 0.25f /* vitesse */ * t;

  static GLfloat a = 0;
  GLint vp[4];
  glGetIntegerv(GL_VIEWPORT, vp);
  glEnable(GL_DEPTH_TEST);
  dessin();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.1, 0.1, -0.1f * vp[3] / vp[2], 0.1f * vp[3] / vp[2], 0.1, 100.0);
  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();
  glUseProgram(_pId);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _screenId);
  glUniform1i(glGetUniformLocation(_pId, "tex"), 0);
  //gl4duScalef(0.3, 0.3, 0.3);
  gl4duTranslatef(0.0, 0.0, -3);
  gl4duRotatef(a, 0.0, 1.0, 0.0);
  gl4duSendMatrices();
  gl4dgDraw(_cube);

  a++;



  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glUseProgram(_creditsPId);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _textTexId);
  glUniform1i(glGetUniformLocation(_creditsPId, "inv"), 1);
  glUniform1i(glGetUniformLocation(_creditsPId, "tex"), 0);
  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();
  gl4duScalef(1, 5, 1);
  gl4duTranslatef(0, d * cos(inclinaison * M_PI / 180.0f), -2 + d * sin(inclinaison * M_PI / 180.0f));
  gl4duRotatef(inclinaison, 1, 0, 0);
  gl4duSendMatrices();
  gl4dgDraw(_quad);

}

