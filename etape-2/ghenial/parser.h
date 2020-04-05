/*
* GHENIA Lucas
* P2022 IRC
*/
#include "abnf.h"
//valid > 0 : ok et c'est la taille qu'il faut avancer
//valid = 0 : ok 
//valid < 0 : règle fausse

/*
* La big fonction de parsing
*/
int parse(tree_node ** tree, abnf_rule * rule, string str);

