AM_CPPFLAGS = -std=c99 -I../../lib_src $(SDL_CFLAGS)
AM_LDFLAGS  = -L$(top_builddir)/bin/ -lGL4Dummies $(SDL_LIBS)

bin_PROGRAMS = $(top_builddir)/bin/gl4dContainer

__top_builddir__bin_gl4dContainer_SOURCES = container.c

clean-local:
	rm -f *~
