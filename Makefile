#  Makefile 
#  Auteur : Farès BELHADJ
#  Email  : amsi@ai.univ-paris8.fr
#  Date   : 12/12/2014

default: 
	@$(MAKE) -C lib_src
	@$(MAKE) -C samples

clean:
	@$(MAKE) -C lib_src clean
	@$(MAKE) -C samples clean

