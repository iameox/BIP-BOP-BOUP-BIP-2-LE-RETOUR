#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "api.h"
#include "parser.h"



//Arbre global, pour pouvoir le récupérer avec getRootTree ET le faire passer au parseur
tree_node * tree = NULL;


void *getRootTree() {
	return tree;
}


/*
* Fonction permettant d'ajouter un Token à une liste chainée.
* Paramètres : list la tête de la liste chainée ; element l'élément à insérer 
*/
void addToken(_Token **list, _Token *element) {
	if(*list == NULL) {
		*list = element;
	} else {
		element->next = *list;
		*list = element;
	}
}


/*
* Fonction qui recherche dans l'arbre tous les noeuds dont l'etiquette est egale à la chaine de caractères en argument.   
* Paramètres : start = racine de l'arbre ou du sous arbre, si start = NULL on prends la racine
* name est le nom de la règle à chercher
* Retourne un pointeur vers une liste chainée contenant tous les éléments matchant la règle.
*/
_Token *searchTree(void *start,char *name) {
	_Token *result = NULL, *token = NULL;
	tree_node *root;

	if (start == NULL) {
		root = getRootTree();
	} else {
		root = start;
	}

	if (root->next != NULL) {
		token = searchTree(root->next, name);
		if (token != NULL) {
			addToken(&result, token);
		}
	}
	if (root->children != NULL) {
		token = searchTree(root->children, name);
		if (token != NULL) {
			addToken(&result, token);
		}
	}

	if (!strcmp(root->rule->rulename.str, name)) {
		token = malloc(sizeof(_Token));
		if (token == NULL) {
			perror("Erreur d'allocation mémoire\n");
			exit(EXIT_FAILURE);
		}
		token->node = root;
		token->next = NULL;
		addToken(&result, token);

	}


	return result;
}


/*
* Fonction qui renvoie un pointeur vers char indiquant l'etiquette du noeud. (le nom de la rulename, intermediaire ou terminal) 
* Paramètres : node est un pointeur vers tree_node, qui est ici le noeud que l'on veut analyser. 
* len est passé par adresse et recevra, si il est différent de NULL, la longueur de la chaine.
* Retourne un pointeur vers le début de la chaine de caractères indiquant le tag du noeud.
*/
char *getElementTag(void *node,int *len) {
	tree_node *element = node;
	if (len != NULL) *len = element->rule->rulename.size;
	return element->rule->rulename.str;
}


/*
* Fonction qui renvoie un pointeur vers char indiquant la valeur du noeud. (la partie correspondnant à la rulename dans la requete HTTP )
* Paramètres : node est un pointeur vers tree_node, qui est ici le noeud que l'on veut analyser. 
* len est passé par adresse et recevra, si il est différent de NULL, la longueur de la chaine.
* Retourne un pointeur vers le début de la chaine de caractères indiquant la valeur du noeud.
*/
char *getElementValue(void *node,int *len) {
	tree_node *element = node;
	if (len != NULL) *len = element->value.size;
	return element->value.str;
}


/* 
* Fonction qui supprime et libere la liste chainée de reponse.
* Paramètres : r la tête de la liste chainée à vider.
*/
void purgeElement(_Token **r) {
	_Token *t1, *t2;
	if (*r != NULL) {
		t1 = *r;
		while (t1->next != NULL) {
			t2 = t1->next;
			t1->next = t2->next;
			free(t2);
		}
		free(t1);
	}
}


/*
* Fonction qui supprime et libere toute la mémoire associée à l'arbre .
* Paramètres : root, la racine de l'arbre.
*/
void purgeTree(void *root) {
	tree_node *tree_root = (tree_node *) root;
	delete_all_rules(&(tree_root->rule));
	delete_all_node(&root); //On fait ici appel à une autre fonction définie dans functions.h
}


/*
* L'appel à votre parser un char* et une longueur à parser.  
* Paramètres : req, pointeur vers le début de la requète ; et len la longueur à parser
* Retourne 1 si la requète est valide, 0 sinon
*/
int parseur(char *req, int len) {
	abnf_rule *main_rule = init_rules();
	string request = {req, len};
	return (parse(&tree, main_rule, request) == len);
}
