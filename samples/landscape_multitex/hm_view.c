/*!\file hm_view.c
 *
 * \brief Visualisation d'un terrain généré
 * + calculs (normales, tangentes, camera..)
 * \author Nicolas HENRIQUES, henriques.nicolas@hotmail.fr
 *
 * \date August 19 2016
 */
#include "hm_view.h"

/*
 * Prototypes des fonctions statiques importantes contenues dans ce fichier C
 */
static void ark_cross(float * vec1, float * vec2, float * cross_vec);
static void ark_normalize(float * vec);
static SDL_Window * initWindow(int w, int h, SDL_GLContext * poglContext);
static void quit(void);
static void initGL(SDL_Window * win);
static void initData(float ** tab, int taille);
static void resizeGL(SDL_Window * win);
static void loop(SDL_Window * win, float** tab, int taille, float min_height, float max_height);
static void manageEvents(SDL_Window * win);
static void draw(int taille, float min_height, float max_height);


/*!\brief pointeur vers la (future) fenêtre SDL */
static SDL_Window * _win = NULL;
static int _windowWidth = 1920, _windowHeight = 1080;
/*!\brief pointeur vers le (futur) contexte OpenGL */
static SDL_GLContext _oglContext = NULL;

static GLuint _squarevao = 0;
static GLuint _squarebuffer = 0;

static GLuint * _vao = 0;
static GLuint * _buffer = 0;
static GLuint * _buffer_norm = 0;
static GLuint * _buffer_tex = 0;
static GLuint * _buffer_tan = 0;
static GLuint * _buffer_bitan = 0;

static GLuint _pId = 0;
static GLuint * _tId = 0;
static GLuint _pause = 0;
static GLuint _terrain_type = 0;

static GLfloat _y_scaling = 1;

enum kyes_t {
	KLEFT = 0,
	KRIGHT,
	KUP,
	KDOWN,

	KADD = 4,        //P
	KSUBSTRACT = 5,  //M

	KLINE = 6,   //L

	KF = 7,
	KG = 8,
	KT = 9,
	KV = 10,

	KTYPE = 11 //A
};

static GLuint _keys[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

typedef struct cam_t cam_t;
struct cam_t {
	GLfloat right[3];
	GLfloat up[3];
	GLfloat position[3];
	GLfloat direction[3];
	GLfloat angleWidth;
	GLfloat angleHeight;
};
cam_t _cam;

static GLfloat lumpos[] = {0, 40, 0, 0};





void view(float ** tab, int taille, float min_height, float max_height, int nb_args, char** args) {

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Erreur lors de l'initialisation de SDL :  %s", SDL_GetError());
		return;
	}
	atexit(SDL_Quit);
	gl4duInit(nb_args, args);
	SDL_SetRelativeMouseMode(SDL_TRUE);
	if ((_win = initWindow(_windowWidth, _windowHeight, &_oglContext))) {
		initGL(_win);
		_pId = gl4duCreateProgram("<vs>bin/share/GL4Dummies/shaders/landscape_multitex.vs", "<fs>bin/share/GL4Dummies/shaders/landscape_multitex.fs", NULL);
		loop(_win, tab, taille, min_height, max_height);
	} else
		fprintf(stderr, "Erreur lors de la creation de la fenetre\n");
	return;
}

static void quit(void) {
	// if (_tId)
	// 	glDeleteTextures(1, &_tId);
	// if (_vao)
	// 	glDeleteVertexArrays(1, &_vao);
	// if (_buffer)
	// 	glDeleteBuffers(1, &_buffer);
	if (_oglContext)
		SDL_GL_DeleteContext(_oglContext);
	if (_win)
		SDL_DestroyWindow(_win);
	gl4duClean(GL4DU_ALL);
}

