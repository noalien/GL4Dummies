SUBDIRS=lib_src lib_src/documentation samples samples/example01 samples/example02 samples/example03 samples/example05

EXTRA_DIST = bin/share

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

uninstall-local:
	@$(NORMAL_UNINSTALL)
	@rm -rf $(prefix)/share/GL4Dummies

clean-local:
	@rm -f *~
