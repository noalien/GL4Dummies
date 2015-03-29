AM_CPPFLAGS = -I../../lib_src $(SDL_CFLAGS)
AM_LDFLAGS  = -L$(top_builddir)/bin/ -lGL4Dummies $(SDL_LIBS)

bin_PROGRAMS = $(top_builddir)/bin/gl4dObjLoader

#__top_builddir__bin_gl4dObjLoader_includedir = $(includedir)/glm
#__top_builddir__bin_gl4dObjLoader_include_HEADERS = glm/glm.h	\
glm/glmint.h

EXTRA_DIST = glm/glm.h glm/glmint.h

__top_builddir__bin_gl4dObjLoader_SOURCES = objLoader.c glm/glm.c	\
glm/glmimg.c glm/glmimg_devil.c glm/glmimg_jpg.c glm/glmimg_png.c	\
glm/glmimg_sdl.c glm/glmimg_sim.c glm/glm_util.c

clean-local:
	rm -f *~
