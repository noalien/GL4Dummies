all:
	for i in sc_*; do make -C "$$i" dist && mv "$$i/$$i.tgz" archives/; done
clean:
	rm -f archives/*.tgz
