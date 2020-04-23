ACLOCAL_AMFLAGS = -I m4

SUBDIRS=lib_src lib_src/documentation samples/demo

EXTRA_DIST = Windows/gl4dDemo.cbp Windows/gl4dDemo.vcxproj Windows/GL4Dummies.cbp Windows/GL4Dummies.sln Windows/GL4Dummies.vcxproj Windows/README.txt Windows/dependencies/SDL2

doc:
	@make -C lib_src/documentation doc

install-data-local:
	@$(NORMAL_INSTALL)
	chmod a+rx $(prefix)/include/GL4D
	chmod a+r $(prefix)/include/GL4D/*
#	$(mkinstalldirs) $(prefix)/share/GL4Dummies
#	$(mkinstalldirs) $(prefix)/share/GL4Dummies/data
#	$(mkinstalldirs) $(prefix)/share/GL4Dummies/images
#	$(mkinstalldirs) $(prefix)/share/GL4Dummies/shaders
#	$(INSTALL_DATA) bin/share/GL4Dummies/data/obama.mtl $(prefix)/share/GL4Dummies/data/
#	$(INSTALL_DATA) bin/share/GL4Dummies/data/obama.obj $(prefix)/share/GL4Dummies/data/
#	$(INSTALL_DATA) bin/share/GL4Dummies/images/repere.bmp $(prefix)/share/GL4Dummies/images/
#	$(INSTALL_DATA) bin/share/GL4Dummies/shaders/*.fs $(prefix)/share/GL4Dummies/shaders/
#	$(INSTALL_DATA) bin/share/GL4Dummies/shaders/*.vs $(prefix)/share/GL4Dummies/shaders/


uninstall-local:
	@$(NORMAL_UNINSTALL)
#	@rm -rf $(prefix)/share/GL4Dummies

clean-local:
	@rm -f *~
