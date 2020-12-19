/*!\file window.c
 *
 * \brief Chargement et visualisation d'une image TIFF à une
 * composante 16bits et application d'une LUT
 *
 * \author Farès BELHADJ, amsi@up8.edu 
 * \date July 13 2020
 */

#include <GL4D/gl4du.h>
#include <GL4D/gl4dp.h>
#include <GL4D/gl4dg.h>
#include <GL4D/gl4duw_SDL2.h>
#include <tiffio.h>
#include <assert.h>

/* Prototypes des fonctions statiques contenues dans ce fichier C */
static void init(const char * filename);
static void resize(int w, int h);
static void draw(void);
static void keydown(int keycode);
static void quit(void);

/*!\brief largeur et hauteur de la fenêtre */
static int _wW = 800, _wH = 800;
/*!\brief identifiant du quadrilatère */
static GLuint _quad = 0;
/*!\brief identifiant du GLSL program */
static GLuint _pId = 0;
/*!\brief identifiant de la texture contenant l'image */
static GLuint _texId = 0;
/*!\brief identifiant de la texture LUT */
static GLuint _lutId = 0;
/*!\brief booléen pour indiquer l'usage ou non de la LUT */
static GLboolean _useLUT = GL_FALSE;
/*!\brief les ratios de l'image */
static GLfloat _iratio[2] = {1.0f, 1.0f};
/*!\brief le zoom sur l'image */
static GLfloat _zoom = 1.0f;
/*!\brief translations sur l'image */
static GLfloat _tx = 0.0f, _ty = 0.0f;
/*!\brief les valeurs min et max dans l'image pour normaliser si besoin */
static GLushort _min = -1, _max = 0;
/*!\brief booléen pour indiquer la normalisation ou non */
static GLboolean _normalize = GL_FALSE;

/*!\brief créé la fenêtre d'affichage, initialise GL et les données,
 * affecte les fonctions d'événements et lance la boucle principale
 * d'affichage. */
int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "GL4Dummies", 20, 20, 
			 _wW, _wH, GL4DW_RESIZABLE | GL4DW_SHOWN))
    return 1;
  /* si un nom d'image est passé en arg, la charger, sinon charger "images/1.tif" */
  init(argc == 2 ? argv[1] : "images/1.tif");
  atexit(quit);
  gl4duwResizeFunc(resize);
  gl4duwKeyDownFunc(keydown);
  gl4duwDisplayFunc(draw);
  gl4duwMainLoop();
  return 0;
}

/*!\brief initialise les paramètres OpenGL et les données (chargement de l'image). */
static void init(const char * filename) {
  GLuint y, w, h, stride;
  GLushort bitsPerSample, samplesPerPixel; 
  tdata_t buf;
  TIFF * tif = NULL;
  tif = TIFFOpen(filename, "r");
  /* sort si problème de chargement de l'image */
  assert(tif);

  /* générer un identifiant de texture */
  glGenTextures(1, &_texId);
  /* lier l'identifiant de texture comme texture 2D (1D ou 3D
   * possibles) */
  glBindTexture(GL_TEXTURE_2D, _texId);

  /* récupération d'infos sur la tiff et chargement puis transfert vers GL */
  
  TIFFGetField(tif, TIFFTAG_IMAGEWIDTH,      &w); /* largeur de l'image */
  TIFFGetField(tif, TIFFTAG_IMAGELENGTH,     &h); /* hauteur de l'image */
  TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &samplesPerPixel); /* bizarre, ça me met 0 au lieu de 1 (composantes par pixel) */ 
  TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE,   &bitsPerSample); /* bits par composante */
  stride = TIFFScanlineSize(tif); /* taille en octets d'une ligne */
  fprintf(stderr, "infos :\n\tdimensions (%u x %u)\n\ta %hu composantes par pixel\n\ta %hu bits par composante\n",
	  w, h, samplesPerPixel, bitsPerSample);
  /* calculs des ratios pour ne pas déformer l'image à l'affichage */
  if(w < h) {
    _iratio[1] = 1.0f;
    _iratio[0] = w / (GLfloat)h;
  } else {
    _iratio[0] = 1.0f;
    _iratio[1] = h / (GLfloat)w;
  }
  
  buf = _TIFFmalloc(stride * h);
  for(y = 0, _max = 0, _min = -1; y < h; ++y) {
    GLuint i;
    GLushort * line = (GLushort *)&buf[y * stride];
    /* récupération de la ligne depuis le fichier */
    TIFFReadScanline(tif, &buf[y * stride], y, 0);
    /* trouver la plus petite et la plus grande valeur (pour normalisation) */
    for(i = 0; i < w; ++i) {
      if(_min > line[i])
	_min = line[i];
      if(_max < line[i])
	_max = line[i];
    }
  }
  /* envoi de la donnée texture depuis la RAM CPU vers la RAM GPU voir
   * la doc de glTexImage2D sur
   * https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml */
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, w, h, 0, GL_RED, GL_UNSIGNED_SHORT, buf);
  
  _TIFFfree(buf);
  TIFFClose(tif);
  /* paramétrer la texture, voir la doc de la fonction glTexParameter
   * sur
   * https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexParameter.xhtml */
  /* utilisation du mipmapping, si non commenter les 3 lignes et
     décommenter les deux suivantes */
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glGenerateMipmap(GL_TEXTURE_2D);
  /* glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // GL_LINEAR possible aussi */
  /* glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_LINEAR possible aussi */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  /* dé-lier la texture 2D */
  glBindTexture(GL_TEXTURE_2D, 0);

  /* générer un identifiant de texture pour LUT */
  glGenTextures(1, &_lutId);
  /* lier l'identifiant de texture comme texture 1D */
  glBindTexture(GL_TEXTURE_1D, _lutId);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
  glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  GLuint palette[] = { 0, RGB(255, 0, 0), RGB(255, 255, 0), RGB(0, 255, 0), RGB(0, 255, 255), RGB(0, 0, 255), RGB(255, 0, 255) , RGB(255, 255, 255) };
  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, 3, 0, GL_RGBA, GL_UNSIGNED_BYTE, palette);

  /* dé-lier la texture 1D */
  glBindTexture(GL_TEXTURE_1D, 0);

  /* création du quadrilatère sur lequel sera plaquée l'image */
  _quad = gl4dgGenQuadf();
  /* création du programme shader (voir le dossier shader) */
  _pId = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic.fs", NULL);
  /* Set de la couleur (RGBA) d'effacement OpenGL */
  glClearColor(0.0f, 0.0f, 0.2f, 1.0f);
  /* Création des matrices GL4Dummies, une pour la projection, une
   * pour la modélisation-vue */
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  /* régler les proportions de projection et le viewport */
  resize(_wW, _wH);
}

