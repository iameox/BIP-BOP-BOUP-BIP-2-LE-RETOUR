#include "functions.h"

abnf_rule *init_rules();

/*
Notes :

Les règles de l'abnf ont été "simplifiées" : 
Les rulename sont case sensitive, ainsi que les multiples caractères entre guillemets ("AbA" est case sensitive)
DIGIT(rulename) n'est pas implémenté, il faut donc écrire les règles sous la forme DIGIT*DIGIT(rulename)
%x n'est pas implémenté non plus, il faut préciser les valeurs une par une via la notation "A" / "B"
La manière dont l'implémentation a été réalisée fait aussi que des ' ' sont nécessaires entre une parenthèse et son contenu (en particulier si elle contient des caractères entre guillemets)
En dehors de ces points, toutes les règles abnf peuvent être ajoutées à cette liste.
*/