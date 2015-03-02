AM_CPPFLAGS = -I../../lib_src/GL4D `sdl2-config --cflags`
AM_LDFLAGS  = -L$(top_builddir)/bin/ -lGL4Dummies `sdl2-config --libs`

bin_PROGRAMS = $(top_builddir)/bin/gl4dObjLoader

__top_builddir__bin_gl4dObjLoader_includedir = glm
__top_builddir__bin_gl4dObjLoader_include_HEADERS = glm/glm.h	\
glm/glmint.h
__top_builddir__bin_gl4dObjLoader_SOURCES = objLoader.c glm/glm.c	\
glm/glmimg.c glm/glmimg_devil.c glm/glmimg_jpg.c glm/glmimg_png.c	\
glm/glmimg_sdl.c glm/glmimg_sim.c glm/glm_util.c

clean-local:
	rm -f *~
