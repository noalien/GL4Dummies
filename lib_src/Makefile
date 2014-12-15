#  Makefile 
#  Auteur : Farès BELHADJ
#  Email  : amsi@ai.univ-paris8.fr
#  Date   : 03/02/2014

SHELL = /bin/sh
# définition des commandes utilisées
CC = gcc
ECHO = echo
RM = rm -f
TAR = tar
MKDIR = mkdir
CHMOD = chmod
CP = rsync -R
# déclaration des options du compilateur
PG_FLAGS =
SDL_CFLAGS = $(shell sdl2-config --cflags)
CPPFLAGS = -IGL4D $(SDL_CFLAGS)
CFLAGS = -Wall -O3

UNAME := $(shell uname)
ifeq ($(UNAME),Darwin)
	MACOSX_DEPLOYMENT_TARGET = 10.8
        CFLAGS += -I/usr/X11R6/include -mmacosx-version-min=$(MACOSX_DEPLOYMENT_TARGET)
endif

#définition des fichiers et dossiers
LIBNAME  = libGL4Dummies.a
PACKAGE=$(LIBNAME)
VERSION = 0.1.1
distdir = $(PACKAGE)-$(VERSION)
HEADERS = GL4D/aes.h GL4D/gl4du.h GL4D/gl4dummies.h GL4D/gl4wdummies.h GL4D/gl4dm.h GL4D/bin_tree.h
SOURCES = GL4D/aes.c GL4D/gl4du.c GL4D/gl4dummies.c GL4D/gl4dm.c GL4D/bin_tree.c
OBJ = $(SOURCES:.c=.o)
DOXYFILE = documentation/Doxyfile
DISTFILES = $(SOURCES) Makefile $(HEADERS) $(DOXYFILE)

all: $(LIBNAME)

$(LIBNAME): $(OBJ)
	ar rcs $(LIBNAME) $(OBJ)

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

dist: distdir
	$(CHMOD) -R a+r $(distdir)
	$(TAR) zcvf $(distdir).tgz $(distdir)
	$(RM) -r $(distdir)

distdir: $(DISTFILES)
	$(RM) -r $(distdir)
	$(MKDIR) $(distdir)
	$(CHMOD) 777 $(distdir)
	$(CP) $(DISTFILES) $(distdir)

doc: $(DOXYFILE)
	cat $< | sed -e "s/PROJECT_NAME *=.*/PROJECT_NAME = $(LIBNAME)/" | sed -e "s/PROJECT_NUMBER *=.*/PROJECT_NUMBER = $(VERSION)/" >> $<.new
	mv -f $<.new $<
	cd documentation && doxygen && cd ..

clean:
	@$(RM) -r $(LIBNAME) $(OBJ) *~ $(distdir).tgz documentation/*~ GL4D/*~ documentation/html
