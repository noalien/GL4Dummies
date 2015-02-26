dnl INIT
AC_INIT([GL4Dummies], [0.1.2])
AM_INIT_AUTOMAKE([subdir-objects])
dnl Verifier le type de systeme
AC_CANONICAL_HOST
dnl Trouver le compilateur C approprie
AC_LANG(C)
AC_PROG_CC

dnl Utiliser la libtool
dnl AC_PROG_LIBTOOL

dnl Verifie l'existance des headers C standards
dnl AC_CONFIG_HEADERS
AC_HEADER_STDC

AM_CONDITIONAL(USE_VERSION_RC, false)

AC_CHECK_TOOL(DOXYGEN, doxygen)
AC_PATH_PROGS([SDL_CONFIG], [sdl2-config sdl20-config], [none], [$PATH])


dnl Here are a set of rules to help you update your library version
dnl information:

dnl 1. Start with version information of ‘0:0:0’ for each libtool
dnl library.
dnl 2. Update the version information only immediately before a public
dnl release of your software. More frequent updates are unnecessary, and
dnl only guarantee that the current interface number gets larger faster.
dnl 3. If the library source code has changed at all since the last
dnl update, then increment revision (‘c:r:a’ becomes ‘c:r+1:a’).
dnl 4. If any interfaces have been added, removed, or changed since
dnl the last update, increment current, and set revision to 0.
dnl 5. If any interfaces have been added since the last public
dnl release, then increment age.
dnl 6. If any interfaces have been removed or changed since the last
dnl public release, then set age to 0.  
m4_define(LT_CURRENT, 1)
m4_define(LT_REVISION, 0) 
m4_define(LT_AGE, 0)

VERSION_INFO="LT_CURRENT():LT_REVISION():LT_AGE()"
AC_SUBST(VERSION_INFO)

LT_INIT

SDL_CFLAGS=`sdl2-config --cflags`
SDL_LDFLAGS=`sdl2-config --libs`
CPPFLAGS="-I. $SDL_CFLAGS"
CFLAGS="-Wall -O3"
LDFLAGS="-lm $SDL_LDFLAGS -lpng -ljpeg"

echo "	***	Configuring for ... $host	***"

case "$host" in
dnl MACOS X
*-*-darwin*)
        AC_DEFINE_UNQUOTED(MACOSX, 1, [If we are on macosx plateform.])
        MACOSX_DEPLOYMENT_TARGET="10.8"
        CPPFLAGS="$CPPFLAGS -I/usr/X11R6/include"
        CFLAGS="$CFLAGS -mmacosx-version-min=$MACOSX_DEPLOYMENT_TARGET"
        LDFLAGS="$LDFLAGS -framework OpenGL -mmacosx-version-min=$MACOSX_DEPLOYMENT_TARGET"
;;
dnl FREEBSD
*-*-freebsd*)
        CPPFLAGS="$CPPFLAGS -I/usr/local/include"
        LDFLAGS="-L/usr/local/lib $LDFLAGS"
;;
dnl CYGWIN
*-*-cygwin*)
        AC_DEFINE_UNQUOTED(CYGWIN,1, [If we are on cygwin plateform.])
        LDFLAGS="$LDFLAGS -lopengl32 -lmingw32 -lSDL2main -lSDL2"
;;
dnl MINGW32
*-*-mingw32)
        AC_DEFINE_UNQUOTED(MINGW32,1, [If we are on mingw32 plateform.])
        LDFLAGS="$LDFLAGS -lopengl32 -lmingw32 -lSDL2main -lSDL2"
;;
dnl Cross-compiling MINGW32
i386-pc-mingw32msvc)
        AC_DEFINE_UNQUOTED(MINGW32,1, [If we are on mingw32 plateform.])
        LDFLAGS="$LDFLAGS -lopengl32 -lmingw32 -lSDL2main -lSDL2"
;;
dnl OTHER SYSTEMS
*)
	echo "$CPPFLAGS --- $CFLAGS --- $LDFLAGS"
;;
esac

AC_OUTPUT([Makefile] [lib_src/Makefile] [lib_src/documentation/Makefile] [samples/Makefile] [samples/example01/Makefile] [samples/example02/Makefile] [samples/example03/Makefile] [samples/example04/Makefile] [samples/example05/Makefile])
