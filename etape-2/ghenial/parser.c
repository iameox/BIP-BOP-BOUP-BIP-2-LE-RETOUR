/*
* GHENIA Lucas
* P2022 IRC
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "abnf.h"

//ASUPPR
//Juste pour indenter
int indent = 0;


//valid > 0 : ok et c'est la taille qu'il faut avancer
//valid = 0 : ok 
//valid < 0 : règle fausse

/*
* La big fonction de parsing
*/
int parse(tree_node ** tree, abnf_rule * rule, string str) {
	int i = 0, j, groups, done, start,
	    k, a, b,
	    valid = 0,
	    match = 0;
	abnf_rule * subrule = NULL;
	string tmp_rule = {rule->expression.str, rule->expression.size},
		   tmp_str = {str.str, str.size};

	//for(int f = 0 ; f < indent ; f++) printf("\t");
	//printf(">>>ON ENTRE DANS %s ET STR = \"%s\"\n", rule->rulename.str, str.str);
	//printf(">>>ON ENTRE DANS %s\n", rule->rulename.str);
	indent++;
	//print_tree(tree);
	//Création et ajout dans l'arbre
	tree_node * self_node = create_node(rule, str);
	add_node(tree, self_node);
	//print_tree(tree);
	if (!(rule->is_terminal)) {
		while (i < rule->expression.size) {
			switch (rule->expression.str[i]) {
				case '"': //Case sensitive
					i++;
					tmp_rule.str++;
					tmp_rule.size--;
					done = 0;
					match = 1;
					start = i; 
					while(!done && match > 0) {
						if(rule->expression.str[i] == '"') {
							done = 1;
						} else {
							match = is_char(rule->expression.str[i],tmp_str);
							if (match > 0) { //si règle vérifiée, on avance
								valid += match;
								tmp_str.str += match;
								tmp_str.size -= match;
							} else {
								valid = -1;
							}
						}
						i++;
						tmp_rule.str++;
						tmp_rule.size--;
					}
					if(match <= 0) {
						//On reset notre avancement dans tmp_str:
						if(i-start > 1) {
							tmp_str.str -= i - start - 1;
							tmp_str.size += i - start - 1;
						}
						while(rule->expression.str[i] != '"') { //on finit quand même la chaine
							i++;
							tmp_rule.str++;
							tmp_rule.size--;
						}

					}
					i++;
					tmp_rule.str++;
					tmp_rule.size--;
					break;
				case '*': //récup les éventuels digits avant et après, et répéter la règle
					//printf("REPETITION\n");
					if(i == 0) {
						a = 0;
					} else {
						a = rule->expression.str[i-1];
					}
					b = rule->expression.str[i+1];
					if(!is_digit(a)) {
						a = 0;
					} else {
						a = a - '0';
					}

					if(!is_digit(b)) {
						b = -1;
					} else {
						b = b - '0';
						i++;
						tmp_rule.str++;
						tmp_rule.size--;
					}
					i++;
					tmp_rule.str++;
					tmp_rule.size--;
					k = 0;
					if(rule->expression.str[i] == '(') {
						j = i+1;
						groups = 1;
						done = 0;
						while (!done) {
							if(rule->expression.str[j] == '(' && rule->expression.str[(j-1)] != '"') groups++;
							else if(rule->expression.str[j] == ')' && rule->expression.str[(j-1)] != '"') {
								if(groups == 1) done = 1;
								else groups--;
							}
							j++;
						}
						subrule = create_rule("GROUPE*", strlen("GROUPE*"), tmp_rule.str + 1, j - i - 1, 0, rule->elements);
						//printf("GROUPE + * = ");
						//printntruc(tmp_rule.str, j - i);

					} else {
						subrule = get_subrule(rule, i);
					}
					if(subrule != NULL) {
						match = 1;
						while(match > 0 && valid != -1) {
							match = parse(&(self_node->children), subrule, tmp_str);
							if(match > 0) { //si règle vérifiée, on avance
								tmp_str.str += match;
								tmp_str.size -= match;
								if(valid != -1) valid += match;
							}
							k++;
						}
						if ((a != b && match < 0 && k < a+1 && a != 0) || (a == b && match < 0 && k != a+1) || (a != b && match < 0 && k > b+1 && b != -1)) {
							valid = -1;
						}
						//On incrémente les index, selon si on a utilisé une sous règle groupée ou pas
						if(rule->expression.str[i] == '(') {
							i += subrule->expression.size;
							tmp_rule.str += subrule->expression.size;
							tmp_rule.size -= subrule->expression.size;
						} else {
							i += subrule->rulename.size;
							tmp_rule.str += subrule->rulename.size;
							tmp_rule.size -= subrule->rulename.size;
						}
						i++;
						tmp_rule.str++;
						tmp_rule.size--;
					} else {
						i++;
						tmp_rule.str++;
						tmp_rule.size--;
					}
					break;
				case '(': // système de stack pour les expressions parenthèsées? 
					if(i > 0 && rule->expression.str[i-1] == '*') {
						printf("\nHONO, %s\n\n", rule->rulename.str);
						printntruc(tmp_rule.str -1, tmp_rule.size);
					}
					//printf("parenthèses\n");
					j = i+1;
					groups = 1;
					done = 0;
					while (!done) {
						if(rule->expression.str[j] == '(' && rule->expression.str[(j-1)] != '"') groups++;
						else if(rule->expression.str[j] == ')' && rule->expression.str[(j-1)] != '"') {
							if(groups == 1) done = 1;
							else groups--;
						}
						j++;
					}
					subrule = create_rule("GROUPE", strlen("GROUPE"), tmp_rule.str + 1, j - i - 1, 0, rule->elements);
					//printf("GROUPE = ");
					//printntruc(tmp_rule.str, j - i);
					if(subrule != NULL) {
						match = parse(&(self_node->children), subrule, tmp_str);
						if(match > 0 && valid != -1) { //si règle vérifiée, on avance
							tmp_str.str += match;
							tmp_str.size -= match;
							valid += match;
						} else {
							valid = -1;
						}
						i += subrule->expression.size;
						tmp_rule.str += subrule->expression.size;
						tmp_rule.size -= subrule->expression.size;
					} else {
						i++;
						tmp_rule.str++;
						tmp_rule.size--;
					}
					i++;
					tmp_rule.str++;
					tmp_rule.size--;
					break;
				case '[': // optionnel, on cherche si il y est 
					j = 1;
					while (tmp_rule.str[j] != ']' || tmp_rule.str[(j-1)] == '"') j++;
					
					subrule = create_rule("OPTIONNEL", strlen("OPTIONNEL"), tmp_rule.str + 1, j-1, 0, rule->elements);
					if(subrule != NULL) {
						match = parse(&(self_node->children), subrule, tmp_str);
						if(match > 0 && valid != -1) { //si règle vérifiée, on avance
							tmp_str.str += match;
							tmp_str.size -= match;
							valid += match;
						}
						i += subrule->expression.size;
						tmp_rule.str += subrule->expression.size;
						tmp_rule.size -= subrule->expression.size;
					} else {
						i++;
						tmp_rule.str++;
						tmp_rule.size--;
					}
					break;
				case '/': //OU
					//printf("OU\n");
					i++;
					tmp_rule.str++;
					tmp_rule.size--;
					if(valid == -1) { //on remet valid a 0 car il y a une autre possibilité
						valid = 0;
					} else if (valid > 0){ // Si la première possibilité est valide, on sort, pas la peine de checker le reste
						i = rule->expression.size;
					 	//return valid;
					}
					break;
				case ' ': //concaténation on avance juste
					//printf("CONCATENATION\n");
					i++;
					tmp_rule.str++;
					tmp_rule.size--;
					break;
				default:
					subrule = get_subrule(rule, i);
					if (subrule != NULL) {
						match = parse(&(self_node->children), subrule, tmp_str);
						if (match >= 0 && valid != -1) { //si règle vérifiée, on avance
							valid += match;

							tmp_str.str += match;
							tmp_str.size -= match;

						} else {
							valid = -1;
						}
						i += subrule->rulename.size;
						tmp_rule.str+= subrule->rulename.size;
						tmp_rule.size-= subrule->rulename.size;
					} else {
						i++;
						tmp_rule.str++;
						tmp_rule.size--;
					}
					
					break;
			}
		}
	} else { //Règles terminales, codées en brute
		if(!strcmp(rule->rulename.str,"ALPHA")) {
			if(str.size != 0 && is_alpha(str.str[0])) valid = 1;
			else valid = -1;
		} else if(!strcmp(rule->rulename.str,"DIGIT")) {
			if(str.size != 0 && is_digit(str.str[0])) valid = 1;
			else valid = -1;
		} else if(!strcmp(rule->rulename.str,"SP")) {
			if(str.size != 0 && is_sp(str.str[0])) valid = 1;
			else valid = -1;
		}  else if(!strcmp(rule->rulename.str,"HTAB")) {
			if(str.size != 0 && is_htab(str.str[0])) valid = 1;
			else valid = -1;
		} else if(!strcmp(rule->rulename.str,"CRLF")) {
			if(str.size > 1 && is_crlf(str.str[0], str.str[1])) valid = 2;
			else valid = -1;
		}
	}

	if(valid >= 0) {
		//for(int f = 0 ; f < indent ; f++) printf("\t");
		//printf("LA CHAINE QUI PARSE LA REGLE %s = ", rule->rulename.str);
		//printntruc(str.str, valid);
		//On met à jour le noeud pour qu'il ne comprenne que la chaine qui marche
		self_node->value.size = valid;
	} else {
		//Sinon on supprime le noeud
		delete_node(tree, self_node);
	}
	indent--;
	//for(int f = 0 ; f < indent ; f++) printf("\t");
	//printf("<<<ON SORT DE %s, valid = %d\n", rule->rulename.str, valid);
	return valid;
}

