AM_CPPFLAGS = -IGL4D -I. `sdl2-config --cflags`

lib_LTLIBRARIES = $(top_builddir)/bin/libGL4Dummies.la

__top_builddir__bin_libGL4Dummiesincludedir = $(includedir)/GL4D
__top_builddir__bin_libGL4Dummiesinclude_HEADERS = GL4D/gl4dm.h	\
GL4D/gl4du.h GL4D/gl4dummies.h GL4D/gl4wdummies.h

__top_builddir__bin_libGL4Dummies_la_SOURCES = GL4D/aes.c GL4D/aes.h	\
	GL4D/bin_tree.c GL4D/bin_tree.h GL4D/gl4dm.c GL4D/gl4dm.inl	\
	GL4D/gl4du.c GL4D/gl4dummies.c

if USE_VERSION_RC
__top_builddir__bin_libGL4Dummies_la_LDFLAGS =      \
        -no-undefined           \
        -release $(LT_RELEASE)  \
        -version-info  $(VERSION_INFO) -Wl,version.o
__top_builddir__bin_libGL4Dummies_la_DEPENDENCIES = version.o
else
__top_builddir__bin_libGL4Dummies_la_LDFLAGS =      \
        -no-undefined           \
        -release 0.1  \
        -version-info $(VERSION_INFO)
endif

%.o : %.rc
        $(WINDRES) $< $@

EXTRA_DIST = version.rc

pkgconfigdir = $(libdir)/pkgconfig

clean-local:
	rm -f *~
