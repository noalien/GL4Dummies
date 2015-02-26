# Utiliser doxygen afin de creer une documentation a partir des
# commentaires presents dans les fichiers sources.

EXTRA_DIST = Doxyfile

all-local:

doc: html/index.html

html/index.html: 
	doxygen Doxyfile

clean-local:
	rm -f *~
	rm -rf html
