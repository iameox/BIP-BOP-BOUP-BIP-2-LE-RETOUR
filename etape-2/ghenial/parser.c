#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TERMINAL_RULES {"DIGIT","ALPHA"}

typedef struct string {
	char * str;
	int size;
} string;

typedef struct abnf_rule {
	string rule_name;
	string expression;
	int is_valid;
	int is_terminal;
	struct rule_list * elements;
} abnf_rule;

typedef struct rule_list {
	abnf_rule * rule;
	struct rule_list * next;
} rule_list;


int is_upper_alpha(char c) {
	return(c >= 0x41 && c <= 0x5a);
}

int is_lower_alpha(char c) {
	return(c >= 0x61 && c <= 0x7a);
}

int is_alpha(char c) {
	return is_upper_alpha(c) || is_lower_alpha(c);
}

int is_digit(char c) {
	return(c >= 0x30 && c <= 0x39);
}


//Retourne 1 si la chaine sub est présente dans str à partir de start
/*int is_subchain(string sub, string str, int start) {
	int i = start, j = 0, valid = 1;


	if(sub.size > (str.size-start) || start > str.size) valid = 0;

	while(valid && j < sub.size && i < str.size) {
		if(sub.str[j] != str.str[i]) {
			valid = 0;
		}
		i++;
		j++;
	}

	return valid;
}*/

//Retourne 1 si la chaine sub est présente dans str
int is_subchain(string sub, string str) {
	int i = 0, valid = 1;

	if(sub.size > str.size) valid = 0;

	while(valid && i < sub.size) {
		if(sub.str[i] != str.str[i]) {
			valid = 0;
		}
		i++;
	}

	return valid;
}

void insert_rule(rule_list * head, abnf_rule * new_rule) {
	//rule_list * r = head;
	rule_list * new = malloc(sizeof(rule_list));
	new->rule = new_rule;

	if(new == NULL) {
		perror("Erreur de malloc\n");
		exit(EXIT_FAILURE);
	}

	if(head == NULL) {
		head = new;
		new->next = NULL;
	} else {
		new->next = head->next; 
		head = new;
	}

}

abnf_rule * create_rule(char * str_name, int name_size, char* str_expression, int expression_size, int is_terminal, rule_list * elements) {
	abnf_rule *new = malloc(sizeof(abnf_rule));
	string rule_name = {str_name, name_size};
	string expression = {str_expression, expression_size};

	new->rule_name = rule_name;
	new->expression = expression;
	new->is_valid = -1;
	new->is_terminal = is_terminal;
	new->elements = elements;

	return new;
}


abnf_rule * get_subrule(abnf_rule * rule, int start) {
	int found = 0;
	rule_list * r = rule->elements;
	string tmp = {rule->expression.str + start, rule->expression.size};
	//printf("je cherche une sous règle de %s a partir de %d\n", rule->rule_name.str, start);
	while(!found && r != NULL) {
		if(is_subchain(r->rule->rule_name, tmp)) {
			found = 1;
			//printf("trouvé, %s est sous chaine de %s à partir de %d\n", r->rule->rule_name.str, rule->expression.str, start);
		} else {
			r = r->next;
		}
	}

	if(r != NULL) return r->rule;
	else return NULL;
}

