#include "api.h"
#include "parser.h"

tree_node * tree = NULL;

void *getRootTree() {
	return tree;
}

void addToken(_Token ** list, _Token * element) {
	if(*list == NULL) {
		*list = element;
	} else {
		element->next = *list;
		*list = element;
	}
}

// Fonction qui recherche dans l'arbre tous les noeuds dont l'etiquette est egale à la chaine de caractères en argument.   
// Par convention si start == NULL alors on commence à la racine 
// sinon on effectue une recherche dans le sous-arbre à partir du noeud start 
_Token *searchTree(void *start,char *name); 

// fonction qui renvoie un pointeur vers char indiquant l'etiquette du noeud. (le nom de la rulename, intermediaire ou terminal) 
// et indique (si len!=NULL) dans *len la longueur de cette chaine.
char *getElementTag(void *node,int *len) {
	tree_node * element = node;
	if(len != NULL) *len = element->rule->rulename.size;
	return element->rule->rulename.str;
}

// fonction qui renvoie un pointeur vers char indiquant la valeur du noeud. (la partie correspondnant à la rulename dans la requete HTTP ) 
// et indique (si len!=NULL) dans *len la longueur de cette chaine.
char *getElementValue(void *node,int *len) {
	tree_node * element = node;
	if(len != NULL) *len = element->value.size;
	return element->value.str;
}

// Fonction qui supprime et libere la liste chainée de reponse. 
void purgeElement(_Token **r); 

// Fonction qui supprime et libere toute la mémoire associée à l'arbre . 
void purgeTree(void *root) {
	delete_node(root);
}

// L'appel à votre parser un char* et une longueur à parser.  
int parseur(char *req, int len) {
	abnf_rule* main_rule = init_rules();
	string request = {req, len};
	return (parse(&tree, main_rule, request) == len);
}