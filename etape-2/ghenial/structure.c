#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "structure.h"



/*
* Fonction élémentaires pour parser les règles terminales
* Toutes retournent 1 si le caractère c vérifie la règle terminale en question, 0 sinon
*/
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

int is_sp(char c) {
	return (c == 0x20);
}

int is_htab(char c) {
	return (c == 0x09);
}


/*
* CRLF fait exception, plutot que de coder CR et LF en brut j'ai directement codé la fonction CRLF, ce qui fait
* qu'elle prends 2 paramètres c1 et c2 et si ils sont respectivement égaux à CR et LF, elle retourne 1, 0 sinon.
*/
int is_crlf(char c1, char c2) {
	return (c1 == 0x0D && c2 == 0x0A);
}


/*
* Paramètres : 
* char c : le caractère que doit matcher la chaine
* string s : la chaine à vérifier
* Retourne :
* 0 si la chaine s est vide
* 1 si le premier caractère de la chaine est égal à c
* 0 sinon
*/
int is_char(char c, string s) {
	if (s.size == 0) return 0;
	if (c == s.str[0]) return 1;
	else return -1;
}


/*
* Retourne 1 si la chaine sub est une sous-chaine de str, 0 sinon
* Paramètres : 
* string sub : la sous chaine à chercher dans str
* string str : la chaine générale
*/
int is_subchain(string sub, string str) {
	int i = 0, valid = 1;

	if (sub.size > str.size) valid = 0;

	while (valid && i < sub.size) {
		if (sub.str[i] != str.str[i]) valid = 0;
		i++;
	}
	return valid;
}


/*
* Fonction qui insère en tête la nouvelle règle dans une liste chainée
* Paramètres : 
* rule_list **head : la tête de la liste chainée, passée par adresse
* abnf_rule *new_rule : un pointeur vers la règle à ajouter
*/
void insert_rule(rule_list **head, abnf_rule *new_rule) {
	rule_list *new = malloc(sizeof(rule_list));
	new->rule = new_rule;

	if (new == NULL) {
		perror("Erreur d'allocation mémoire\n");
		exit(EXIT_FAILURE);
	}

	if (*head == NULL) {
		*head = new;
		new->next = NULL;
	} else {
		new->next = *head;
		*head = new;
	}

}


/*
* Crée une nouvelle règle à partir des paramètres
* Paramètres : 
* char *str_name : un pointeur vers le nom de la règle
* int name_size : la taille du nom
* char *str_expression : un pointeur vers l'expression de la règle
* int expression_size : la taille de l'expression
* int is_terminal : 0 pour une règle non terminale, 1 si la règle est terminale
* (terminale : si la règle n'a aucune sous-règles)
* rule_list *elements : pointeur vers une liste de règles désignant les sous règles dont dépendent la règle
*/
abnf_rule *create_rule(char *str_name, int name_size, char *str_expression, int expression_size, int is_terminal, rule_list *elements) {
	abnf_rule *new = malloc(sizeof(abnf_rule));

	if (new == NULL) {
		perror("Erreur d'allocation mémoire\n");
		exit(EXIT_FAILURE);
	}

	string rulename = {str_name, name_size};
	string expression = {str_expression, expression_size};

	new->rulename = rulename;
	new->expression = expression;
	new->is_terminal = is_terminal;
	new->elements = elements;

	return new;
}


/*
* Retourne la sous règle qui commencé à l'indice start dans l'expression de la règle rule
sous forme de structure règle abnf
* Paramètres : 
* abnf_rule *rule : la règle concernée (les sous règles seront récupérées via rule->elements)
* int start : l'indice dans la chaine rule->expression où il faut chercher la sous-règle
* Retourne un pointeur vers la sous règle trouvée, NULL si elle n'est pas trouvée
*/
abnf_rule *get_subrule(abnf_rule *rule, int start) {
	int found = 0;
	rule_list *r = rule->elements;
	string tmp = {rule->expression.str + start, rule->expression.size};
	while (!found && r != NULL) {
		if (is_subchain(r->rule->rulename, tmp)) found = 1;
		else r = r->next;
	}

	if(r != NULL) return r->rule;
	else return NULL;
}


/*
* Crée un noeud pour l'arbre de dérivation
* Paramètres : 
* abnf_rule *rule : la règle abnf vérifiée par le noeud
* string value : la chaine parsée par ce noeud
*/
tree_node *create_node(abnf_rule *rule, string value) {
	tree_node *new = malloc(sizeof(tree_node));

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


/*
* Fonction d'affichage, utile seulement pour le débug
* Je l'ai laissée, si éventuellement vous rencontrez des problèmes avec mon code...
* Paramètres : 
* tree_node **tree : la racine de l'arbre passée par adresse
*/
void print_tree(tree_node **tree) {
	tree_node *n;
	int i;
	if(*tree == NULL) printf("VIDE\n");
	else {
		n = *tree;
		if(n->children != NULL) print_tree(&(n->children));
		if(n->next != NULL) print_tree(&(n->next));
		printf("Noeud : rulename = %s value =", n->rule->rulename.str);
		printf("\"");
		for(i = 0 ; i < n->value.size ; i ++) printf("%c", n->value.str[i]);
		printf("\"\n");
	}
}


/*
* Ajoute un noeud à l'arbre (ou au sous-arbre) de dérivation passé en paramètre.
* Paramètres : 
* tree_node **tree : la racine de l'arbre passée par adresse
* tree_node *self_node : un pointeur vers le noeud à ajouter
*/
void add_node(tree_node **tree, tree_node *self_node) {
	if(*tree == NULL) {
		*tree = self_node;
		//self_node->next = NULL;
		//self_node->children = NULL;
	} else {
		self_node->next = (*tree)->children;
		(*tree)->children = self_node;
	}
}


/*
* Supprime un noeud à l'arbre (ou au sous-arbre) de dérivation, et ses noeuds enfants si il en a.
* Paramètres : 
* tree_node **tree : la racine de l'arbre passée par adresse
* tree_node *self_node : un pointeur vers le noeud à supprimer
*/
void delete_node(tree_node **tree, tree_node *self_node) {
	tree_node *n = *tree;
	if(*tree == self_node) {
		*tree = NULL;
	} else if((*tree)->children == self_node) { //Si on est le premier fils
		(*tree)->children = self_node->next;
	} else {
		n = (*tree)->children; //Sinon, on récupère un pointeur vers le précédent pour pouvoir s'extraire de la liste
		while(n->next != self_node) {
			n = n->next;
		}
		n->next = self_node->next;
	}
	if(self_node->children != NULL) { //On s'appelle récursivement pour supprimer les noeuds enfants
		delete_node(&self_node, self_node->children);
	}
	free(self_node); //On libère la mémoire, mais pas la règle ABNF. Cette dernière sera libérée par une autre fonction.
}


/*
* Supprime tous les noeud de l'arbre (ou au sous-arbre) de dérivation passé en paramètre.
* Paramètres : 
* tree_node **tree : la racine de l'arbre passée par adresse
*/
void delete_all_node(tree_node ** tree) {
	abnf_rule *test;
	if(*tree != NULL) {
		tree_node * self_node = *tree;
		if((*tree)->children != NULL) { //On supprime récursivement tous les noeuds fils
			delete_all_node(&(*tree)->children);
		}
		if((*tree)->next != NULL) { //On supprime récursivement tous les noeuds frères
			delete_all_node(&(*tree)->next);
		}

		(*tree) = NULL;
		free(self_node); //On libère le noeud
	}
}
