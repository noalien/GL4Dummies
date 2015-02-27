AM_CPPFLAGS = -I../../lib_src/GL4D `sdl2-config --cflags`
AM_LDFLAGS  = -L$(top_builddir) -lGL4Dummies `sdl2-config --libs` $(AM_ADDITIONAL)

bin_PROGRAMS = $(top_builddir)/gl4dWalkOnPolygons

__top_builddir__gl4dWalkOnPolygons_SOURCES = walkOnPolygons.c

clean-local:
	rm -f *~
