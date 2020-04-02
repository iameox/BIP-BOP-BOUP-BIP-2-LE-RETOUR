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

int is_crlf(char c1, char c2) {
	return(c1 == 0x0D && c2 == 0x0A);
}

int is_sp(char c) {
	return(c == 0x20);
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

void insert_rule(rule_list ** head, abnf_rule * new_rule) {
	//rule_list * r = head;
	rule_list * new = malloc(sizeof(rule_list));
	new->rule = new_rule;

	if(new == NULL) {
		perror("Erreur de malloc\n");
		exit(EXIT_FAILURE);
	}

	if(*head == NULL) {
		*head = new;
		new->next = NULL;
	} else {
		new->next = (*head);
		*head = new;
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

void printList(rule_list * head) {
	rule_list * r = head;
	while(r != NULL) {
		printf("Règle : %s\n", r->rule->rule_name.str);
		r = r->next;
	}
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

void printntruc(char* str, int size) {
	int i;
	printf("\"");
	for(i = 0 ; i < size ; i ++) printf("%c", str[i]);
	printf("\"\n");
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
		   tmp_str = {str.str, str.size};

	//for(int f = 0 ; f < bite ; f++) printf("\t");
	//printf(">>>ON ENTRE DANS %s ET STR = \"%s\"\n", rule->rule_name.str, str.str);
	bite++;

	if(!(rule->is_terminal)) {
		while(i < rule->expression.size) {
			switch(rule->expression.str[i]) {
				case '"':
					i++;
					match = is_char(rule->expression.str[i],tmp_str);
					//printf("%c == \"%c\" ?\n", rule->expression.str[i], tmp_str.str[0]);
					if(match > 0) { //si règle vérifiée, on avance
						//printf("\"%c\" match un caractère entre guillemets\n", rule->expression.str[i]);
						valid += match;
						tmp_str.str += match;
						tmp_str.size -= match;
					} else {
						valid = -1;
						//return valid;
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
						subrule = create_rule("GROUPE", strlen("GROUPE"), tmp_rule.str + 1, j-1, 0, rule->elements);
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
						//On incrémente les index, selon si on a utilisé une sous règle personnalisée ou pas
						if(rule->expression.str[i] == '(') {
							i += subrule->expression.size;
							tmp_rule.str += subrule->expression.size;
							tmp_rule.size -= subrule->expression.size;
						} else {
							i += subrule->rule_name.size;
							tmp_rule.str += subrule->rule_name.size;
							tmp_rule.size -= subrule->rule_name.size;
						}
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
					j = 1;
					while (tmp_rule.str[j] != ']' || tmp_rule.str[(j-1)] == '"') j++;
					
					subrule = create_rule("OPTIONNEL", strlen("OPTIONNEL"), tmp_rule.str + 1, j-1, 0, rule->elements);
					if(subrule != NULL) {
						//printf("On va appliquer %s = \"%s\" de %d à %d sur la chaine \"%s\"\n", subrule->rule_name.str, subrule->expression.str,a,b, tmp_str.str);
						match = parse(subrule, tmp_str);
						if(match > 0) { //si règle vérifiée, on avance
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
					} else if (valid > 0){
						bite--;
						//for(int f = 0 ; f < bite ; f++) printf("\t");
						//printf("<<< MDR ON SORT DE %s, valid = %d\n", rule->rule_name.str, valid);
						if(valid > 0) {
							printf("LA CHAINE QUI PARSE LA REGLE %s = ", rule->rule_name.str);
							printntruc(str.str, valid);
						}
					 	return valid;
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
					if( subrule != NULL) {
						//printf("On applique la sous regle %s = \"%s\"\n", subrule->rule_name.str, subrule->expression.str);
						match = parse(subrule, tmp_str);
						if(match >= 0) { //si règle vérifiée, on avance
							valid += match;
							tmp_str.str += match;
							tmp_str.size -= match;

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
			//printf("TMP_RULE = ");
			//printntruc(tmp_rule.str, tmp_rule.size);
			//printf("TMP_STR = ");
			//printntruc(tmp_str.str, tmp_str.size);
			//printf("RULE     = ");
			//printntruc(rule->expression.str + i, rule->expression.size-i);
			
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
		} else if(!strcmp(rule->rule_name.str,"SP")) {
			if(str.size != 0 && is_sp(str.str[0])) valid = 1;
			else valid = -1;
		} else if(!strcmp(rule->rule_name.str,"CRLF")) {
			if(str.size > 1 && is_crlf(str.str[0], str.str[1])) valid = 2;
			else valid = -1;
		}
	}

	if(valid > 0) {
		printf("LA CHAINE QUI PARSE LA REGLE %s = ", rule->rule_name.str);
		printntruc(str.str, valid);
	}
	bite--;
	//for(int f = 0 ; f < bite ; f++) printf("\t");
	//printf("<<<ON SORT DE %s, valid = %d\n", rule->rule_name.str, valid);
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

int main(int argc, char * argv[]) {
	abnf_rule *ALPHA = create_rule("ALPHA", 5, "", 0, 1, NULL);
	abnf_rule *DIGIT = create_rule("DIGIT", 5, "", 0, 1, NULL);
	abnf_rule *SP = create_rule("SP", 2, "", 0, 1, NULL);
	abnf_rule *CRLF = create_rule("CRLF", 4, "", 0, 1, NULL);
	//abnf_rule *QUOTE = create_rule("QUOTE", 5, "", 0, 1, NULL);


	char * tchar_name = "tchar";
	char * tchar_exp = "\"!\" / \"#\" / \"$\" / \"%\" / \"&\" / \"'\" / \"*\" / \"+\" / \"-\" / \".\" / \"^\" / \"_\" / \"`\" / \"|\" / \"~\" / DIGIT / ALPHA";
	rule_list *liste_tchar = NULL;
	insert_rule(&liste_tchar, ALPHA);
	insert_rule(&liste_tchar, DIGIT);
	abnf_rule *tchar = create_rule(tchar_name, strlen(tchar_name), tchar_exp, strlen(tchar_exp), 0, liste_tchar);


	char * token_name = "token";
	char * token_expr = "1*tchar";
	rule_list *liste_token = NULL;
	insert_rule(&liste_token, tchar);
	abnf_rule *token = create_rule(token_name, strlen(token_name), token_expr,strlen(token_expr), 0, liste_token);

	abnf_rule *token2 = create_rule("[token]", 7, "*tchar", 6, 0, liste_token);
	abnf_rule *token13 = create_rule("tokenmaisjfaiscquejveux", strlen("tokenmaisjfaiscquejveux"), "1*3tchar", 8, 0, liste_token);

	//method = token
	char * method_name = "method";
	char * method_expr = "token";
	rule_list *liste_method = NULL;
	insert_rule(&liste_method, token);
	abnf_rule *method = create_rule(method_name, strlen(method_name), method_expr, strlen(method_expr), 0, liste_method);

	//HTTP-name = "H" "T" "T" "P" (simplifié)
	char * HTTP_name_name = "HTTP-name";
	char * HTTP_name_expr = "\"H\" \"T\" \"T\" \"P\""; // ATTENTION CASE INSENTIVE/SENSITIVE A GERER
	rule_list *liste_HTTP_name = NULL;
	abnf_rule *HTTP_name = create_rule(HTTP_name_name, strlen(HTTP_name_name), HTTP_name_expr, strlen(HTTP_name_expr), 0, liste_HTTP_name);

	//HTTP-version = HTTP-name "/" DIGIT "." DIGIT 
	char * HTTP_version_name = "HTTP-version";
	char * HTTP_version_expr = "HTTP-name \"/\" DIGIT \".\" DIGIT ";
	rule_list *liste_HTTP_version = NULL;
	insert_rule(&liste_HTTP_version, HTTP_name);
	insert_rule(&liste_HTTP_version, DIGIT);
	abnf_rule *HTTP_version = create_rule(HTTP_version_name, strlen(HTTP_version_name), HTTP_version_expr, strlen(HTTP_version_expr), 0, liste_HTTP_version);

	//HEXDIG = DIGIT / "A" / "B" / "C" / "D" / "E" / "F"
	char * HEXDIG_name = "HEXDIG";
	char * HEXDIG_expr = "DIGIT / \"A\" / \"B\" / \"C\" / \"D\" / \"E\" / \"F\"";
	rule_list *liste_HEXDIG = NULL;
	insert_rule(&liste_HEXDIG, DIGIT);
	abnf_rule *HEXDIG = create_rule(HEXDIG_name, strlen(HEXDIG_name), HEXDIG_expr, strlen(HEXDIG_expr), 0, liste_HEXDIG);

	//unreserved = ALPHA / DIGIT / "-" / "." / "_" / "~" 
	char * unreserved_name = "unreserved";
	char * unreserved_expr = "ALPHA / DIGIT / \"-\" / \".\" / \"_\" / \"~\"";
	rule_list *liste_unreserved = NULL;
	insert_rule(&liste_unreserved, ALPHA);
	insert_rule(&liste_unreserved, DIGIT);
	abnf_rule *unreserved = create_rule(unreserved_name, strlen(unreserved_name), unreserved_expr, strlen(unreserved_expr), 0, liste_unreserved);

	//pct-encoded = "%" HEXDIG HEXDIG
	char * pct_encoded_name = "pct-encoded";
	char * pct_encoded_expr = "\"%\" HEXDIG HEXDIG";
	rule_list *liste_pct_encoded = NULL;
	insert_rule(&liste_pct_encoded, HEXDIG);
	abnf_rule *pct_encoded = create_rule(pct_encoded_name, strlen(pct_encoded_name), pct_encoded_expr, strlen(pct_encoded_expr), 0, liste_pct_encoded);

	//sub-delims = "!" / "$" / "&" / "'" / "(" / ")" / "*" / "+" / "," / ";" / "="
	char * sub_delims_name = "sub-delims";
	char * sub_delims_expr = "\"!\" / \"$\" / \"&\" / \"'\" / \"(\" / \")\" / \"*\" / \"+\" / \",\" / \";\" / \"=\"";
	rule_list *liste_sub_delims = NULL;
	abnf_rule *sub_delims = create_rule(sub_delims_name, strlen(sub_delims_name), sub_delims_expr, strlen(sub_delims_expr), 0, liste_sub_delims);

	//pchar = unreserved / pct-encoded / sub-delims / ":" / "@" 
	char * pchar_name = "pchar";
	char * pchar_expr = "unreserved / pct-encoded / sub-delims / \":\" / \"@\"";
	rule_list *pchar_list = NULL;
	insert_rule(&pchar_list, unreserved);
	insert_rule(&pchar_list, pct_encoded);
	insert_rule(&pchar_list, sub_delims);
	abnf_rule *pchar = create_rule(pchar_name, strlen(pchar_name), pchar_expr, strlen(pchar_expr), 0, pchar_list);

	//segment = *pchar
	char * segment_name = "segment";
	char * segment_expr = "*pchar";
	rule_list *segment_list = NULL;
	insert_rule(&segment_list, pchar);
	abnf_rule *segment = create_rule(segment_name, strlen(segment_name), segment_expr, strlen(segment_expr), 0, segment_list);

	//absolute-path = 1*( "/" segment )
	char * absolute_path_name = "absolute-path";
	char * absolute_path_expr = "1*( \"/\" segment )";
	rule_list *absolute_path_list = NULL;
	insert_rule(&absolute_path_list, segment);
	abnf_rule *absolute_path = create_rule(absolute_path_name, strlen(absolute_path_name), absolute_path_expr, strlen(absolute_path_expr), 0, absolute_path_list);

	//query = *( pchar / "/" / "?" )
	char * query_name = "query";
	char * query_expr = "*( pchar / \"/\" / \"?\" )";
	rule_list *query_list = NULL;
	insert_rule(&query_list, pchar);
	abnf_rule *query = create_rule(query_name, strlen(query_name), query_expr, strlen(query_expr), 0, query_list);

	//origin-form = absolute-path [ "?" query ]
	char * origin_form_name = "origin-form";
	char * origin_form_expr = "absolute-path [ \"?\" query ]";
	rule_list *origin_form_list = NULL;
	insert_rule(&origin_form_list, absolute_path);
	insert_rule(&origin_form_list, query);
	abnf_rule *origin_form = create_rule(origin_form_name, strlen(origin_form_name), origin_form_expr, strlen(origin_form_expr), 0, origin_form_list);

	//request-target = origin-form
	char * request_target_name = "request-target";
	char * request_target_expr = "origin-form";
	rule_list *request_target_list = NULL;
	insert_rule(&request_target_list, origin_form);
	abnf_rule *request_target = create_rule(request_target_name, strlen(request_target_name), request_target_expr, strlen(request_target_expr), 0, request_target_list);

	//request-line = method SP request-target SP HTTP-version CRLF
	char * request_line_name = "request-line";
	char * request_line_expr = "method SP request-target SP HTTP-version CRLF";
	rule_list *request_line_list = NULL;
	insert_rule(&request_line_list, method);
	insert_rule(&request_line_list, SP);
	insert_rule(&request_line_list, request_target);
	insert_rule(&request_line_list, HTTP_version);
	insert_rule(&request_line_list, CRLF);
	abnf_rule *request_line = create_rule(request_line_name, strlen(request_line_name), request_line_expr, strlen(request_line_expr), 0, request_line_list);

	//start-line = request-line
	char * start_line_name = "start-line";
	char * start_line_expr = "request-line";
	rule_list *start_line_list = NULL;
	insert_rule(&start_line_list, request_line);
	abnf_rule *start_line = create_rule(start_line_name, strlen(start_line_name), start_line_expr, strlen(start_line_expr), 0, start_line_list);



	//Accept-Charset-header

	//Referer-header

	//header-field = Accept-Charset-header / Referer-header

	//HTTP-message = start-line *( header-field CRLF ) CRLF [ message-body ]
	char * HTTP_message_name = "HTTP-message";
	char * HTTP_message_expr = "start-line *( header-field CRLF ) CRLF [ message-body ]";
	rule_list *HTTP_message_list = NULL;
	insert_rule(&HTTP_message_list, start_line);
	//insert_rule(&HTTP_message_list, header_field);
	insert_rule(&HTTP_message_list, CRLF);
	//insert_rule(&HTTP_message_list, message_body);
	abnf_rule *HTTP_message = create_rule(HTTP_message_name, strlen(HTTP_message_name), HTTP_message_expr, strlen(HTTP_message_expr), 0, HTTP_message_list);

	

	

//tests unitaires, enfin vite fait
	/*test(token, "", 0);
	test(token2, "", 0);
	test(token13, "", 0);

	test(token, "bwa", 3);
	test(token2, "bwa", 3);
	test(token13, "bwa", 3);

	test(token, "!#$%&!b!", strlen("!#$%&!b!"));

	test(method, "GET", strlen("GET"));

	test(pct_encoded, "%AF", strlen("%AF"));

	test(start_line, "POST /cgi-bin/process.cgi HTTP/1.1\r\n", strlen("POST /cgi-bin/process.cgi HTTP/1.1\r\n"));*/

	

	printf("============================================================================================\n");
	//ZONE DE TEST
	
	char * my_rule = "ALPHA [ DIGIT ] ALPHA";
	abnf_rule *rtest = create_rule("test", 5, my_rule, strlen(my_rule), 0, liste_tchar);

	char * requete = "GET / HTTP/1.1\r\n";
	test(start_line, requete, strlen(requete));

	return 1;
}