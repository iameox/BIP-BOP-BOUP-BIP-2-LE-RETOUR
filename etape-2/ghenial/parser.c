#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "abnf.h"

/*
* Fonction auxiliaire qui permet de récupérer une expression délimitée par start_delim et end_delim,
* en prenant en compte les éventuelles imbrications
* ATTENTION : la chaine est supposée contenir au moins un caractère "end_delim", aucune gestion d'erreur n'est faite
* si la chaine n'est pas correcte
*  Paramètres :
* string expression : l'expression globale, où se trouve d'éventuelles imbrications
* int start : le début, indique toujours le caractère juste après le premier start_delim
* char start_delim : le caractère indiquant le début d'une expression ('(' ou '[')
* char end_delim : le caractère indiquant la fin d'une expression (')' ou ']')
* Retourne un entier indiquant la position du délimiteur de fin de l'expression
*/
int get_expression(string expression, int start, char start_delim, char end_delim) {
	int j = start, 
	    groups = 1, 
	    done = 0;

	while (!done) {
		if(expression.str[j] == start_delim && expression.str[(j-1)] != '"') groups++;
		else if(expression.str[j] == end_delim && expression.str[(j-1)] != '"') {
			if(groups == 1) done = 1;
			else groups--;
		}
		j++;
	}
	return j;
}


