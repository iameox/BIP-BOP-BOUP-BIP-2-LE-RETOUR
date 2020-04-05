Marin Dautrey
3A IR&C
Le 05/04/2020
					Commentaires sur le parseur HTTP


Pour compiler : make

Pour exécuter le parseur : ./main fichier méthode

Pour exécuter le parseur sur tous les fichiers de tests acceptés : ./test.sh | grep "FOUND" | wc -l


Je tiens à m'excuser pour l'imperfection et l'incomplétude de ce parseur.

Dû à un manque de temps, je n'ai pas pu identifier les sources d'erreurs (notamment dues aux caractères spéciaux 
utilisés dans beaucoup de fichiers de test). Ainsi, pour ce parseur, seuls 16 fichiers sont valides syntaxiquement,
tandis que 12 fichiers de test provoquent des erreurs non négligeables (boucle infinie, erreur de segmentation ou stack smashing).

Je n'ai pas non plus pu m'adapter parfaitement à l'api. Donnée tardivement, j'ai dû modifier la grande majorité
de mon code juste pour pouvoir exécuter le parseur à l'aide de l'api, ce qui m'a pris la majorité de mon temps sur cette étape du projet.

Enfin, je m'excuse de ne pas avoir mieux géré la mémoire.
Egalement dû à un manque de temps, je n'ai pas pu réaliser des fonctions efficaces pour libérer toute la mémoire utilisée.
Cette dernière est donc en majorité perdue, ce qui est réellement déplorable.

J'aurais vraiment aimé avoir plus de temps pour améliorer la version du parseur que je propose aujourd'hui.
Mais le travail dans les autres matières étant lui aussi conséquent, j'accepte de ne rendre que cette version médiocre.

Merci d'avance pour votre compréhension.

