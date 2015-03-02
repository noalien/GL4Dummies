AM_CPPFLAGS = -I../../lib_src/GL4D `sdl2-config --cflags`
AM_LDFLAGS  = -L$(top_builddir)/bin -lGL4Dummies `sdl2-config --libs`

bin_PROGRAMS = $(top_builddir)/bin/gl4dLandscape

__top_builddir__bin_gl4dLandscape_SOURCES = landscape.c

clean-local:
	rm -f *~