static SDL_Window * initWindow(int w, int h, SDL_GLContext * poglContext) {
	SDL_Window * win = NULL;
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	// if ( (win = SDL_CreateWindow("Fenetre GL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
	//                              w, h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE |
	//                              SDL_WINDOW_SHOWN)) == NULL )
	if ( (win = SDL_CreateWindow("Fenetre GL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
	                             w, h, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN)) == NULL )
		return NULL;
	if ( (*poglContext = SDL_GL_CreateContext(win)) == NULL ) {
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
	glEnable(GL_CULL_FACE);
	gl4duGenMatrix(GL_FLOAT, "modelMatrix");
	gl4duGenMatrix(GL_FLOAT, "viewMatrix");
	gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
	resizeGL(win);
}

static void ark_cross(float * vec1, float * vec2, float * cross_vec) {

	cross_vec[0] = vec1[1] * vec2[2] - vec1[2] * vec2[1];
	cross_vec[1] = vec1[2] * vec2[0] - vec1[0] * vec2[2];
	cross_vec[2] = vec1[0] * vec2[1] - vec1[1] * vec2[0];
}

static void ark_normalize(float * vec) {

	float norm_factor = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
	vec[0] = -vec[0] / norm_factor;
	vec[1] = -vec[1] / norm_factor;
	vec[2] = -vec[2] / norm_factor;
}


static void initData(float ** tab, int taille) {

	_cam.angleWidth = 0;
	_cam.angleHeight = M_PI;

	char temp[8][BUFSIZ], * fn[8];
	SDL_Surface * texSurface[8];

	fn[0] = "bin/share/GL4Dummies/images/sdirt.bmp";
	fn[1] = "bin/share/GL4Dummies/images/sgrass.bmp";
	fn[2] = "bin/share/GL4Dummies/images/srock.bmp";

	fn[3] = "bin/share/GL4Dummies/images/sdirt2.bmp";
	fn[4] = "bin/share/GL4Dummies/images/sgrass2.bmp";
	fn[5] = "bin/share/GL4Dummies/images/srock2.bmp";

	fn[6] = "bin/share/GL4Dummies/images/ssand.bmp";
	fn[7] = "bin/share/GL4Dummies/images/ssnow.bmp";


	int it1;
	int it2;

	GLfloat squaredata[3 * 4] = {
		-1.0,  0.0, -1.0,
		-1.0,  0.0,  1.0,
		1.0,  0.0, -1.0,
		1.0,  0.0,  1.0
	};

	glGenVertexArrays(1, &_squarevao);
	glBindVertexArray(_squarevao);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &_squarebuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _squarebuffer);
	glBufferData(GL_ARRAY_BUFFER, 3 * 4 * sizeof(GLfloat), squaredata, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDisableVertexAttribArray(0);
	glBindVertexArray(0);


	_vao = malloc((taille - 1) * sizeof(GLuint));
	_buffer = malloc((taille - 1) * sizeof(GLuint));
	_buffer_norm = malloc((taille - 1) * sizeof(GLuint));
	_buffer_tex = malloc((taille - 1) * sizeof(GLuint));
	_buffer_tan = malloc((taille - 1) * sizeof(GLuint));
	_buffer_bitan = malloc((taille - 1) * sizeof(GLuint));


	/*coordonnées de sommets */ //on les garde toutes pour calculer les normales
	GLfloat * data = malloc((taille - 1) * taille * 2 * 3 * sizeof(float));
	/*coordonnées de texture, une par sommet */
	GLfloat * data_tex = malloc(/*(taille - 1) **/ taille * 2 * 2 * sizeof(float));
	/*normales*/
	GLfloat * data_norm = malloc(/*(taille - 1) **/ taille * 2 * 3 * sizeof(float));
	/*tangente */
	GLfloat * data_tan = malloc(/*(taille - 1) **/ taille * 2 * 3 * sizeof(float));
	/*bitangente */
	GLfloat * data_bitan = malloc(/*(taille - 1) **/ taille * 2 * 3 * sizeof(float));


	for (it1 = 0; it1 < taille - 1; ++it1) {

		for (it2 = 0; it2 < taille * 2; ++it2) {

			data[it2 * 3 + it1 * taille * 3 * 2    ] = it2 / 2;
			data[it2 * 3 + it1 * taille * 3 * 2 + 1] = (taille / 200) > 0 ? (float)taille / 200 * tab[it2 / 2][it1 + it2 % 2] : 1 * tab[it2 / 2][it1 + it2 % 2];
			data[it2 * 3 + it1 * taille * 3 * 2 + 2] = it1 + it2 % 2;
			//printf("x:%d   y:tab[%d][%d]    z:%d\n", it2 / 2, it2 / 2, it1 + it2 % 2, it1 + it2 % 2);
			data_tex[it2 * 2 /*+ it1 * taille * 2 * 2*/    ] = (float)(it2 / 2) / taille * 10;
			data_tex[it2 * 2 /*+ it1 * taille * 2 * 2*/ + 1] = (float)(it1 + it2 % 2) / taille * 10;
		}

		glGenVertexArrays(1, &_vao[it1]);
		glBindVertexArray(_vao[it1]);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(2);

		glGenBuffers(1, &_buffer[it1]);
		glGenBuffers(1, &_buffer_tex[it1]);
		glBindBuffer(GL_ARRAY_BUFFER, _buffer[it1]);
		glBufferData(GL_ARRAY_BUFFER, (taille - 1) * taille * 2 * 3 * sizeof(GLfloat), data, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const void *)(it1 * taille * 3 * 2 * sizeof * data));
		glBindBuffer(GL_ARRAY_BUFFER, _buffer_tex[it1]);
		glBufferData(GL_ARRAY_BUFFER, /*(taille - 1) **/ taille * 2 * 2 * sizeof(GLfloat), data_tex, GL_STATIC_DRAW);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (const void *)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(0);
		glBindVertexArray(0);
	}


	//NORMALES
	int tt0, tt1, tt2, tt3, nb_norm;
	float t0[3], t1[3], t2[3], t3[3];
	float edge1[3], edge2[3], deltaUV1[2], deltaUV2[2], frac_part;
	float norm0[3], norm1[3], norm2[3], norm3[3];

	for (it1 = 0; it1 < taille - 1; ++it1) {
		for (it2 = 0; it2 < taille * 2; ++it2) {

			tt0 = 0; tt1 = 0; tt2 = 0; tt3 = 0; nb_norm = 0;

			//ligne 1
			if (it2 > 0) {

				tt0 = 1;
				t0[0] = data[it2 * 3 + it1 * taille * 3 * 2    ] - data[it2 * 3 + it1 * taille * 3 * 2 - 6];
				t0[1] = data[it2 * 3 + it1 * taille * 3 * 2 + 1] - data[it2 * 3 + it1 * taille * 3 * 2 - 5];
				t0[2] = data[it2 * 3 + it1 * taille * 3 * 2 + 2] - data[it2 * 3 + it1 * taille * 3 * 2 - 4];
			}
			//colonne 1
			if (it1 + it2 % 2 > 1) {

				tt1 = 1;
				t1[0] = data[it2 * 3 + it1 * taille * 3 * 2    ] - data[it2 * 3 + (it1 - 1) * taille * 3 * 2    ];
				t1[1] = data[it2 * 3 + it1 * taille * 3 * 2 + 1] - data[it2 * 3 + (it1 - 1) * taille * 3 * 2 + 1];
				t1[2] = data[it2 * 3 + it1 * taille * 3 * 2 + 2] - data[it2 * 3 + (it1 - 1) * taille * 3 * 2 + 2];
			}
			//ligne 2
			if (it2 < taille * 2 - 1) {

				tt2 = 1;
				t2[0] = data[it2 * 3 + it1 * taille * 3 * 2    ] - data[it2 * 3 + it1 * taille * 3 * 2 + 6];
				t2[1] = data[it2 * 3 + it1 * taille * 3 * 2 + 1] - data[it2 * 3 + it1 * taille * 3 * 2 + 7];
				t2[2] = data[it2 * 3 + it1 * taille * 3 * 2 + 2] - data[it2 * 3 + it1 * taille * 3 * 2 + 8];
			}

			//colonne 2
			if (it1 + it2 % 2 < taille - 2) {

				tt3 = 1;
				t3[0] = data[it2 * 3 + it1 * taille * 3 * 2    ] - data[it2 * 3 + (it1 + 1) * taille * 3 * 2    ];
				t3[1] = data[it2 * 3 + it1 * taille * 3 * 2 + 1] - data[it2 * 3 + (it1 + 1) * taille * 3 * 2 + 1];
				t3[2] = data[it2 * 3 + it1 * taille * 3 * 2 + 2] - data[it2 * 3 + (it1 + 1) * taille * 3 * 2 + 2];
			}


			if (tt0 && tt1) {

				nb_norm++;
				ark_cross(t0, t1, norm0);
				ark_normalize(norm0);
			}

			if (tt1 && tt2) {

				nb_norm++;
				ark_cross(t1, t2, norm1);
				ark_normalize(norm1);
			}

			if (tt2 && tt3) {

				nb_norm++;
				ark_cross(t2, t3, norm2);
				ark_normalize(norm2);
			}

			if (tt3 && tt0) {

				nb_norm++;
				ark_cross(t3, t0, norm3);
				ark_normalize(norm3);
			}

			if (nb_norm == 0) {

				printf("probleme d algo, divison par 0 impossible\n");
				return;
			}

			data_norm[it2 * 3 /*+ it1 * taille * 3 * 2*/    ] = (norm0[0] * (int)((tt0 + tt1) / 2) + norm1[0] * (int)((tt1 + tt2) / 2) + norm2[0] * (int)((tt2 + tt3) / 2) + norm3[0] * (int)((tt3 + tt0) / 2)) / nb_norm;
			data_norm[it2 * 3 /*+ it1 * taille * 3 * 2*/ + 1] = (norm0[1] * (int)((tt0 + tt1) / 2) + norm1[1] * (int)((tt1 + tt2) / 2) + norm2[1] * (int)((tt2 + tt3) / 2) + norm3[1] * (int)((tt3 + tt0) / 2)) / nb_norm;
			data_norm[it2 * 3 /*+ it1 * taille * 3 * 2*/ + 2] = (norm0[2] * (int)((tt0 + tt1) / 2) + norm1[2] * (int)((tt1 + tt2) / 2) + norm2[2] * (int)((tt2 + tt3) / 2) + norm3[2] * (int)((tt3 + tt0) / 2)) / nb_norm;

			// printf("data     : %f %f %f\n", data[it2 * 3 + it1 * taille * 3 * 2    ], data[it2 * 3 + it1 * taille * 3 * 2 + 1], data[it2 * 3 + it1 * taille * 3 * 2 + 2]);
			// printf("data_norm: %f %f %f\n\n", data_norm[it2 * 3 + it1 * taille * 3 * 2    ], data_norm[it2 * 3 + it1 * taille * 3 * 2 + 1], data_norm[it2 * 3 + it1 * taille * 3 * 2 + 2]);

			//test
			// data_norm[it2 * 3 + it1 * taille * 3 * 2] = 0;
			// data_norm[it2 * 3 + it1 * taille * 3 * 2 + 1] = 1;
			// data_norm[it2 * 3 + it1 * taille * 3 * 2 + 2] = 0;

			//POUR NORMAL MAPPING
			edge1[0] = data[it2 * 3 + it1 * taille * 3 * 2 + 3] - data[it2 * 3 + it1 * taille * 3 * 2    ];
			edge1[1] = data[it2 * 3 + it1 * taille * 3 * 2 + 4] - data[it2 * 3 + it1 * taille * 3 * 2 + 1];
			edge1[2] = data[it2 * 3 + it1 * taille * 3 * 2 + 5] - data[it2 * 3 + it1 * taille * 3 * 2 + 2];

			edge2[0] = data[it2 * 3 + it1 * taille * 3 * 2 + 6] - data[it2 * 3 + it1 * taille * 3 * 2    ];
			edge2[1] = data[it2 * 3 + it1 * taille * 3 * 2 + 7] - data[it2 * 3 + it1 * taille * 3 * 2 + 1];
			edge2[2] = data[it2 * 3 + it1 * taille * 3 * 2 + 8] - data[it2 * 3 + it1 * taille * 3 * 2 + 2];

			deltaUV1[0] = data_tex[it2 * 2 /*+ it1 * taille * 2 * 2*/ + 2] - data_tex[it2 * 2 /*+ it1 * taille * 2 * 2*/    ];
			deltaUV1[1] = data_tex[it2 * 2 /*+ it1 * taille * 2 * 2*/ + 3] - data_tex[it2 * 2 /*+ it1 * taille * 2 * 2*/ + 1];

			deltaUV2[0] = data_tex[it2 * 2 /*+ it1 * taille * 2 * 2*/ + 4] - data_tex[it2 * 2 /*+ it1 * taille * 2 * 2*/    ];
			deltaUV2[1] = data_tex[it2 * 2 /*+ it1 * taille * 2 * 2*/ + 5] - data_tex[it2 * 2 /*+ it1 * taille * 2 * 2*/ + 1];

			frac_part = 1.0f / (deltaUV1[0] * deltaUV2[1] - deltaUV2[0] * deltaUV1[1]);

			data_tan[it2 * 3 /*+ it1 * taille * 3 * 2*/    ] = frac_part * (deltaUV2[1] * edge1[0] - deltaUV1[1] * edge2[0]);
			data_tan[it2 * 3 /*+ it1 * taille * 3 * 2*/ + 1] = frac_part * (deltaUV2[1] * edge1[1] - deltaUV1[1] * edge2[1]);
			data_tan[it2 * 3 /*+ it1 * taille * 3 * 2*/ + 2] = frac_part * (deltaUV2[1] * edge1[2] - deltaUV1[1] * edge2[2]);
			ark_normalize(data_tan);

			data_bitan[it2 * 3 /*+ it1 * taille * 3 * 2*/    ] = frac_part * (-deltaUV2[0] * edge1[0] + deltaUV1[0] * edge2[0]);
			data_bitan[it2 * 3 /*+ it1 * taille * 3 * 2*/ + 1] = frac_part * (-deltaUV2[0] * edge1[1] + deltaUV1[0] * edge2[1]);
			data_bitan[it2 * 3 /*+ it1 * taille * 3 * 2*/ + 2] = frac_part * (-deltaUV2[0] * edge1[2] + deltaUV1[0] * edge2[2]);
			ark_normalize(data_bitan);

		}

		glBindVertexArray(_vao[it1]);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);

		glGenBuffers(1, &_buffer_norm[it1]);
		glGenBuffers(1, &_buffer_tan[it1]);
		glGenBuffers(1, &_buffer_bitan[it1]);
		glBindBuffer(GL_ARRAY_BUFFER, _buffer_norm[it1]);
		glBufferData(GL_ARRAY_BUFFER, /*(taille - 1) **/ taille * 2 * 3 * sizeof(GLfloat), data_norm, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE,  0, (const void *)0);
		glBindBuffer(GL_ARRAY_BUFFER, _buffer_tan[it1]);
		glBufferData(GL_ARRAY_BUFFER, /*(taille - 1) **/ taille * 2 * 3 * sizeof(GLfloat), data_tan, GL_STATIC_DRAW);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
		glBindBuffer(GL_ARRAY_BUFFER, _buffer_bitan[it1]);
		glBufferData(GL_ARRAY_BUFFER, /*(taille - 1) **/ taille * 2 * 3 * sizeof(GLfloat), data_bitan, GL_STATIC_DRAW);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDisableVertexAttribArray(4);
		glDisableVertexAttribArray(3);
		glDisableVertexAttribArray(1);
		glBindVertexArray(0);

	}

	//SEPARER LES triangle_strip (lignes diagonales qui relient chauqe lignes)

	_tId = malloc(8 * sizeof(GLuint));


	for (it1 = 0; it1 < 8; ++it1) {

		glActiveTexture(GL_TEXTURE0 + it1);
		glGenTextures(1, &_tId[it1]);
		glBindTexture(GL_TEXTURE_2D, _tId[it1]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		gl4duMakeBinRelativePath(temp[it1], sizeof temp[it1], fn[it1]);
		if ( (texSurface[it1] = SDL_LoadBMP(fn[it1])) == NULL && (texSurface[it1] = SDL_LoadBMP(temp[it1])) == NULL) {
			fprintf(stderr, "Impossible d'ouvrir le fichier %s : %s\n", fn[it1], SDL_GetError());
			exit(1);
		}
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texSurface[it1]->w, texSurface[it1]->h, 0, GL_BGR, GL_UNSIGNED_BYTE, texSurface[it1]->pixels);
		SDL_FreeSurface(texSurface[it1]);
		glBindTexture(GL_TEXTURE_2D, 0);

	}
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
static void loop(SDL_Window * win, float ** tab, int taille, float min_height, float max_height) {
	GLfloat dt = 0.0f, dtheta = (GLfloat)GL4DM_PI, pas = 150.0f;
	Uint32 t0 = SDL_GetTicks(), t;
	SDL_GL_SetSwapInterval(1);

	initData(tab, taille);

	for (;;) {
		dt = ((t = SDL_GetTicks()) - t0) / 1000.0f;
		t0 = t;
		manageEvents(win);


		if (_keys[KLEFT]) {
			_cam.position[0] -= _cam.right[0] * dt * pas * dtheta;
			_cam.position[1] -= _cam.right[1] * dt * pas * dtheta;
			_cam.position[2] -= _cam.right[2] * dt * pas * dtheta;
		}
		if (_keys[KRIGHT]) {
			_cam.position[0] += _cam.right[0] * dt * pas * dtheta;
			_cam.position[1] += _cam.right[1] * dt * pas * dtheta;
			_cam.position[2] += _cam.right[2] * dt * pas * dtheta;
		}
		if (_keys[KUP]) {
			_cam.position[0] += _cam.direction[0] * dt * pas * dtheta;
			_cam.position[1] += _cam.direction[1] * dt * pas * dtheta;
			_cam.position[2] += _cam.direction[2] * dt * pas * dtheta;
		}
		if (_keys[KDOWN]) {
			_cam.position[0] -= _cam.direction[0] * dt * pas * dtheta;
			_cam.position[1] -= _cam.direction[1] * dt * pas * dtheta;
			_cam.position[2] -= _cam.direction[2] * dt * pas * dtheta;
		}

		if (_keys[KF]) {
			lumpos[0] -= 0.1;
		}
		if (_keys[KG]) {
			lumpos[0] += 0.1;
		}
		if (_keys[KT]) {
			lumpos[2] -= 0.1;
		}
		if (_keys[KV]) {
			lumpos[2] += 0.1;
		}

		if (_keys[KADD]) {
			_y_scaling += dt * 30;
		}

		if (_keys[KSUBSTRACT]) {
			_y_scaling -= dt * 30;
			if (_y_scaling < 0.1)
				_y_scaling = 0.1;
		}

		if (_keys[KLINE]) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		draw(taille, min_height, max_height);
		gl4duPrintFPS(stderr);
		SDL_GL_SwapWindow(win);
		gl4duUpdateShaders();
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
	while (SDL_PollEvent(&event))
		switch (event.type) {
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
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

			case SDLK_f:
				_keys[KF] = 1;
				break;
			case SDLK_g:
				_keys[KG] = 1;
				break;
			case SDLK_t:
				_keys[KT] = 1;
				break;
			case SDLK_v:
				_keys[KV] = 1;
				break;

			case SDLK_p:
				_keys[KADD] = 1;
				break;
			case SDLK_m:
				_keys[KSUBSTRACT] = 1;
				break;
			case SDLK_a:
				if (_keys[KTYPE] == 0) {
					_keys[KTYPE] = 1;
					_terrain_type++;
					if (_terrain_type == 4)
						_terrain_type = 0;
				}
				break;
			case SDLK_l:
				if (_keys[KLINE])
					_keys[KLINE] = 0;
				else
					_keys[KLINE] = 1;
				break;
			case SDLK_ESCAPE:
			case 'q':
				exit(0);
			case SDLK_SPACE:
				_pause = !_pause;
				break;
			default:
				fprintf(stderr, "La touche %s a ete pressee\n",
				        SDL_GetKeyName(event.key.keysym.sym));
				break;
			}
			break;
		case SDL_KEYUP:
			switch (event.key.keysym.sym) {
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

			case SDLK_f:
				_keys[KF] = 0;
				break;
			case SDLK_g:
				_keys[KG] = 0;
				break;
			case SDLK_t:
				_keys[KT] = 0;
				break;
			case SDLK_v:
				_keys[KV] = 0;
				break;

			case SDLK_p:
				_keys[KADD] = 0;
				break;
			case SDLK_m:
				_keys[KSUBSTRACT] = 0;
				break;
			case SDLK_a:
				_keys[KTYPE] = 0;
				break;
			}
			break;
		case SDL_WINDOWEVENT:
			if (event.window.windowID == SDL_GetWindowID(win)) {
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
static void draw(int taille, float min_height, float max_height) {
	static int xm = 0, ym = 0;
	char value[10];
	char texture[11] = "myTexture\0";
	int xmr, ymr, it1;
	SDL_PumpEvents();
	SDL_GetRelativeMouseState(&xmr, &ymr);
	xm += xmr; ym += ymr;

	_cam.angleWidth = (float) - xm / 200;
	_cam.angleHeight = (float) - ym / 200;
	// if (_cam.angleHeight < M_PI - M_PI / 2)
	// 	_cam.angleHeight = M_PI - M_PI / 2;
	// if (_cam.angleHeight > M_PI + M_PI / 2)
	// 	_cam.angleHeight = M_PI + M_PI / 2;

	// printf("%f\n", _cam.angleHeight);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(_pId);

	// printf("type=%d", _terrain_type);

	//texturing selon la pente
	//2 samples
	if (_terrain_type == 0) {
		for (it1 = 0; it1 < 3; ++it1) {
			glActiveTexture(GL_TEXTURE0 + it1);
			glBindTexture(GL_TEXTURE_2D, _tId[it1]);
			sprintf(value, "%d", it1);
			strcat(texture, value);
			glUniform1i(glGetUniformLocation(_pId, texture), it1);
			// printf("%s\n", texture);
			texture[9] = '\0';
		}
	}
	else if (_terrain_type == 1) {
		for (it1 = 3; it1 < 6; ++it1) {
			glActiveTexture(GL_TEXTURE0 + it1);
			glBindTexture(GL_TEXTURE_2D, _tId[it1]);
			sprintf(value, "%d", it1 - 3);
			strcat(texture, value);
			glUniform1i(glGetUniformLocation(_pId, texture), it1);
			// printf("%s\n", texture);
			texture[9] = '\0';
		}
	}
	//texturing selon la hauteur
	//2 samples
	else if (_terrain_type == 2) {
		for (it1 = 0; it1 < 3; ++it1) {
			glActiveTexture(GL_TEXTURE0 + it1);
			glBindTexture(GL_TEXTURE_2D, _tId[it1]);
			sprintf(value, "%d", it1);
			strcat(texture, value);
			glUniform1i(glGetUniformLocation(_pId, texture), it1);
			// printf("%s\n", texture);
			texture[9] = '\0';
		}
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, _tId[6]);
		sprintf(value, "%d", 3);
		strcat(texture, value);
		glUniform1i(glGetUniformLocation(_pId, texture), 6);
		// printf("%s\n", texture);
		texture[9] = '\0';

		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, _tId[7]);
		sprintf(value, "%d", 4);
		strcat(texture, value);
		glUniform1i(glGetUniformLocation(_pId, texture), 7);
		// printf("%s\n", texture);
		texture[9] = '\0';
	}
	else if (_terrain_type == 3) {
		for (it1 = 3; it1 < 6; ++it1) {
			glActiveTexture(GL_TEXTURE0 + it1);
			glBindTexture(GL_TEXTURE_2D, _tId[it1]);
			sprintf(value, "%d", it1 - 3);
			strcat(texture, value);
			glUniform1i(glGetUniformLocation(_pId, texture), it1);
			// printf("%s\n", texture);
			texture[9] = '\0';
		}
		glActiveTexture(GL_TEXTURE8);
		glBindTexture(GL_TEXTURE_2D, _tId[2]);
		sprintf(value, "%d", 3);
		strcat(texture, value);
		glUniform1i(glGetUniformLocation(_pId, texture), 8);
		// printf("%s\n", texture);
		texture[9] = '\0';
		glActiveTexture(GL_TEXTURE9);
		glBindTexture(GL_TEXTURE_2D, _tId[7]);
		sprintf(value, "%d", 4);
		strcat(texture, value);
		glUniform1i(glGetUniformLocation(_pId, texture), 9);
		// printf("%s\n", texture);
		texture[9] = '\0';
	}
	else
		printf("t es pas au bon endroit mon coco\n");

	glUniform1i(glGetUniformLocation(_pId, "hm"), 1);
	glUniform1i(glGetUniformLocation(_pId, "type"), _terrain_type);

	glUniform1f(glGetUniformLocation(_pId, "min_h"), min_height);
	glUniform1f(glGetUniformLocation(_pId, "max_h"), max_height);

	gl4duBindMatrix("modelMatrix");
	gl4duLoadIdentityf();
	/* Avec des rotate et translate faire :
	   gl4duRotatef(-_cam.theta * 180.0f / GL4DM_PI, 0.0, 1.0, 0.0);
	   gl4duTranslatef(-_cam.x, -1.0, -_cam.z);
	   A la place du LookAt */
	gl4duBindMatrix("viewMatrix");
	gl4duLoadIdentityf();

	_cam.direction[0] = cos(_cam.angleHeight) * sin(_cam.angleWidth);
	_cam.direction[1] = sin(_cam.angleHeight);
	_cam.direction[2] = cos(_cam.angleHeight) * cos(_cam.angleWidth);

	_cam.right[0] = sin(_cam.angleWidth - M_PI / 2.0f);
	_cam.right[1] = 0;
	_cam.right[2] = cos(_cam.angleWidth - M_PI / 2.0f);

	ark_cross(_cam.direction, _cam.right, _cam.up);

	gl4duLookAtf(_cam.position[0], _cam.position[1], _cam.position[2], _cam.position[0] + _cam.direction[0], _cam.position[1] + _cam.direction[1], _cam.position[2] + _cam.direction[2], 0.0, 1.0, 0.0);

	glUniform3fv(glGetUniformLocation(_pId, "lumpos"), 1, lumpos);
	glUniform3fv(glGetUniformLocation(_pId, "viewpos"), 1, _cam.position);

	gl4duScalef(1.0f, _y_scaling, 1.0f);
	gl4duSendMatrices();

	for (it1 = 0; it1 < taille - 1; ++it1) {

		glBindVertexArray(_vao[it1]);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, /*(taille - 1) **/ taille * 2);

		glDisableVertexAttribArray(4);
		glDisableVertexAttribArray(3);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(0);
		glBindVertexArray(0);
	}

	glUniform1i(glGetUniformLocation(_pId, "hm"), 0);

	gl4duBindMatrix("modelMatrix");
	gl4duLoadIdentityf();
	gl4duTranslatef(lumpos[0], lumpos[1], lumpos[2]);
	gl4duScalef(5.0f, 5.0f, 5.0f);

	gl4duSendMatrices();



	glBindVertexArray(_squarevao);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glDisableVertexAttribArray(0);
	glBindVertexArray(0);

	// gl4duPushMatrix();
	// gl4duTranslatef(2, 1, -7);
	// gl4duRotatef(90, 1.0, 0.0, 0.0);
	// gl4duRotatef(-100.0f * a0, 0.0, 0.0, 1.0);
	// gl4duScalef(0.1f, 0.1f, 0.1f);

	// gl4duSendMatrix();
	// glBindVertexArray(_vao);
	// glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	// glBindVertexArray(0);
	// gl4duPopMatrix();

	// gl4duPushMatrix();
	// gl4duTranslatef(-2, 2, -8);
	// gl4duRotatef(70, 1.0, 0.0, 0.0);
	// gl4duRotatef(20.0f * a0, 0.0, 0.0, 1.0);
	// gl4duScalef(0.1f, 0.1f, 0.2f);
	// glUniformMatrix4fv(glGetUniformLocation(_pId, "modelViewMatrix"), 1, GL_TRUE, gl4duGetMatrixData());
	// glBindVertexArray(_vao);
	// glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	// glBindVertexArray(0);
	// gl4duPopMatrix();
}