/*
* Le parseur ABNF, indique si une chaine donnée match une certaine règle ABNF
* Paramètres : 
* tree_node ** tree : un pointeur vers la tête de l'arbre (ou du sous arbre), passé par adresse
* Il pointe toujours vers la règle parent de celle qui est actuellement parsée (ou NULL pour la règle principale)
* abnf_rule * rule : La règle ABNF que l'on veut vérifier
* string str : La chaine à parser
* Retourne un entier :
* - Positif si la chaine match la règle ABNF, dans ce cas il indique la longueur de la chaine qui match 
* (donc si cet entier n'est pas égal à la longueur de la chaine donnée en entrée, ça veut dire qu'il y a des caractères en trop
* qui ne matchent pas)
* - égal à -1 si la chaine ne respecte pas la règle ABNF.
*/
int parse(tree_node ** tree, abnf_rule * rule, string str) {
	int i = 0, j, k, a, b, done, start,
	    valid = 0, match = 0;
	abnf_rule * subrule = NULL;
	string tmp_rule = {rule->expression.str, rule->expression.size},
		   tmp_str = {str.str, str.size};

	//Création et ajout dans l'arbre
	tree_node * self_node = create_node(rule, str);
	add_node(tree, self_node);

	if (!(rule->is_terminal)) {
		while (i < rule->expression.size) {
			switch (rule->expression.str[i]) {

// Ce cas permet de vérifier si une règle match une expression entre guillemets
// Ici la vérification est case-sensitive, malgré le fais qu'officiellement la grammaire ABNF stipule que
// la règle "abc" match "aBc", "AbC" etc...
// C'est une simplification que j'ai faite par manque de temps. 
				case '"': 
					i++;
					tmp_rule.str++;
					tmp_rule.size--;

					done = 0;
					match = 1;
					start = i;
					//On compare caractère par caractère, jusqu'à rencontrer le 2e " ou 
					// jusqu'a ce qu'un caractère ne match pas
					while(!done && match > 0) {
						if(rule->expression.str[i] == '"') {
							done = 1;
						} else {
							match = is_char(rule->expression.str[i], tmp_str);
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
						//Si un caractère n'a pas matché, on reset notre avancement dans la chaine:
						if(i-start > 1) {
							tmp_str.str -= i - start - 1;
							tmp_str.size += i - start - 1;
						}
						while(rule->expression.str[i] != '"') { //on avance quand même jusqu'au prochaine guillemet, pour passer à la règle suivante
							i++;
							tmp_rule.str++;
							tmp_rule.size--;
						}

					}
					i++;
					tmp_rule.str++;
					tmp_rule.size--;
					break;

// Ce cas permet de gérer les répétition ABNF. Ici la fonctionnalité 4rulename n'est pas implémentée, il faut
// forcément que le caractère '*' soit présent pour qu'une répétition soit détéctée.
				case '*':
					if(i == 0) {
						a = 0;
					} else {
						a = rule->expression.str[i-1];
					}
					b = rule->expression.str[i+1];
					//On récupère les 2 caractères entourant le '*', qui, si ils existent, seront les bornes à respecter
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
					k = 0; //k représente le nombre de fois que la règle match
					if(rule->expression.str[i] == '(') {
						j = get_expression(rule->expression, i+1, '(', ')');
						subrule = create_rule("REPETITION", strlen("REPETITION"), tmp_rule.str + 1, j - i - 1, 0, rule->elements);
					} else {
						subrule = get_subrule(rule, i);
					}
					if(subrule != NULL) {
						match = 1;
						while(match > 0 && valid != -1) {
							match = parse(&(self_node->children), subrule, tmp_str);
							if(match > 0) {
								tmp_str.str += match;
								tmp_str.size -= match;
								if(valid != -1) valid += match;
							}
							k++;
						}
						//Respectivement : si la règle a matché moins de a fois, pas exactement a fois (lorsque a = b), ou plus de b fois
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
					}
					break;

// Dans le cas d'une parenthèse, récupère la règle (ou le groupe de règles) entre parenthèses 
// et on s'appelle récursivement dessus
				case '(':
					j = get_expression(rule->expression, i+1, '(', ')');
					subrule = create_rule("GROUP", strlen("GROUP"), tmp_rule.str + 1, j - i - 1, 0, rule->elements);
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
					}
					i++;
					tmp_rule.str++;
					tmp_rule.size--;
					break;

// Cas des options
// Comme les groupes de règles, on récupère toute l'expression entre crochets et on s'appelle récursivement dessus
				case '[':
					j = get_expression(rule->expression, i+1, '[', ']');
					subrule = create_rule("OPTION", strlen("OPTION"), tmp_rule.str + 1, j-1, 0, rule->elements);
					if(subrule != NULL) {
						match = parse(&(self_node->children), subrule, tmp_str);
						if(match > 0 && valid != -1) {
							tmp_str.str += match;
							tmp_str.size -= match;
							valid += match;
						}
						i += subrule->expression.size;
						tmp_rule.str += subrule->expression.size;
						tmp_rule.size -= subrule->expression.size;
					}
					break;

// Cas de l'alternation
// Cela signifie que l'on viens de tester une possibilité seulement
// Il faut donc soit recommencer de 0 avec la règle qui suit le '/', soit sortir si la règle avant le '/' est vérifiée
				case '/':
					i++;
					tmp_rule.str++;
					tmp_rule.size--;
					if(valid == -1) { //on remet valid a 0 car il y a une autre possibilité
						valid = 0;
					} else if (valid > 0){ // Si la première possibilité est valide, on sort, pas la peine de regarder le reste
						i = rule->expression.size;
					}
					break;

// Les espaces en ABNF sont optionnels, on les ignore juste
				case ' ': 
					i++;
					tmp_rule.str++;
					tmp_rule.size--;
					break;

// Le cas par défaut est de regarder si le carctère que l'on regarde est le début d'une rulename
				default:
					subrule = get_subrule(rule, i);
					if (subrule != NULL) {
						match = parse(&(self_node->children), subrule, tmp_str);
						if (match >= 0 && valid != -1) {
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
	} else { 
// Cas des règles terminales, elles sont codées en brute, chaque nouvelle règle terminale devra être ajoutée ici
// Je sais qu'il est possible d'automatiser ce processus avec des pointeurs de fonctions, mais je n'ai pas eu le temps d'y mettre en oeuvre...
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
//CRLF fait exception, plutot que de coder CR et LF en brut j'ai directement codé la fonction CRLF, ce qui fait
// qu'elle retourne 2 et non 1 puisqu'il faut avancer de 2 caractères...
			if(str.size > 1 && is_crlf(str.str[0], str.str[1])) valid = 2; 
			else valid = -1;
		}
	}

//A la fin du parsing et des appels récursif, il faut simplement vérifier la valeur de valid
	if(valid >= 0) {
		self_node->value.size = valid; // On met à jour la taille de la chaine qui parse
	} else {
		delete_node(tree, self_node); //Sinon on supprime le noeud (ainsi que ses éventuels fils)
	}
	return valid;
}



/*int main(){
	tree_node *tree = NULL;
	abnf_rule *start = init_rules();
	//La requete à parser : (attention à échapper les caractères spéciaux)
	char *str = "GET / HTTP/1.0\r\nAccept-Charset: iso-8859-5, unicode-1-1; q=0.8 \r\n\r\n";
	int size = strlen(str);
	string request = {str, size};
	int a = parse(&tree, start, request);
	printf("*** RESULTAT = %d\n", a);
	if (a == size) printf("*** OK : \"%s\" (%d) match la règle %s\n", request.str, request.size, start->rulename.str);
	else printf("*** KO : \"%s\" (%d) ne match PAS la règle %s\n", request.str, request.size, start->rulename.str);
	print_tree(&tree);
	delete_all_node(&tree);
	delete_all_rules(&start);
	return 1;
}*/
