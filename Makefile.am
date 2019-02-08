SUBDIRS=lib_src lib_src/documentation samples samples/example01 samples/example02 samples/example03 samples/example04  


EXTRA_DIST = bin/share bin/Win32/SDL2.dll Windows/GL4D.workspace Windows/gl4dLandscape.cbp Windows/gl4dLandscape.dev Windows/gl4dLandscape.vcxproj Windows/gl4dObjLoader.cbp Windows/gl4dObjLoader.dev Windows/gl4dObjLoader.vcxproj Windows/GL4Dummies.cbp Windows/GL4Dummies.dev Windows/GL4Dummies.sln Windows/GL4Dummies.suo Windows/GL4Dummies.vcxproj Windows/gl4dWalkOnPolygons.cbp Windows/gl4dWalkOnPolygons.dev Windows/gl4dWalkOnPolygons.vcxproj Windows/Makefile.win Windows/README.txt Windows/dependencies/include Windows/dependencies/lib

doc:
	@make -C lib_src/documentation doc

install-data-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(prefix)/share/GL4Dummies
	$(mkinstalldirs) $(prefix)/share/GL4Dummies/data
	$(mkinstalldirs) $(prefix)/share/GL4Dummies/images
	$(mkinstalldirs) $(prefix)/share/GL4Dummies/shaders
	$(INSTALL_DATA) bin/share/GL4Dummies/data/obama.mtl $(prefix)/share/GL4Dummies/data/
	$(INSTALL_DATA) bin/share/GL4Dummies/data/obama.obj $(prefix)/share/GL4Dummies/data/
	$(INSTALL_DATA) bin/share/GL4Dummies/images/repere.bmp $(prefix)/share/GL4Dummies/images/
	$(INSTALL_DATA) bin/share/GL4Dummies/shaders/*.fs $(prefix)/share/GL4Dummies/shaders/
	$(INSTALL_DATA) bin/share/GL4Dummies/shaders/*.vs $(prefix)/share/GL4Dummies/shaders/
	chmod a+rx $(prefix)/include/GL4D
	chmod a+r $(prefix)/include/GL4D/*

uninstall-local:
	@$(NORMAL_UNINSTALL)
	@rm -rf $(prefix)/share/GL4Dummies

clean-local:
	@rm -f *~
