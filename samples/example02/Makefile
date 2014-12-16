#  Makefile 
#  Auteur : Farès BELHADJ
#  Email  : amsi@ai.univ-paris8.fr
#  Date   : 15/12/2014

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
SDL_LDFLAGS = $(shell sdl2-config --libs)
CPPFLAGS = -I. -I../../lib_src/GL4D $(SDL_CFLAGS)
CFLAGS = -Wall -O3
LDFLAGS = -lm $(SDL_LDFLAGS) -lpng -ljpeg

UNAME := $(shell uname)
ifeq ($(UNAME),Darwin)
	MACOSX_DEPLOYMENT_TARGET = 10.8
        CFLAGS += -I/usr/X11R6/include -mmacosx-version-min=$(MACOSX_DEPLOYMENT_TARGET)
        LDFLAGS += -framework OpenGL -mmacosx-version-min=$(MACOSX_DEPLOYMENT_TARGET)
else
        LDFLAGS +=  -L/usr/lib -L/usr/X11R6/lib -lGL -lGLU
endif

#définition des fichiers et dossiers
PROGS = landscape
HEADERS = 
SOURCES = landscape.c
OBJ = $(SOURCES:.c=.o)
SLIB = ../../lib_src/libGL4Dummies.a
EXTRAFILES = ../shaders/basic.vs ../shaders/basic.fs

all: $(PROGS)

$(PROGS): $(OBJ) $(SLIB)
	$(CC) $(OBJ) $(SLIB) $(LDFLAGS) -o $(PROGS)

../../lib_src/libGL4Dummies.a:
	@make -C ../../lib_src/

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	@$(RM) -r $(PROGS) $(OBJ) *~ gmon.out core.* ../shaders/*~
