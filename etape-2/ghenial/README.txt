Auteur : GHENIA Lucas
P2022 IR&C


Commandes :

Pour compiler : make
Pour exécuter le parseur : ./main fichier règle

Si besoin, pour tester uniquement le parseur (sans api) : 
Décommenter le main de parser.c
make test
./parseur_test


Remarques :
Ce parseur se base sur les règles ABNF, avec quelques simplifications, et peut, en théorie, parser selon n'importe quelle règle ABNF du moment que certaines spécifications sont respectées. 
Les fonctions sont toutes commentées dans leurs .c respectifs, et les structures dans les .h.
Dans parser.h et abnf.h se trouvent des commentaires sur les règles ABNF, notamment quelles simplifications ont été réalisées et les règles à respecter pour ajouter une nouvelle règle.

Je suis conscient que la fonction parse() est excessivement longue et indigeste, mais du à un manque d'organisation de ma part j'ai manqué de temps pour le projet (bien que ce soit entièrement ma faute), ainsi je n'ai pas eu le temps de la simplifier d'avantage. De nombreux bouts de cette fonctions peuvent être simplifiés, la plupart n'est très probablement pas optimisé et il est même possible que certains cas de la grammaire ABNF autres que ceux précisés dans les commentaires soient mal parsés.

Après avoir personnellement testé le parseur sur les tests avancés, seulement 1/4 ont été parsés correctement (58/200 pour être exact...), je ne doute pas que ce ne sera pas mieux de votre côté.

Je suis aussi conscient que toute la mémoire n'est pas libérée. Après analyse par Valgrind, je trouve qu'environ 60% de la mémoire est libérée en moyenne. C'est un bilan plutôt catastrophique, mais encore une fois je n'ai pas eu le temps de me pencher dessus...