/*!\brief régler la matrice de projection et le viewport OpenGL selon
 * width and height. */
static void resize(int w, int h) {
  _wW  = w; _wH = h;
  /* paramétrer la matrice de projection créée précédemment */
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duOrthof(-1, 1, -_wH/(GLfloat)_wW, _wH/(GLfloat)_wW, 0, 10);
  /* dans quelle partie de l'écran on effectue le rendu */
  glViewport(0, 0, _wW, _wH);
}

/*!\brief Cette fonction dessine dans le contexte OpenGL actif. */
static void draw(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(_pId);
  /* modifier la matrice model-view */
  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();
  gl4duLookAtf(0, 0, 1, 0, 0, 0, 0, 1, 0);
  gl4duScalef(_zoom * _iratio[0], _zoom * _iratio[0], 1.0f);
  gl4duTranslatef(_tx, _ty, 0.0f);
  /* Envoyer toutes les matrices connues dans GL4Dummies */
  gl4duSendMatrices();
  /* activer l'étage de textures 0, plusieurs étages sont disponibles,
   * nous pouvons lier une texture par type et par étage */
  glActiveTexture(GL_TEXTURE0);
  /* lier la texture _texId comme texture 2D */
  glBindTexture(GL_TEXTURE_2D, _texId);
  /* envoyer une info au program shader indiquant que tex est une
   * texture d'étage 0, voir le type (sampler2D) de la variable tex
   * dans le shader */
  glUniform1i(glGetUniformLocation(_pId, "tex"), 0);
  /* envoi d'un booléen pour inverser l'axe y des coordonnées de
   * textures (plus efficace à faire dans le vertex shader */
  glUniform1i(glGetUniformLocation(_pId, "inv"), 1); 
  /* activer l'étage de textures 1 */
  glActiveTexture(GL_TEXTURE1);
  /* lier la texture _lutId comme texture 1D */
  glBindTexture(GL_TEXTURE_1D, _lutId);
  /* envoyer une info au program shader indiquant que lut est une
   * texture d'étage 1, voir le type (sampler1D) de la variable lut
   * dans le shader */
  glUniform1i(glGetUniformLocation(_pId, "lut"), 1);
  /* envoi d'un booléen pour inverser l'axe y des coordonnées de
   * textures (plus efficace à faire dans le vertex shader */
  glUniform1i(glGetUniformLocation(_pId, "inv"), 1); 
  /* envoi d'un booléen pour indiquer si on utilise la LUT ou pas */
  glUniform1i(glGetUniformLocation(_pId, "useLUT"), _useLUT); 
  /* envoi d'un booléen pour indiquer si on souhaite normaliser ou pas */
  glUniform1i(glGetUniformLocation(_pId, "normalize"), _normalize); 
  /* envoi des données de normalisation (converties dans [0, 1]) */
  glUniform1f(glGetUniformLocation(_pId, "minv"), _min / 65535.0f); 
  glUniform1f(glGetUniformLocation(_pId, "maxv"), _max / 65535.0f); 
  /* dessiner le quadrilatère */
  gl4dgDraw(_quad);
  /* désactiver le programme shader */
  glUseProgram(0);
}

/*!\brief manages keyboard-down event */
static void keydown(int keycode) {
  switch(keycode) {
  case GL4DK_r:
    _zoom = 1.0f; _tx = 0; _ty = 0;
    break;
  case GL4DK_UP:
    _ty -= 0.05f / _zoom;
    break;
  case GL4DK_DOWN:
    _ty += 0.05f / _zoom;
    break;
  case GL4DK_RIGHT:
    _tx -= 0.05f / _zoom;
    break;
  case GL4DK_LEFT:
    _tx += 0.05f / _zoom;
    break;
  case GL4DK_z:
    _zoom += 0.05;
    break;
  case GL4DK_d:
    if(_zoom > 0.5f)
      _zoom -= 0.05;
    break;
  case GL4DK_l:
    _useLUT = !_useLUT;
    break;
  case GL4DK_n:
    _normalize = !_normalize;
    break;
  case GL4DK_ESCAPE:
  case 'q':
    exit(0);
  default:
    break;
  }
}

/*!\brief appelée au moment de sortir du programme (atexit), elle
 *  libère les éléments OpenGL utilisés. */
static void quit(void) {
  /* suppression de la texture _texId en GPU */
  if(_texId) {
    glDeleteTextures(1, &_texId);
    _texId = 0;
  }
  /* nettoyage des éléments utilisés par la bibliothèque GL4Dummies */
  gl4duClean(GL4DU_ALL);
}
