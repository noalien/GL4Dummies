AM_CPPFLAGS = -I../../lib_src $(SDL_CFLAGS) 
AM_LDFLAGS  = -L$(top_builddir)/bin -lGL4Dummies $(SDL_LIBS)

bin_PROGRAMS = $(top_builddir)/bin/gl4dDemo

__top_builddir__bin_gl4dDemo_SOURCES = landscape.c

clean-local:
	rm -f *~
