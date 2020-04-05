#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "api.h"
#include "utils.h"
#include "parseur.h"
#include "compare.h"
#include "tree.h"

/*
typedef struct _token
{
	void *node;
	struct _token *next;
} _Token;
*/


/*
typedef struct tree_node
{
    char *name;
    void *adress;
    char *content;
    int is_correct;
    struct tree_node *next_node;
    struct tree_node *children;
} tree_node;
*/

tree_node *global_tree = NULL;

// Fonction qui retourne un pointeur (type opaque) vers la racine de l'arbre construit. 
void *getRootTree()
{

    //void *to_return = global_tree->adress;
    //return to_return;

    return global_tree;
}




// Fonction qui recherche dans l'arbre tous les noeuds 
// dont l'etiquette est égale à la chaine de caractères en argument.   
// Par convention si start == NULL alors on commence à la racine 
// sinon on effectue une recherche dans le sous-arbre à partir du noeud start 

/*
    1) On part de la racine de l'arbre avec deux pointeurs 
    2) On va parcourir l'arbre jusqu'à trouver un noeud d'adresse égale à start
    3) Une fois qu'on a trouvé le noeud start, on appelle une fonction auxiliaire pour remplir to_return

    Cette fonction prend en argument un noeud à partir duquel checher et une liste à laquelle ajouter
        Pour chaque noeud parent :
            1) On rappelle récurisvement la fonction sur le noeud enfant et ses suivants
            2) Si le nom du noeud match, on l'ajoute avec addToken(_Token ** list, _Token * element);
            3) On passe au noeud parent suivant, tant qu'il n'est pas NULL 
*/



// Exemple start = NULL ; name = token => retourner l'adresse des noeuds start-line dans les sous-noeuds
_Token *searchTree(void *start,char *name)
{
    _Token *to_return = NULL;
    _Token *token;
    tree_node *node;

    if (start == NULL) node = getRootTree();
    else node = start;

    if (node->children != NULL)
    {
        token = searchTree(node->children,name);
        if (token != NULL) addToken(&to_return, token);
    }

    if (node->next_node != NULL)
    {
        token = searchTree(node->next_node,name);
        if (token != NULL) addToken(&to_return, token);
    }

    if(strcmp(node->name,name)==0)
    {
        token = malloc(sizeof(_Token));
        token->node = node;
        token->next = NULL;
        addToken(&to_return,token);
    }

    return to_return;
}




// fonction qui renvoie un pointeur vers char indiquant l'etiquette du noeud. (le nom de la rulename, intermediaire ou terminal) 
// et indique (si len!=NULL) dans *len la longueur de cette chaine.
char *getElementTag(void *node,int *len)
{
    tree_node *real_node = (tree_node *) node;
    
    if (len != NULL) *len = strlen(real_node->name);
    return real_node->name;
}




// fonction qui renvoie un pointeur vers char indiquant la valeur du noeud. (la partie correspondnant à la rulename dans la requete HTTP ) 
// et indique (si len!=NULL) dans *len la longueur de cette chaine.
char *getElementValue(void *node,int *len)
{
    tree_node *real_node = (tree_node *) node;
    
    if (len != NULL) *len = strlen(real_node->content);
    return real_node->content;
}






/*

    while (pointeur_children != NULL && pointeur_children->is_correct != 0)
	{
		print(pointeur_children);
		pointeur_children = pointeur_children->children;
	}
	while (pointeur_next_node != NULL && pointeur_next_node->is_correct != 0)
	{
		pointeur_children = pointeur_next_node;
		while (pointeur_children != NULL && pointeur_children->is_correct != 0)
		{
			print(pointeur_children);
			pointeur_children = pointeur_children->children;
		}
		pointeur_next_node = pointeur_next_node->next_node;
	}

*/



// Fonction qui supprime et libere la liste chainée de reponse. 
void purgeElement(_Token **r)
/*
_Token * t1, *t2;
	if(*r != NULL) {
		t1 = *r;
		while(t1->next != NULL) {
			t2 = t1->next;
			t1->next = t2->next;
			free(t2);
		}
	}
*/
{
    _Token *current;
    _Token *next_token;

    if (*r != NULL)
    {
        current = *r;
        while (current->next != NULL)
        {
            next_token = current->next;
            current->next = next_token->next;
            free(next_token);
        }
    }
}

// Fonction qui supprime et libere toute la mémoire associée à l'arbre . 
void purgeTree(void *root)
{
    delete_all_nodes(&root);
}

// L'appel à votre parser un char* et une longueur à parser.  
int parseur(char *req, int len)
{
    /*
    char requete[len + 1];
    requete[len] = '\0';

    int index;
    for (index = 0; index < len; index++) requete[index] = req[index];

    return is_http_message(requete,global_tree);
    */
    int to_return = is_http_message_preparsage(req,global_tree);

    //printf("\n Message http parsé sans erreur de segmentation mais ATTENTION le retour de fonction a été effectué donc (global_tree == NULL) = %d. \n\n",global_tree==NULL);

    /*
    if (global_tree != NULL)
    {
        if (global_tree->content != NULL) printf("\n Nom : %s \n Content : %s \n Is_correct : %d \n\n",global_tree->name,global_tree->content,global_tree->is_correct);
	    else printf("\n Nom : %s \n Content : Empty \n Is_correct : %d \n\n",global_tree->name,global_tree->is_correct);

        print(global_tree);
    }
    */

    return to_return;
}

