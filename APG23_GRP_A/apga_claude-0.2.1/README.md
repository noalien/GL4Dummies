* version 0.1
  - Gestion de la rastérisation d'un triangle
  - Puis ajout du dégradé de couleurs (interpolation bilinéaire des couleurs affectées aux sommets)
  - Enfin ajout de l'ajout de coordonnées de texture et application de la texture (pour l'instant par multiplication avec la couleur)


* version 0.2.0
  - ~~TODO~~ : créer une structure surface_t à n triangles.
  - ~~TODO~~ : ajouter/modifier le sommet : coordonnée 4D (x, y, z, w) ; couleur r, g, b, a ; texture s, t ; vecteur normal nx, ny, nz et enfin les coordonnées écran xe, ye (la depth ? le depth-buffer ? dans la v0.2.1 ?).
  - ~~TODO~~ : un fichier (.h ?) contenant les opérations sur les matrices et vecteurs
  - ~~TODO~~ : un transformations.c avec
    -	 -> claude_apply_transforms(M, V, P, s, s’) -> void avec s’ transformation de s (clipping non compris ??)
    -	 -> claude_draw(s’, viewport) -> (TODO -- clipping à faire ici ???)

* version 0.2.1
  - ~~TODO~~ : calcul de projection en perspective (frustum)
  - TODO : finir le depth test
  - ...