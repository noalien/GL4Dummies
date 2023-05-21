#  Makefile 
#  Auteur : Farès BELHADJ
#  Email  : amsi@up8.edu
#  Date   : 08/04/2022
# définition des commandes utilisées
CC = gcc
CXX = g++
ECHO = echo
RM = rm -f
TAR = tar
ZIP = zip
MKDIR = mkdir
CHMOD = chmod
CP = rsync -R
# déclaration des options du compilateur
CFLAGS = -Wall -O3
CPPFLAGS = -I.
LDFLAGS = -lm
# définition des fichiers et dossiers
PACKNAME = sc_03_01
PROGNAME = libAssimp
VERSION = 1.0
distdir = $(PACKNAME)_$(PROGNAME)-$(VERSION)
HEADERS = assimp.h
SOURCES = window.c assimp.c
MSVCSRC = $(patsubst %,<ClCompile Include=\"%\\\" \\/>,$(SOURCES))
SSRC = $(SOURCES:.cpp=.o)
ifeq ($(SOURCES), $(SSRC))
# aucun fichier C++ dans les sources
	LD = $(CC)
else
# au moins un fichier C++
	LD = $(CXX)
endif
OBJ = $(SSRC:.c=.o)
DOXYFILE = documentation/Doxyfile
VSCFILES = $(PROGNAME).vcxproj $(PROGNAME).sln
EXTRAFILES = COPYING $(wildcard shaders/*.?s images/*.png audio/* models/* models/Al/* models/fish/* models/soccer/*) $(VSCFILES)
DISTFILES = $(SOURCES) Makefile $(HEADERS) $(DOXYFILE) $(EXTRAFILES)
# Traitements automatiques pour ajout de chemins et options (ne pas modifier)
ifneq (,$(shell ls -d /usr/local/include 2>/dev/null | tail -n 1))
	CPPFLAGS += -I/usr/local/include
endif
ifneq (,$(shell ls -d $(HOME)/local/include 2>/dev/null | tail -n 1))
	CPPFLAGS += -I$(HOME)/local/include
endif
ifneq (,$(shell ls -d /usr/local/lib 2>/dev/null | tail -n 1))
	LDFLAGS += -L/usr/local/lib
endif
ifneq (,$(shell ls -d $(HOME)/local/lib 2>/dev/null | tail -n 1))
	LDFLAGS += -L$(HOME)/local/lib
endif
ifeq ($(shell uname),Darwin)
	MACOSX_DEPLOYMENT_TARGET = 11.0
	CPPFLAGS += -I/opt/local/include/ -I/opt/local/include/opencv3/
        CFLAGS   += -mmacosx-version-min=$(MACOSX_DEPLOYMENT_TARGET)
        LDFLAGS  += -framework OpenGL -mmacosx-version-min=$(MACOSX_DEPLOYMENT_TARGET)
        LDFLAGS  += -L/opt/local/lib/opencv3/
else
	CPPFLAGS += -I/usr/include/opencv3/
	CPPFLAGS += -I/usr/local/include/opencv3/
        LDFLAGS  += -lGL
endif
CPPFLAGS += $(shell sdl2-config --cflags)
LDFLAGS  += -lGL4Dummies $(shell sdl2-config --libs) -lSDL2_image -lassimp
# si besoin LDFLAGS  += -lSDL2_mixer -lopencv_core -lopencv_highgui -lopencv_video -lopencv_videoio

all: $(PROGNAME)
$(PROGNAME): $(OBJ)
	$(LD) $(OBJ) $(LDFLAGS) -o $(PROGNAME)
%.o: %.cpp
	@echo $(SSRC) $(SOURCES)
	$(CXX) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
%.o: %.c
	echo $(SSRC) $(SOURCES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
dist: distdir
	$(CHMOD) -R a+r $(distdir)
	$(TAR) zcvf $(distdir).tgz $(distdir)
	$(RM) -r $(distdir)
zip: distdir
	$(CHMOD) -R a+r $(distdir)
	$(ZIP) -r $(distdir).zip $(distdir)
	$(RM) -r $(distdir)
distdir: $(DISTFILES)
	$(RM) -r $(distdir)
	$(MKDIR) $(distdir)
	$(CHMOD) 777 $(distdir)
	$(CP) $(DISTFILES) $(distdir)
doc: $(DOXYFILE)
	cat $< | sed -e "s/PROJECT_NAME *=.*/PROJECT_NAME = $(PROGNAME)/" |\
	  sed -e "s/PROJECT_NUMBER *=.*/PROJECT_NUMBER = $(VERSION)/" >> $<.new
	mv -f $<.new $<
	cd documentation && doxygen && cd ..
msvc: $(VSCFILES)
	@echo "Now these files ($?) already exist. If you wish to regenerate them, you should first delete them manually."  
$(VSCFILES):
	@echo "Generating $@ ..."
	@cat ../../Windows/templates/gl4dSample$(suffix $@) | sed -e "s/INSERT_PROJECT_NAME/$(PROGNAME)/g" | sed -e "s/INSERT_TARGET_NAME/$(PROGNAME)/" | sed -e "s/INSERT_SOURCE_FILES/$(MSVCSRC)/" > $@
clean:
	@$(RM) -r $(PROGNAME) $(OBJ) *~ $(distdir).tgz $(distdir).zip gmon.out	\
	  core.* documentation/*~ shaders/*~ documentation/html
