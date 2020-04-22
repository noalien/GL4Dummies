#  Makefile 
#  Auteur : Farès BELHADJ
#  Email  : amsi@ai.univ-paris8.fr
#  Date   : 03/02/2014

SHELL = /bin/sh
# définition des commandes utilisées
CPPC = g++
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
CPPFLAGS = -I. $(SDL_CFLAGS)
CFLAGS = -Wall -O3
LDFLAGS = -lm -lGL4Dummies $(SDL_LDFLAGS) -lopencv_highgui -lopencv_imgproc -lopencv_core -lopencv_objdetect -lopencv_videoio

UNAME := $(shell uname)
ifeq ($(UNAME),Darwin)
	MACOSX_DEPLOYMENT_TARGET = 10.9
        CFLAGS += -I/usr/X11R6/include -I/opt/local/include/ -I/opt/local/include/opencv2	\
        -I/usr/local/include/opencv2					\
        -I/usr/local/include/opencv2/objdetect				\
        -mmacosx-version-min=$(MACOSX_DEPLOYMENT_TARGET)
        LDFLAGS += -framework OpenGL -L/opt/local/lib -mmacosx-version-min=$(MACOSX_DEPLOYMENT_TARGET) -L/usr/lib -lc++ -lopencv_videoio -lopencv_imgcodecs
else
        CFLAGS += -I/usr/include/opencv2 -I/usr/include/opencv2/objdetect
        LDFLAGS += -L/usr/lib -L/usr/X11R6/lib -lGL -lGLU -lstdc++
endif

#définition des fichiers et dossiers
PROGNAME = sample_ocv3
PACKAGE=$(PROGNAME)
VERSION = 1.0
distdir = $(PACKAGE)-$(VERSION)
HEADERS = 
SOURCES = window.cpp
OBJ = $(SOURCES:.cpp=.o)
DOXYFILE = documentation/Doxyfile
EXTRAFILES = COPYING shaders/basic.vs shaders/basic.fs
DISTFILES = $(SOURCES) Makefile $(HEADERS) $(DOXYFILE) $(EXTRAFILES)

all: $(PROGNAME)

$(PROGNAME): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $(PROGNAME)

%.o: %.cpp
	$(CPPC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

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
	cat $< | sed -e "s/PROJECT_NAME *=.*/PROJECT_NAME = $(PROGNAME)/" | sed -e "s/PROJECT_NUMBER *=.*/PROJECT_NUMBER = $(VERSION)/" >> $<.new
	mv -f $<.new $<
	cd documentation && doxygen && cd ..

clean:
	@$(RM) -r $(PROGNAME) $(OBJ) *~ $(distdir).tgz gmon.out core.* documentation/*~ shaders/*~ documentation/html