//valid > 0 : ok et c'est la taille qu'il faut avancer
//valid = 0 : 
//valid < 0 : règle fausse
int parse(abnf_rule * rule, string str) {
	int i = 0,
	    k, a, b,
	    valid = 0,
	    match = 0;
	abnf_rule * subrule = NULL;
	string tmp_rule = {rule->expression.str, rule->expression.size},
		   tmp_str = {str.str, str.size},
		   parenthese;

	if(!(rule->is_terminal)) {
		while(i < rule->expression.size && valid != -1) {
			switch(rule->expression.str[i]) {
				case '*': //récup les digits avant et après, et répéter la règle
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
					}
					i++;
					k = a;
					subrule = get_subrule(rule, i);
					if( subrule != NULL) {
						//printf("On va appliquer %s = \"%s\" de %d à %d sur la chaine \"%s\"\n", subrule->rule_name.str, subrule->expression.str,a,b, tmp_str.str);
						match = 1;
						while(k != b && match > 0) {
							match = parse(subrule, tmp_str);
							//printf("match = %d, k = %d\n", match,k);
							if(match > 0) { //si règle vérifiée, on avance
								tmp_str.str += match;
								tmp_str.size -= match;
								valid += match;
							}
							k++;
						}

						if(match < 0 && k >= a && k < b) {
							valid = -1;
						} else if (k <= b && match > 0) {
							match = parse(subrule, tmp_str);
							if(match > 0) { //si règle vérifiée, on avance
								tmp_str.str += match;
								tmp_str.size -= match;
								valid += match;
							}
						}
						i += subrule->rule_name.size;
					} else {
						i++;
					}
					break;
				case '(': // système de stack pour les expressions parenthèsées? 
					// OWS A GERER
					i++;
					break;
				case '[': // optionnel, on cherche si il y est 
				case '/': //OU
					i++;
					if(valid == -1) { //on remet valide a 0 car il y a une autre possibilité
						valid = 0;
					}
					break;
				case ' ': //concaténation, on cherche les 2 règles à gauche et à droite
					i++;
					break;
				default:
					subrule = get_subrule(rule, i);
					if( subrule != NULL) {
						//printf("On applique la sous regle %s = \"%s\"\n", subrule->rule_name.str, subrule->expression.str);
						valid = parse(subrule, tmp_str);
						if(valid > 0) { //si règle vérifiée, on avance
							//printf("ok, on avance de %d\n", valid);
							tmp_str.str += valid;
							tmp_str.size -= valid;
						}
						i += subrule->rule_name.size;
					} else {
						i++;
						printf("jsp\n");
					}
					
					break;
			}
			tmp_rule.str++;
			tmp_rule.size--;
		}
	} else { //Règles terminales
		if(!strcmp(rule->rule_name.str,"ALPHA")) {
			if(str.size == 0) return 0;
			else if(is_alpha(str.str[0])) return 1;
			else return -1;
		} else if(!strcmp(rule->rule_name.str,"DIGIT")) {
			if(str.size == 0) return 0;
			else if(is_digit(str.str[0])) return 1;
			else return -1;
		}
	}

	return valid;
}

void test(abnf_rule *rule, char * str, int size) {
	string test = {str, size};
	int a = parse(rule, test);
	printf("RESULTAT = %d\n", a);
	if(a == size) printf("OK\n");
	else printf("\n\nTEST RATE NUL NUL NUL\n\n");
}

abnf_rule * EXISTING_RULES = NULL;

int main() {
	abnf_rule *ALPHA = create_rule("ALPHA", 5, "", 0, 1, NULL);
	abnf_rule *DIGIT = create_rule("DIGIT", 5, "", 0, 1, NULL);

	rule_list *liste_tchar = malloc(sizeof(rule_list)), *liste_test = malloc(sizeof(rule_list));
	liste_tchar->rule = ALPHA;
	liste_tchar->next = NULL;


	abnf_rule *tchar = create_rule("tchar", 5, "ALPHA", 5, 0, liste_tchar);

	rule_list * bwa = malloc(sizeof(rule_list));

	bwa->rule = tchar;
	bwa->next = NULL;

	abnf_rule *token = create_rule("token", 5, "1*3tchar", 7, 0, bwa);
	abnf_rule *token2 = create_rule("token", 5, "*tchar", 6, 0, bwa);
	abnf_rule *token13 = create_rule("token", 5, "1*3tchar", 6, 0, bwa);

	test(token, "", 0);
	test(token2, "", 0);
	test(token13, "", 0);

	test(token, "bwa", 3);
	test(token2, "bwa", 3);
	test(token13, "bwa", 3);


	//ZONE DE TEST
	liste_test->rule = tchar;
	abnf_rule *rtest = create_rule("test", 5, "tchar / DIGIT", 6, 0, bwa);
	test(rtest, "b0", 2);

	return 1;
}