* Version 0.1 :
  	- Remplissage de triangle par dégradé
	- Remplissage de triangle par placage de texture
	- TODO : mixer les deux types de remplissage
	- TODO : réfléchir aux fonctionnalités relatives aux transformations 3D
	  - TODO : ->> étendre les structures de données

* Version 0.1.1 :
  	- (DONE en 0.1) : Remplissage de triangle par dégradé
	- (DONE en 0.1) : Remplissage de triangle par placage de texture
	- mixer les deux types de remplissage
	- TODO : réfléchir aux fonctionnalités relatives aux transformations 3D
	  - TODO : ->> étendre les structures de données

* Version 0.2.0 :
  	- (DONE en 0.1) : Remplissage de triangle par dégradé
	- (DONE en 0.1) : Remplissage de triangle par placage de texture
	- (DONE en 0.1.1) : mixer les deux types de remplissage
	- DONE : fonctionnalités relatives aux transformations 3D : ajoutées, non complètement testées
	- DONE : étendre les structures de données : première proposition, reste d'autres ajouts à faire

* Version 0.2.1 :
  - DONE : utilisation de elClear();
  - DONE : Correction de la perspective
  - TODO : Éclairage
  - TODO : Mixer les rendus par des tests
  - TODO : ajouter le depth buffer (tableau de profondeur)
  - DONE : passer la texture en RGBA au lieu de ABGR

* Version 0.2.2 :
  - DONE : ajouter le depth buffer (tableau de profondeur)
  - TODO : Éclairage
  - TODO : Mixer les rendus par des tests

* Version 0.2.3 :
  - DONE : Éclairage Gouraud
  - TODO !!! : ajouter des géométries (sphère et cube, le quad est déjà fait)
  - TODO !!! : surface, appliquer une texture différente ???? Comment ???? (propriété de surface)
  - TODO !!! : surface, appliquer une couleur ???? Comment ???? (propriété de surface)
  - TODO : Comment faire pour mixer les rendus ? Par des tests -> OK. Où se situe l'information ? dans la surface

* Version 0.2.4 :
  - DONE : ajouter des géométries (sphère, cube et quad)
  - DONE : corrections de bugs sur division par zéro
  - TODO !!! : surface, appliquer une texture différente ???? Comment ???? (propriété de surface)
  - TODO !!! : surface, appliquer une couleur ???? Comment ???? (propriété de surface)
  - TODO : Comment faire pour mixer les rendus ? Par des tests -> OK. Où se situe l'information ? dans la surface

* Version 0.2.5 :
  - DONE : surface, appliquer une texture différente par surface (elGenTexture/elUseTexture/elFreeTexture)
  - TODO !!! : surface, appliquer une couleur ???? Comment ???? (propriété de surface)
  - TODO : Comment faire pour mixer les rendus ? Par des tests -> OK. Où se situe l'information ? dans la surface

* Version 0.2.6 :
  - DONE : changement de paradigme sur les texture, on affecte une texture à une surface (elGenTexture/elSetTexture/elFreeTexture)
    - si vous préférez l'autre méthode, reprenez la dans la v0.2.5
  - DONE : surface, appliquer/affecter une couleur sur les sommets d'une surface (elSetColor)
  - TODO : Comment faire pour mixer les rendus ? Par des tests -> OK. Où se situe l'information ? dans la surface

* Version 0.2.7 :
  - DONE : Comment faire pour mixer les rendus ? mise en place d'activation/désactivation d'options de rendu : EL_ALPHA, EL_BACKFACE_CULLING, EL_COLOR (reste à faire), EL_SHADING_EL_TEXTURE. Fonctions : elEnable/elDisable/elIsEnabled
  - DONE : calcul des faces arrières pour pouvoir ne pas les rendre

* Version 0.2.8 :
  - DONE : un excemple riche utilisant beaucoup de fonctionnalités
  - TODO : toujours des clipping à faire
