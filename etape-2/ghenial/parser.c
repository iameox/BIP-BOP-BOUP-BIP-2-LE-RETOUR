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

int is_char(char c, string s) {
	if(s.size == 0) return 0;
	if(c == s.str[0]) return 1;
	else return -1;
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

int bite = 0;
//valid > 0 : ok et c'est la taille qu'il faut avancer
//valid = 0 : 
//valid < 0 : règle fausse
int parse(abnf_rule * rule, string str) {
	int i = 0, j,
	    k, a, b,
	    valid = 0,
	    match = 0;
	abnf_rule * subrule = NULL;
	string tmp_rule = {rule->expression.str, rule->expression.size},
		   tmp_str = {str.str, str.size},
		   tmp_subrule;

	for(int f = 0 ; f < bite ; f++) printf("\t");
	printf(">>>ON ENTRE DANS %s ET STR = \"%s\"\n", rule->rule_name.str, str.str);
	bite++;
	

	if(!(rule->is_terminal)) {
		while(i < rule->expression.size) {
			switch(rule->expression.str[i]) {
				case '"':
					i++;
					match = is_char(rule->expression.str[i],tmp_str);
					if(match > 0) { //si règle vérifiée, on avance
						//printf("\"%c\" match un caractère entre guillemets\n", rule->expression.str[i]);
						valid += match;
						tmp_str.str += valid;
						tmp_str.size -= valid;
					}
					i += 2;
					tmp_rule.str+= 3;
					tmp_rule.size-=3;
					break;
				case '*': //récup les digits avant et après, et répéter la règle
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
					}
					i++;
					tmp_rule.str++;
					tmp_rule.size--;
					k = a;
					if(rule->expression.str[i] == '(') {
						//FAUT GERER LES CAS TYPE (C (A/B) D)
						j = 1;
						while (rule->expression.str[j] != ')' || rule->expression.str[(j-1)] == '"') j++;
						subrule = create_rule("jsp cque jfais", strlen("jsp cque jfais"), tmp_rule.str + 1, j-1, 0, rule->elements);
					} else {
						subrule = get_subrule(rule, i);
					}
					if(subrule != NULL) {
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
						if(match < 0 && k <= a+1 && a != 0) {
							valid = -1;
						} else if (k == b && match > 0) {
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
						tmp_rule.str++;
						tmp_rule.size--;
					}
					break;
				case '(': // système de stack pour les expressions parenthèsées? 
					// OWS A GERER
					//printf("parenthèses\n");
					i++;
					tmp_rule.str++;
					tmp_rule.size--;
					break;
				case '[': // optionnel, on cherche si il y est 
				case '/': //OU
					//printf("OU\n");
					i++;
					tmp_rule.str++;
					tmp_rule.size--;
					if(valid == -1) { //on remet valid a 0 car il y a une autre possibilité
						valid = 0;
					} else if (valid > 0){
						bite--;
						for(int f = 0 ; f < bite ; f++) printf("\t");
						printf("<<< MDR ON SORT DE %s, valid = %d\n", rule->rule_name.str, valid);
					 	return valid;
					}
					break;
				case ' ': //concaténation, on cherche les 2 règles à gauche et à droite
					//printf("CONCATENATION\n");
					i++;
					tmp_rule.str++;
					tmp_rule.size--;
					break;
				default:
					subrule = get_subrule(rule, i);
					if( subrule != NULL) {
						//printf("On applique la sous regle %s = \"%s\"\n", subrule->rule_name.str, subrule->expression.str);
						match = parse(subrule, tmp_str);
						if(match >= 0) { //si règle vérifiée, on avance
							valid += match;
							tmp_str.str += valid;
							tmp_str.size -= valid;
						} else {
							valid = -1;
						}
						i += subrule->rule_name.size;
						tmp_rule.str+= subrule->rule_name.size;
						tmp_rule.size-= subrule->rule_name.size;
					} else {
						i++;
						tmp_rule.str++;
						tmp_rule.size--;
					}
					
					break;
			}
			//printf("checkup, char = \"%c\" = \"%c\", i= %d \n",rule->expression.str[i], tmp_rule.str[0],i);
			
		}
	} else { //Règles terminales
		if(!strcmp(rule->rule_name.str,"ALPHA")) {
			if(str.size != 0 && is_alpha(str.str[0])) valid = 1;
			else valid = -1;
		} else if(!strcmp(rule->rule_name.str,"DIGIT")) {
			if(str.size != 0 && is_digit(str.str[0])) valid = 1;
			else valid = -1;
		} else if(!strcmp(rule->rule_name.str,"QUOTES")) {
			if(str.size != 0 && is_char(rule->expression.str[0], str)) valid = 1;
			else valid = -1;
		}
	}

	//if(valid > 0) {
	//	printf("\"%s\" (%d) match la règle %s = \"%s\", valid = %d\n", str.str, str.size, rule->rule_name.str, rule->expression.str, valid);
	//}
	bite--;
	for(int f = 0 ; f < bite ; f++) printf("\t");
	printf("<<<ON SORT DE %s, valid = %d\n", rule->rule_name.str, valid);
	return valid;
}

void test(abnf_rule *rule, char * str, int size) {
	string test = {str, size};
	int a = parse(rule, test);
	printf("*** RESULTAT = %d\n", a);
	if(a == size) printf("*** OK : \"%s\" (%d) match la règle %s = \"%s\"\n", test.str, test.size, rule->rule_name.str, rule->expression.str);
	else printf("*** KO : \"%s\" (%d) ne match PAS la règle %s = \"%s\"\n", test.str, test.size, rule->rule_name.str, rule->expression.str);
}

abnf_rule * EXISTING_RULES = NULL;

int main() {
	abnf_rule *ALPHA = create_rule("ALPHA", 5, "", 0, 1, NULL);
	abnf_rule *DIGIT = create_rule("DIGIT", 5, "", 0, 1, NULL);
	abnf_rule *QUOTE = create_rule("QUOTE", 5, "", 0, 1, NULL);

	rule_list *liste_tchar = malloc(sizeof(rule_list)), *liste_test = malloc(sizeof(rule_list)), 
			  *liste_tchar2 = malloc(sizeof(rule_list)),
			  *liste_token = malloc(sizeof(rule_list));
	liste_tchar->rule = ALPHA;
	liste_tchar->next = liste_tchar2;
	liste_tchar2->rule = DIGIT;
	liste_tchar2->next = NULL;


	char * tchar_exp = "\"!\" / \"#\" / \"$\" / \"%\" / \"&\" / \"'\" / \"*\" / \"+\" / \"-\" / \".\" / \"^\" / \"_\" / \"`\" / \"|\" / \"~\" / DIGIT / ALPHA";
	abnf_rule *tchar = create_rule("tchar", 5, tchar_exp, strlen(tchar_exp), 0, liste_tchar);

	liste_token->rule = tchar;
	liste_token->next = NULL;
//tests unitaires, enfin vite fait
	
	abnf_rule *token = create_rule("token", 5, "1*tchar", 7, 0, liste_token);
	abnf_rule *token2 = create_rule("[token]", 5, "*tchar", 6, 0, liste_token);
	abnf_rule *token13 = create_rule("tokenmaisjfaiscquejveux", 5, "1*3tchar", 8, 0, liste_token);

	test(token, "", 0);
	test(token2, "", 0);
	test(token13, "", 0);

	test(token, "bwa", 3);
	test(token2, "bwa", 3);
	test(token13, "bwa", 3);

	printf("============================================================================================\n");
	//ZONE DE TEST
	
	char * my_rule = "1*3(tchar / DIGIT)";
	abnf_rule *rtest = create_rule("test", 5, my_rule, strlen(my_rule), 0, liste_tchar);

	char * requete = "!#$%&!b!";
	test(token, requete, strlen(requete));

	return 1;
}