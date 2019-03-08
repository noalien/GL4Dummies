AM_CPPFLAGS = -IGL4D -I. $(SDL_CFLAGS)

lib_LTLIBRARIES = $(top_builddir)/bin/libGL4Dummies.la

__top_builddir__bin_libGL4Dummiesincludedir = $(includedir)/GL4D

__top_builddir__bin_libGL4Dummiesinclude_HEADERS = GL4D/aes.h		\
	GL4D/bin_tree.h GL4D/linked_list.h GL4D/fixed_heap.h		\
	GL4D/gl4dm.h GL4D/gl4dg.h GL4D/gl4du.h GL4D/gl4dummies.h	\
	GL4D/gl4wdummies.h GL4D/gl4droid.h GL4D/gl4duw_SDL2.h		\
	GL4D/list.h GL4D/vector.h GL4D/gl4dm.inl			\
	GL4D/gl4dhAnimeManager.h GL4D/gl4dh.h GL4D/gl4dp.h		\
	GL4D/gl4dq.h GL4D/gl4dfBlurWeights.h GL4D/gl4df.h GL4D/gl4da.h

__top_builddir__bin_libGL4Dummies_la_SOURCES = GL4D/aes.c GL4D/aes.h	\
	GL4D/bin_tree.c GL4D/bin_tree.h GL4D/fixed_heap.h		\
	GL4D/fixed_heap.c GL4D/gl4dm.c GL4D/gl4dm.h GL4D/gl4dg.c	\
	GL4D/gl4dg.h GL4D/gl4du.c GL4D/gl4du.h GL4D/gl4dummies.c	\
	GL4D/gl4dummies.h GL4D/gl4wdummies.h GL4D/gl4droid.h		\
	GL4D/gl4droid.c GL4D/gl4duw_SDL2.h GL4D/gl4duw_SDL2.c		\
	GL4D/list.c GL4D/list.h GL4D/linked_list.c GL4D/linked_list.h	\
	GL4D/vector.c GL4D/vector.h GL4D/gl4dhAnimeManager.c		\
	GL4D/gl4dhAnimeManager.h GL4D/gl4dh.h GL4D/gl4dp.c		\
	GL4D/gl4dp.h GL4D/gl4dq.c GL4D/gl4dfBlur.c			\
	GL4D/gl4dfBlurWeights.h GL4D/gl4df.h GL4D/gl4dfCommon.h		\
	GL4D/gl4dfCommon.c GL4D/gl4dfSobel.c GL4D/gl4dfCanny.c		\
	GL4D/gl4dfMedian.c GL4D/gl4dfScattering.c GL4D/gl4dfFocus.c	\
	GL4D/gl4dfConversion.c GL4D/gl4dfMedia.c			\
	GL4D/gl4dfFractalPainting.c GL4D/gl4dfHatching.c		\
	GL4D/gl4dfSegmentation.c GL4D/gl4dfOpticalFlow.c		\
	GL4D/gl4dfOp.c GL4D/gl4da.c GL4D/gl4da.h

if USE_VERSION_RC
__top_builddir__bin_libGL4Dummies_la_LDFLAGS =      \
        $(SDL_LIBS) -no-undefined           \
        -release $(LT_RELEASE)  \
        -version-info  $(VERSION_INFO) -Wl,version.o
__top_builddir__bin_libGL4Dummies_la_DEPENDENCIES = version.o
else
__top_builddir__bin_libGL4Dummies_la_LDFLAGS =      \
        $(SDL_LIBS) -no-undefined           \
        -release 0.1  \
        -version-info $(VERSION_INFO)
endif

%.o : %.rc
        $(WINDRES) $< $@

EXTRA_DIST = version.rc

pkgconfigdir = $(libdir)/pkgconfig

clean-local:
	rm -f *~
