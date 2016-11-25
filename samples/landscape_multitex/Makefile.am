AM_CPPFLAGS = -I../../lib_src  $(SDL_CFLAGS)
AM_LDFLAGS  = -L$(top_builddir)/bin -lGL4Dummies $(SDL_LIBS) -lpng

#Ancienne version
#AM_CPPFLAGS = -O0 -g -I/opt/local/include/ -I../../lib_src  $(SDL_CFLAGS) 
#AM_LDFLAGS  = -L$(top_builddir)/bin -lGL4Dummies -lm -lpng $(SDL_LIBS) -lGLU -lGL

bin_PROGRAMS = $(top_builddir)/bin/landscape_multitex

__top_builddir__bin_landscape_multitex_SOURCES = png_export.c hm_view.c dsquare.c

clean-local:
	rm -f *~
