
Fichier créé par Vincent Boyer
email boyer@ai.univ-paris8.fr
Novembre 2016
---------------------------

Ce fichier a pour objectif de fournir une explication sur ce que permet la librairie ASSIMP
et ce qu'elle ne permet pas.


Ce fichier est organisé par format de fichier de modèle 3D.
Il exprime les limitations constatées et produit si possible une solution.


---------------------------
OBJ

Pb : Les materiaux utlisés ne sont pas les bons. Seul les dernier materiel est utilisé.

Raison : lorsque un groupe est défini et que les faces sont
spécifiées, il n'est pas obligatoire de spécifier le matériau
utilisé. Le materiau utilisé peut être specifié en aval du fichier en
associant le nom du groupe et son materiel. La librairie ne supporte
pas cela et il est necessaire que les materiau soient précisés au
moment où les faces sont données.

Exemple : 
g pupilL
f 3477 3541 3540
f 3477 3540 3539
f 3477 3539 3550
f 3477 3550 3549

group pupilL
usemtl pupilL


Solution : specifier le materiel utilisé lors de la définition du groupe.

Exemple : 
g pupilL
usemtl pupilL
f 3477 3541 3540
f 3477 3540 3539
f 3477 3539 3550
f 3477 3550 3549

---------------------------
OBJ

PB : les valeurs de shininess lues sont 4 fois supérieures à celles figurant dans le fichier mtl

Raison : La question a été posée sur la mailing list de libassimp et n'a pas de réponse :
Hello everyone,

I'm new to assimp mailing list, so, I don't know if this was discussed
before. I didn't found anything related in previous topics.

I'm using assimp to import obj files into my application. I noticed that
the SHININESS property is multiplied by 4 compared to what is written for
Ns in mtl file. I take a look at the source code (ObjFileMtlImporter.cpp)
and there is nothing wrong, it is reading a float value and storing it in
the shininess parameter (which by the way is written as shineness).

Is this an expected behavior?

Solution : Ne pas considérer le speculaire dans le shader


---------------------------
OBJ

PB : Meme lorsqu'aucun materiau n'est spécifié, une valeur par défaut
est lue. Du coup les valeurs par défaut mentionné dans le code semble
inutile (diffus)

Raison : ?

Solution : ?
