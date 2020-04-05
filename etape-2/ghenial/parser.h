#include "abnf.h"

int parse(tree_node ** tree, abnf_rule * rule, string str);

/*
Notes :

Les règles de l'abnf ont été "simplifiées" : 
Les rulename sont case sensitive, ainsi que les multiples caractères entre guillemets ("AbA" est case sensitive)
Le format DIGIT(rulename), qui permet de vérifier si la règles est présente exactement DIGIT fois,
n'est pas implémenté, il faut par conséquent y remplacer par des règles sous la forme DIGIT*DIGIT(rulename)
%x n'est pas implémenté non plus, il faut préciser les valeurs une par une via la notation "A" / "B"
La manière dont l'implémentation a été réalisée fait aussi que des ' ' sont nécessaires entre une parenthèse et son contenu (en particulier si elle contient des caractères entre guillemets)
En dehors de ces points, toutes les règles abnf peuvent être ajoutées à cette liste.
*/