void test(tree_node ** tree, abnf_rule *rule, char * str, int size) {
	string test = {str, size};
	int a = parse(tree, rule, test);
	printf("\n*** RESULTAT = %d\n", a);
	if(a == size) printf("*** OK : \"%s\" (%d) match la règle %s = \"%s\"\n\n", test.str, test.size, rule->rulename.str, rule->expression.str);
	else printf("*** KO : \"%s\" (%d) ne match PAS la règle %s = \"%s\"\n\n", test.str, test.size, rule->rulename.str, rule->expression.str);
}



/*int main(){
	tree_node * tree = NULL;
	abnf_rule* bite = init_rules();

	test(&tree, bite, "GET / HTTP/1.0\r\nAccept-Charset: iso-8859-5, unicode-1-1; q=0.8 \r\n\r\n", strlen("GET / HTTP/1.0\r\nAccept-Charset: iso-8859-5, unicode-1-1; q=0.8 \r\n\r\n"));
	print_tree(&tree);
	return 1;
}*/


/*
Notes :

Les règles de l'abnf ont été "simplifiées" : 
Les rulename sont case sensitive, ainsi que les multiples caractères entre guillemets ("AbA" est case sensitive)
DIGIT(rulename) n'est pas implémenté, il faut donc écrire les règles sous la forme DIGIT*DIGIT(rulename)
%x n'est pas implémenté non plus, il faut préciser les valeurs une par une via la notation "A" / "B"
La manière dont l'implémentation a été réalisée fait aussi que des ' ' sont nécessaires entre une parenthèse et son contenu (en particulier si elle contient des caractères entre guillemets)
En dehors de ces points, toutes les règles abnf peuvent être ajoutées au parseur.
*/