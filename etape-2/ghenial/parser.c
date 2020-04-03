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

int is_htab(char c) {
	return(c == 0x09);
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
	while(!found && r != NULL) {
		if(is_subchain(r->rule->rule_name, tmp)) {
			found = 1;
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
	int i = 0, j, groups, done, start,
	    k, a, b,
	    valid = 0,
	    match = 0;
	abnf_rule * subrule = NULL;
	string tmp_rule = {rule->expression.str, rule->expression.size},
		   tmp_str = {str.str, str.size};

	//for(int f = 0 ; f < bite ; f++) printf("\t");
	//printf(">>>ON ENTRE DANS %s ET STR = \"%s\"\n", rule->rule_name.str, str.str);
	bite++;

	if (!(rule->is_terminal)) {
		while (i < rule->expression.size) {
			switch (rule->expression.str[i]) {
				case '"': //AFAIRE FONCTION POUR LE CASE INSENTIVE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
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
						if(valid == -1) printf("NO\n");
						while(match > 0 && valid != -1) {
							match = parse(subrule, tmp_str);
							if(match > 0) { //si règle vérifiée, on avance
								tmp_str.str += match;
								tmp_str.size -= match;
								if(valid != -1) valid += match;
							}
							k++;
							//printf("k = %d\n", k);
						}
						if ((a != b && match < 0 && k < a+1 && a != 0) || (a == b && match < 0 && k != a+1) || (a != b && match < 0 && k > b+1 && b != -1)) {
							//printf("a = %d b = %d k = %d \n", a,b,k);
							valid = -1;
						}
						//On incrémente les index, selon si on a utilisé une sous règle groupée ou pas
						if(rule->expression.str[i] == '(') {
							i += subrule->expression.size;
							tmp_rule.str += subrule->expression.size;
							tmp_rule.size -= subrule->expression.size;
						} else {
							i += subrule->rule_name.size;
							tmp_rule.str += subrule->rule_name.size;
							tmp_rule.size -= subrule->rule_name.size;
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
						printf("\nHONO, %s\n\n", rule->rule_name.str);
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
						match = parse(subrule, tmp_str);
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
						match = parse(subrule, tmp_str);
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
						match = parse(subrule, tmp_str);
						if (match >= 0 && valid != -1) { //si règle vérifiée, on avance
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
		}  else if(!strcmp(rule->rule_name.str,"HTAB")) {
			if(str.size != 0 && is_htab(str.str[0])) valid = 1;
			else valid = -1;
		} else if(!strcmp(rule->rule_name.str,"CRLF")) {
			if(str.size > 1 && is_crlf(str.str[0], str.str[1])) valid = 2;
			else valid = -1;
		}
	}

	if(valid >= 0) {
		for(int f = 0 ; f < bite ; f++) printf("\t");
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
	printf("\n*** RESULTAT = %d\n", a);
	if(a == size) printf("*** OK : \"%s\" (%d) match la règle %s = \"%s\"\n\n", test.str, test.size, rule->rule_name.str, rule->expression.str);
	else printf("*** KO : \"%s\" (%d) ne match PAS la règle %s = \"%s\"\n\n", test.str, test.size, rule->rule_name.str, rule->expression.str);
}

abnf_rule * EXISTING_RULES = NULL;

int main(int argc, char * argv[]) {
	abnf_rule *ALPHA = create_rule("ALPHA", 5, "", 0, 1, NULL);
	abnf_rule *DIGIT = create_rule("DIGIT", 5, "", 0, 1, NULL);
	abnf_rule *SP = create_rule("SP", 2, "", 0, 1, NULL);
	abnf_rule *CRLF = create_rule("CRLF", 4, "", 0, 1, NULL);
	abnf_rule *HTAB = create_rule("HTAB", 4, "", 0, 1, NULL);
	//abnf_rule *QUOTE = create_rule("QUOTE", 5, "", 0, 1, NULL);

	//OWS = *( SP / HTAB )
	char * OWS_name = "OWS";
	char * OWS_exp = "*( SP / HTAB )";
	rule_list *OWS_list = NULL;
	insert_rule(&OWS_list, SP);
	insert_rule(&OWS_list, HTAB);
	abnf_rule *OWS = create_rule(OWS_name, strlen(OWS_name), OWS_exp, strlen(OWS_exp), 0, OWS_list);

	//tchar = "!" / "#" / "$" / "%" / "&" / "'" / "*" / "+" / "-" / "." / "^" / "_" / "`" / "|" / "~" / DIGIT / ALPHA
	char * tchar_name = "tchar";
	char * tchar_exp = "\"!\" / \"#\" / \"$\" / \"%\" / \"&\" / \"'\" / \"*\" / \"+\" / \"-\" / \".\" / \"^\" / \"_\" / \"`\" / \"|\" / \"~\" / DIGIT / ALPHA";
	rule_list *liste_tchar = NULL;
	insert_rule(&liste_tchar, ALPHA);
	insert_rule(&liste_tchar, DIGIT);
	abnf_rule *tchar = create_rule(tchar_name, strlen(tchar_name), tchar_exp, strlen(tchar_exp), 0, liste_tchar);

	//token = 1*tchar
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

	//charset = token
	char * charset_name = "charset";
	char * charset_expr = "token";
	rule_list *charset_list = NULL;
	insert_rule(&charset_list, token);
	abnf_rule *charset = create_rule(charset_name, strlen(charset_name), charset_expr, strlen(charset_expr), 0, charset_list);


	//qvalue = ( "0" [ "." *3DIGIT ] ) / ( "1" [ "." *3"0" ] )
	char * qvalue_name = "qvalue";
	char * qvalue_expr = "( \"0\" [ \".\" *3DIGIT ] ) / ( \"1\" [ \".\" *3\"0\" ] )";
	rule_list *qvalue_list = NULL;
	insert_rule(&qvalue_list, DIGIT);
	abnf_rule *qvalue = create_rule(qvalue_name, strlen(qvalue_name), qvalue_expr, strlen(qvalue_expr), 0, qvalue_list);
	
	//weight = OWS ";" OWS "q=" qvalue
	char * weight_name = "weight";
	char * weight_expr = "OWS \";\" OWS \"q=\" qvalue";
	rule_list *weight_list = NULL;
	insert_rule(&weight_list, OWS);
	insert_rule(&weight_list, qvalue);
	abnf_rule *weight = create_rule(weight_name, strlen(weight_name), weight_expr, strlen(weight_expr), 0, weight_list);

	//Accept-Charset = *( "," OWS ) ( ( charset / "*" ) [ weight ] ) *( OWS "," [ OWS ( ( charset / "*" ) [ weight ] ) ] )
	char * Accept_Charset_name = "Accept-Charset";
	char * Accept_Charset_expr = "*( \",\" OWS ) ( ( charset / \"*\" ) [ weight ] ) *( OWS \",\" [ OWS ( ( charset / \"*\" ) [ weight ] ) ] )";
	rule_list *Accept_Charset_list = NULL;
	insert_rule(&Accept_Charset_list, OWS);
	insert_rule(&Accept_Charset_list, charset);
	insert_rule(&Accept_Charset_list, weight);
	abnf_rule *Accept_Charset = create_rule(Accept_Charset_name, strlen(Accept_Charset_name), Accept_Charset_expr, strlen(Accept_Charset_expr), 0, Accept_Charset_list);

	//Accept-Charset-header = "Accept-Charset" ":" OWS Accept-Charset OWS
	char * Accept_Charset_header_name = "Accept-Charset-header";
	char * Accept_Charset_header_expr = "\"Accept-Charset\" \":\" OWS Accept-Charset OWS";
	rule_list *Accept_Charset_header_list = NULL;
	insert_rule(&Accept_Charset_header_list, OWS);
	insert_rule(&Accept_Charset_header_list, Accept_Charset);
	abnf_rule *Accept_Charset_header = create_rule(Accept_Charset_header_name, strlen(Accept_Charset_header_name), Accept_Charset_header_expr, strlen(Accept_Charset_header_expr), 0, Accept_Charset_header_list);

	//scheme = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
	char * scheme_name = "scheme";
	char * scheme_expr = "ALPHA *( ALPHA / DIGIT / \"+\" / \"-\" / \".\" )";
	rule_list *scheme_list = NULL;
	insert_rule(&scheme_list, ALPHA);
	insert_rule(&scheme_list, DIGIT);
	abnf_rule *scheme = create_rule(scheme_name, strlen(scheme_name), scheme_expr, strlen(scheme_expr), 0, scheme_list);

	//userinfo = *( unreserved / pct-encoded / sub-delims / ":" )
	char * userinfo_name = "userinfo";
	char * userinfo_expr = "*( unreserved / pct-encoded / sub-delims / \":\" )";
	rule_list *userinfo_list = NULL;
	insert_rule(&userinfo_list, unreserved);
	insert_rule(&userinfo_list, pct_encoded);
	insert_rule(&userinfo_list, sub_delims);
	abnf_rule *userinfo = create_rule(userinfo_name, strlen(userinfo_name), userinfo_expr, strlen(userinfo_expr), 0, userinfo_list);

	//IPvFuture = "v" 1*HEXDIG "." 1*( unreserved / sub-delims / ":" )
	char * IPvFuture_name = "IPvFuture";
	char * IPvFuture_expr = "\"v\" 1*HEXDIG \".\" 1*( unreserved / sub-delims / \":\" )";
	rule_list *IPvFuture_list = NULL;
	insert_rule(&IPvFuture_list, HEXDIG);
	insert_rule(&IPvFuture_list, unreserved);
	insert_rule(&IPvFuture_list, sub_delims);
	abnf_rule *IPvFuture = create_rule(IPvFuture_name, strlen(IPvFuture_name), IPvFuture_expr, strlen(IPvFuture_expr), 0, IPvFuture_list);

	//dec-octet = "25" %x30-35 / "2" %x30-34 DIGIT / "1" 2DIGIT / %x31-39 DIGIT / DIGIT
	char * dec_octet_name = "dec-octet";
	char * dec_octet_expr = "\"25\" ( \"0\" / \"1\" / \"2\" / \"3\" / \"4\" / \"5\" ) / \"2\" ( \"0\" / \"1\" / \"2\" / \"3\" / \"4\" ) DIGIT / \"1\" 2*2(DIGIT) / ( \"1\" / \"2\" / \"3\" / \"4\" / \"5\" / \"6\" / \"7\" / \"8\" / \"9\" ) DIGIT / DIGIT";
	rule_list *dec_octet_list = NULL;
	insert_rule(&dec_octet_list, DIGIT);
	abnf_rule *dec_octet = create_rule(dec_octet_name, strlen(dec_octet_name), dec_octet_expr, strlen(dec_octet_expr), 0, dec_octet_list);

	//IPv4address = dec-octet "." dec-octet "." dec-octet "." dec-octet
	char * IPv4address_name = "IPv4address";
	char * IPv4address_expr = "dec-octet \".\" dec-octet \".\" dec-octet \".\" dec-octet";
	rule_list *IPv4address_list = NULL;
	insert_rule(&IPv4address_list, dec_octet);
	abnf_rule *IPv4address = create_rule(IPv4address_name, strlen(IPv4address_name), IPv4address_expr, strlen(IPv4address_expr), 0, IPv4address_list);
	
	//h16 = 1*4HEXDIG
	char * h16_name = "h16";
	char * h16_expr = "1*4HEXDIG";
	rule_list *h16_list = NULL;
	insert_rule(&h16_list, HEXDIG);
	abnf_rule *h16 = create_rule(h16_name, strlen(h16_name), h16_expr, strlen(h16_expr), 0, h16_list);

	//ls32 = ( h16 ":" h16 ) / IPv4address
	char * ls32_name = "ls32";
	char * ls32_expr = "( h16 \":\" h16 ) / IPv4address";
	rule_list *ls32_list = NULL;
	insert_rule(&ls32_list, h16);
	insert_rule(&ls32_list, IPv4address);
	abnf_rule *ls32 = create_rule(ls32_name, strlen(ls32_name), ls32_expr, strlen(ls32_expr), 0, ls32_list);

	//IPv6address = 6( h16 ":" ) ls32 / "::" 5( h16 ":" ) ls32 / [ h16 ] "::" 4( h16 ":" ) ls32 / [ h16 *1( ":" h16 ) ] "::" 3( h16 ":" ) ls32 / [ h16 *2( ":" h16 ) ] "::" 2( h16 ":" ) ls32 / [ h16 *3( ":" h16 ) ] "::" h16 ":" ls32 / [ h16 *4( ":" h16 ) ] "::" ls32 / [ h16 *5( ":" h16 ) ] "::" h16 / [ h16 *6( ":" h16 ) ] "::"
	char * IPv6address_name = "IPv6address";
	char * IPv6address_expr = "6*6( h16 \":\" ) ls32 / \"::\" 5*5( h16 \":\" ) ls32 / [ h16 ] \"::\" 4*4( h16 \":\" ) ls32 / [ h16 *1( \":\" h16 ) ] \"::\" 3*3( h16 \":\" ) ls32 / [ h16 *2( \":\" h16 ) ] \"::\" 2*2( h16 \":\" ) ls32 / [ h16 *3( \":\" h16 ) ] \"::\" h16 \":\" ls32 / [ h16 *4( \":\" h16 ) ] \"::\" ls32 / [ h16 *5( \":\" h16 ) ] \"::\" h16 / [ h16 *6( \":\" h16 ) ] \"::\"";
	rule_list *IPv6address_list = NULL;
	insert_rule(&IPv6address_list, h16);
	insert_rule(&IPv6address_list, ls32);
	abnf_rule *IPv6address = create_rule(IPv6address_name, strlen(IPv6address_name), IPv6address_expr, strlen(IPv6address_expr), 0, IPv6address_list);

	//segment-nz = 1*pchar
	char * segment_nz_name = "segment-nz";
	char * segment_nz_expr = "1*pchar";
	rule_list *segment_nz_list = NULL;
	insert_rule(&segment_nz_list, HEXDIG);
	abnf_rule *segment_nz = create_rule(segment_nz_name, strlen(segment_nz_name), segment_nz_expr, strlen(segment_nz_expr), 0, segment_nz_list);

	//segment-nz-nc = 1*( unreserved / pct-encoded / sub-delims / "@" )
	char * segment_nz_nc_name = "segment-nz-nc";
	char * segment_nz_nc_expr = "1*( unreserved / pct-encoded / sub-delims / \"@\" )";
	rule_list *segment_nz_nc_list = NULL;
	insert_rule(&segment_nz_nc_list, unreserved);
	insert_rule(&segment_nz_nc_list, pct_encoded);
	insert_rule(&segment_nz_nc_list, sub_delims);
	abnf_rule *segment_nz_nc = create_rule(segment_nz_nc_name, strlen(segment_nz_nc_name), segment_nz_nc_expr, strlen(segment_nz_nc_expr), 0, segment_nz_nc_list);

	//path-abempty = *( "/" segment )
	char * path_abempty_name = "path-abempty";
	char * path_abempty_expr = "*( \"/\" segment )";
	rule_list *path_abempty_list = NULL;
	insert_rule(&path_abempty_list, segment);
	abnf_rule *path_abempty = create_rule(path_abempty_name, strlen(path_abempty_name), path_abempty_expr, strlen(path_abempty_expr), 0, path_abempty_list);

	//path-absolute = "/" [ segment-nz *( "/" segment ) ]
	char * path_absolute_name = "path-absolute";
	char * path_absolute_expr = "\"/\" [ segment-nz *( \"/\" segment ) ]";
	rule_list *path_absolute_list = NULL;
	insert_rule(&path_absolute_list, segment_nz);
	insert_rule(&path_absolute_list, segment);
	abnf_rule *path_absolute = create_rule(path_absolute_name, strlen(path_absolute_name), path_absolute_expr, strlen(path_absolute_expr), 0, path_absolute_list);

	//path-noscheme = segment-nz-nc *( "/" segment )
	char * path_noscheme_name = "path-noscheme";
	char * path_noscheme_expr = "segment-nz-nc *( \"/\" segment )";
	rule_list *path_noscheme_list = NULL;
	insert_rule(&path_noscheme_list, segment_nz_nc);
	insert_rule(&path_noscheme_list, segment);
	abnf_rule *path_noscheme = create_rule(path_noscheme_name, strlen(path_noscheme_name), path_noscheme_expr, strlen(path_noscheme_expr), 0, path_noscheme_list);

	//path-rootless = segment-nz *( "/" segment )
	char * path_rootless_name = "path-rootless";
	char * path_rootless_expr = "segment-nz *( \"/\" segment )";
	rule_list *path_rootless_list = NULL;
	insert_rule(&path_rootless_list, segment_nz);
	insert_rule(&path_rootless_list, segment);
	abnf_rule *path_rootless = create_rule(path_rootless_name, strlen(path_rootless_name), path_rootless_expr, strlen(path_rootless_expr), 0, path_rootless_list);

	//path-empty = ""
	char * path_empty_name = "path_empty";
	char * path_empty_expr = "";
	rule_list *path_empty_list = NULL;
	abnf_rule *path_empty = create_rule(path_empty_name, strlen(path_empty_name), path_empty_expr, strlen(path_empty_expr), 0, path_empty_list);

	//IP-literal = "[" ( IPv6address / IPvFuture ) "]"
	char * IP_literal_name = "IP-literal";
	char * IP_literal_expr = "\"[\" ( IPvFuture ) \"]\"";
	rule_list *IP_literal_list = NULL;
	insert_rule(&IP_literal_list, IPv6address);
	insert_rule(&IP_literal_list, IPvFuture);
	abnf_rule *IP_literal = create_rule(IP_literal_name, strlen(IP_literal_name), IP_literal_expr, strlen(IP_literal_expr), 0, IP_literal_list);

	//reg-name = *( unreserved / pct-encoded / sub-delims )
	char * reg_name_name = "reg-name";
	char * reg_name_expr = "*( unreserved / pct-encoded / sub-delims )";
	rule_list *reg_name_list = NULL;
	insert_rule(&reg_name_list, unreserved);
	insert_rule(&reg_name_list, pct_encoded);
	insert_rule(&reg_name_list, sub_delims);
	abnf_rule *reg_name = create_rule(reg_name_name, strlen(reg_name_name), reg_name_expr, strlen(reg_name_expr), 0, reg_name_list);

	//host = IP-literal / IPv4address / reg-name
	char * host_name = "host";
	char * host_expr = "IP-literal / IPv4address / reg-name";
	rule_list *host_list = NULL;
	insert_rule(&host_list, IP_literal);
	insert_rule(&host_list, IPv4address);
	insert_rule(&host_list, reg_name);
	abnf_rule *host = create_rule(host_name, strlen(host_name), host_expr, strlen(host_expr), 0, host_list);

	//port = *DIGIT
	char * port_name = "port";
	char * port_expr = "*DIGIT";
	rule_list *port_list = NULL;
	insert_rule(&port_list, IP_literal);
	abnf_rule *port = create_rule(port_name, strlen(port_name), port_expr, strlen(port_expr), 0, port_list);

	//authority = [ userinfo "@" ] host [ ":" port ]
	char * authority_name = "authority";
	char * authority_expr = "[ userinfo \"@\" ] host [ \":\" port ]";
	rule_list *authority_list = NULL;
	insert_rule(&authority_list, userinfo);
	insert_rule(&authority_list, host);
	insert_rule(&authority_list, port);
	abnf_rule *authority = create_rule(authority_name, strlen(authority_name), authority_expr, strlen(authority_expr), 0, authority_list);

	//hier-part = "//" authority path-abempty / path-absolute / path-rootless / path-empty
	char * hier_part_name = "hier-part";
	char * hier_part_expr = "\"//\" authority path-abempty / path-absolute / path-rootless / path-empty";
	rule_list *hier_part_list = NULL;
	insert_rule(&hier_part_list, authority);
	insert_rule(&hier_part_list, path_abempty);
	insert_rule(&hier_part_list, path_absolute);
	insert_rule(&hier_part_list, path_rootless);
	insert_rule(&hier_part_list, path_empty);
	abnf_rule *hier_part = create_rule(hier_part_name, strlen(hier_part_name), hier_part_expr, strlen(hier_part_expr), 0, hier_part_list);

	//absolute-URI = scheme ":" hier-part [ "?" query ]
	char * absolute_URI_name = "absolute-URI";
	char * absolute_URI_expr = "scheme \":\" hier-part [ \"?\" query ]";
	rule_list *absolute_URI_list = NULL;
	insert_rule(&absolute_URI_list, scheme);
	insert_rule(&absolute_URI_list, hier_part);
	insert_rule(&absolute_URI_list, query);
	abnf_rule *absolute_URI = create_rule(absolute_URI_name, strlen(absolute_URI_name), absolute_URI_expr, strlen(absolute_URI_expr), 0, absolute_URI_list);

	//relative-part = "//" authority path-abempty / path-absolute / path-noscheme / path-empty
	char * relative_part_name = "relative_part";
	char * relative_part_expr = "\"//\" authority path-abempty / path-absolute / path-noscheme / path-empty";
	rule_list *relative_part_list = NULL;
	insert_rule(&relative_part_list, authority);
	insert_rule(&relative_part_list, path_abempty);
	insert_rule(&relative_part_list, path_absolute);
	insert_rule(&relative_part_list, path_noscheme);
	insert_rule(&relative_part_list, path_empty);
	abnf_rule *relative_part = create_rule(relative_part_name, strlen(relative_part_name), relative_part_expr, strlen(relative_part_expr), 0, relative_part_list);

	//partial-URI = relative-part [ "?" query ]
	char * partial_URI_name = "partial_URI";
	char * partial_URI_expr = "relative-part [ \"?\" query ]";
	rule_list *partial_URI_list = NULL;
	insert_rule(&partial_URI_list, relative_part);
	insert_rule(&partial_URI_list, query);
	abnf_rule *partial_URI = create_rule(partial_URI_name, strlen(partial_URI_name), partial_URI_expr, strlen(partial_URI_expr), 0, partial_URI_list);

	//Referer = absolute-URI / partial-URI
	char * Referer_name = "Referer";
	char * Referer_expr = "absolute-URI / partial-URI";
	rule_list *Referer_list = NULL;
	insert_rule(&Referer_list, absolute_URI);
	insert_rule(&Referer_list, partial_URI);
	abnf_rule *Referer = create_rule(Referer_name, strlen(Referer_name), Referer_expr, strlen(Referer_expr), 0, Referer_list);

	//Referer-header = "Referer" ":" OWS Referer OWS
	char * Referer_header_name = "Referer-header";
	char * Referer_header_expr = "\"Referer\" \":\" OWS Referer OWS";
	rule_list *Referer_header_list = NULL;
	insert_rule(&Referer_header_list, OWS);
	insert_rule(&Referer_header_list, Referer);
	abnf_rule *Referer_header = create_rule(Referer_header_name, strlen(Referer_header_name), Referer_header_expr, strlen(Referer_header_expr), 0, Referer_header_list);


	//header-field = Accept-Charset-header / Referer-header
	char * header_field_name = "header-field";
	char * header_field_expr = "Accept-Charset-header / Referer-header";
	rule_list *header_field_list = NULL;
	insert_rule(&header_field_list, Accept_Charset_header);
	insert_rule(&header_field_list, Referer_header);
	abnf_rule *header_field = create_rule(header_field_name, strlen(header_field_name), header_field_expr, strlen(header_field_expr), 0, header_field_list);

	//message-body = ""
	char * message_body_name = "message-body";
	char * message_body_expr = "ALPHA";
	rule_list *message_body_list = NULL;
	abnf_rule *message_body = create_rule(message_body_name, strlen(message_body_name), message_body_expr, strlen(message_body_expr), 0, message_body_list);

	//HTTP-message = start-line *( header-field CRLF ) CRLF [ message-body ]
	char * HTTP_message_name = "HTTP-message";
	char * HTTP_message_expr = "start-line *( header-field CRLF ) CRLF [ message-body ]";
	rule_list *HTTP_message_list = NULL;
	insert_rule(&HTTP_message_list, start_line);
	insert_rule(&HTTP_message_list, header_field);
	insert_rule(&HTTP_message_list, CRLF);
	insert_rule(&HTTP_message_list, message_body);
	abnf_rule *HTTP_message = create_rule(HTTP_message_name, strlen(HTTP_message_name), HTTP_message_expr, strlen(HTTP_message_expr), 0, HTTP_message_list);

	

	

//tests unitaires, enfin vite fait
	test(token, "", 0);
	test(token2, "", 0);
	test(token13, "", 0);

	test(token, "bwa", 3);
	test(token2, "bwa", 3);
	test(token13, "bwa", 3);

	test(token, "!#$%&!b!", strlen("!#$%&!b!"));

	test(method, "GET", strlen("GET"));

	test(pct_encoded, "%AF", strlen("%AF"));

	test(start_line, "POST /cgi-bin/process.cgi HTTP/1.1\r\n", strlen("POST /cgi-bin/process.cgi HTTP/1.1\r\n"));

	//test(start_line, "GET / HTTP/1.1\r\n", strlen("GET / HTTP/1.1\r\n"));

	test(HTTP_message, "GET / HTTP/1.0\r\nAccept-Charset: iso-8859-5, unicode-1-1; q=0.8 \r\n\r\n", strlen("GET / HTTP/1.0\r\nAccept-Charset: iso-8859-5, unicode-1-1; q=0.8 \r\n\r\n"));

	test(HTTP_message, "GET / HTTP/1.0\r\nReferer: http://www.tutorialspoint.org/http/index.htm \r\n\r\n", strlen("GET / HTTP/1.0\r\nReferer: http://www.tutorialspoint.org/http/index.htm \r\n\r\n"));
	

	printf("============================================================================================\n");
	//ZONE DE TEST
	
	//char * my_rule = "\"Algerie\" / *ALPHA";
	//abnf_rule *rtest = create_rule("test", 5, my_rule, strlen(my_rule), 0, liste_tchar);

	//char * requete = "Algeroz";
	//test(rtest, requete, strlen(requete));

	return 1;
}

//"GET / HTTP/1.0\r\nReferer: http://www.tutorialspoint.org/http/index.htm \r\n\r\n"