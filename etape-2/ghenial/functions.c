#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "functions.h"

int is_upper_alpha(char c) {
	return (c >= 0x41 && c <= 0x5a);
}

int is_lower_alpha(char c) {
	return (c >= 0x61 && c <= 0x7a);
}

int is_alpha(char c) {
	return (is_upper_alpha(c) || is_lower_alpha(c));
}

int is_digit(char c) {
	return (c >= 0x30 && c <= 0x39);
}

int is_char(char c, string s) {
	if (s.size == 0) return 0;
	if (c == s.str[0]) return 1;
	else return -1;
}

int is_crlf(char c1, char c2) {
	return (c1 == 0x0D && c2 == 0x0A);
}

int is_sp(char c) {
	return (c == 0x20);
}

int is_htab(char c) {
	return (c == 0x09);
}

/*
* Retourne 1 si la chaine sub est présente dans str, 0 sinon
*/
int is_subchain(string sub, string str) {
	int i = 0, valid = 1;

	if (sub.size > str.size) valid = 0;

	while (valid && i < sub.size) {
		if (sub.str[i] != str.str[i]) {
			valid = 0;
		}
		i++;
	}

	return valid;
}

/*
* Insertion en tête d'une règle dans une liste de règles
*/
void insert_rule(rule_list ** head, abnf_rule * new_rule) {
	//rule_list * r = head;
	rule_list * new = malloc(sizeof(rule_list));
	new->rule = new_rule;

	if (new == NULL) {
		perror("Erreur d'allocation mémoire\n");
		exit(EXIT_FAILURE);
	}

	if (*head == NULL) {
		*head = new;
		new->next = NULL;
	} else {
		new->next = (*head);
		*head = new;
	}

}

/*
* Crée une nouvelle règle à partir des paramètres
*/
abnf_rule * create_rule(char * str_name, int name_size, char* str_expression, int expression_size, int is_terminal, rule_list * elements) {
	abnf_rule *new = malloc(sizeof(abnf_rule));

	if (new == NULL) {
		perror("Erreur d'allocation mémoire\n");
		exit(EXIT_FAILURE);
	}

	string rulename = {str_name, name_size};
	string expression = {str_expression, expression_size};

	new->rulename = rulename;
	new->expression = expression;
	new->is_valid = -1;
	new->is_terminal = is_terminal;
	new->elements = elements;

	return new;
}

/* DEBUG ASUPPR
* Fonction d'affichage de liste chainée
*/
void print_list(rule_list * head) {
	rule_list * r = head;
	while (r != NULL) {
		printf("Règle : %s\n", r->rule->rulename.str);
		r = r->next;
	}
}

/*
* Récupère la sous règle sous forme de structure abnf, en cherchant à l'indice start de l'expression de la règle principale
*/
abnf_rule * get_subrule(abnf_rule * rule, int start) {
	int found = 0;
	rule_list * r = rule->elements;
	string tmp = {rule->expression.str + start, rule->expression.size};
	while (!found && r != NULL) {
		if (is_subchain(r->rule->rulename, tmp)) {
			found = 1;
		} else {
			r = r->next;
		}
	}

	if(r != NULL) return r->rule;
	else return NULL;
}


tree_node * create_node(abnf_rule * rule, string value) {
	tree_node * new = malloc(sizeof(tree_node));

	if (new == NULL) {
		perror("Erreur d'allocation mémoire\n");
		exit(EXIT_FAILURE);
	}

	new->rule = rule;
	new->value = value;
	new->next = NULL;
	new->children = NULL;

	return new;
}

/* DEBUG ASUPPR
* Fonction d'affichage personnalisée (pour ne pas dépendre des sentinelles)
*/
void printntruc(char* str, int size) {
	int i;
	printf("\"");
	for(i = 0 ; i < size ; i ++) printf("%c", str[i]);
	printf("\"\n");
}

void print_tree(tree_node ** tree) {
	tree_node * n;
	if(*tree == NULL) printf("VIDE\n");
	else {
		n = *tree;
		if(n->children != NULL) print_tree(&(n->children));
		if(n->next != NULL) print_tree(&(n->next));
		printf("Noeud : rulename = %s value =", n->rule->rulename.str);
		printntruc(n->value.str, n->value.size);
	}
}

void add_node(tree_node ** tree, tree_node * self_node) {
	if(*tree == NULL) {
		*tree = self_node;
		self_node->next = NULL;
		self_node->children = NULL;
	} else if(*tree == self_node) {
		printf("c ça?\n");
	} else {
		self_node->next = (*tree)->children;
		(*tree)->children = self_node;
	}
}

void delete_node(tree_node ** tree, tree_node * self_node) {
	tree_node * n = *tree;
	if(*tree == self_node) {
		*tree = NULL;
	} else if((*tree)->children == self_node) { //Si on est le 1er fils
		(*tree)->children = self_node->next;
	} else {
		n = (*tree)->children;
		while(n->next != self_node) {
			n = n->next;
		}

		n->next = self_node->next;
	}
	//if(!strncmp(self_node->rule->rulename.str, "GROUPE", self_node->rule->rulename.size) || !strncmp(self_node->rule->rulename.str, "GROUPE*", self_node->rule->rulename.size) || !strncmp(self_node->rule->rulename.str, "OPTIONNEL", self_node->rule->rulename.size)) {
	//	free(self_node->rule);
	//}
	if(self_node->children != NULL) {
		delete_node(&self_node, self_node->children);
	}
	free(self_node);
}

void delete_all_node(tree_node ** tree) {
	if((*tree)->children != NULL) {
		delete_all_node(&(*tree)->children);
	}
	if((*tree)->next != NULL) {
		delete_all_node(&(*tree)->next);
	}

	free((*tree)->rule);
	free((*tree));
}