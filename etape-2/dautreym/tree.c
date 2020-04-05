#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "api.h"
#include "utils.h"
#include "compare.h"
#include "parseur.h"
#include "tree.h"


/*
    Exemple d'utilisation :

    tree_node *children;
    children = malloc(sizeof(tree_node));
    children->name = malloc(50*sizeof(char));
    children->name = "HTTP_message\0";
    children->adress = &is_http_message;
    children->content = malloc(500*sizeof(char));
    children->is_correct = 0;
    children->next_node = malloc(sizeof(tree_node));
    children->children = malloc(sizeof(tree_node));
    
    tree_node *children_next;
    children_next = malloc(sizeof(tree_node));
    children_next->name = malloc(50*sizeof(char));
    children_next->name = "HTTP_message\0";
    children_next->adress = &is_http_message;
    children_next->content = malloc(500*sizeof(char));
    children_next->is_correct = 0;
    children_next->next_node = malloc(sizeof(tree_node));
    children_next->children = malloc(sizeof(tree_node));

    tree->name = "absolute_path\0";
    tree->adress = &is_absolute_path;
    tree->children = children;
    tree->children->next_node = children_next;

    tree->content = NULL;
    purgeNode(tree);
*/


void addToken(_Token **token_list, _Token *to_add)
{
    if(*token_list == NULL) *token_list = to_add;
    else
    {
        to_add->next = *token_list;
        *token_list = to_add;
    }
}


/*
    Cette fonction prend en argument un noeud à partir duquel checher et une liste à laquelle ajouter
        Pour chaque noeud parent :
            1) On rappelle récurisvement la fonction sur le noeud enfant et ses suivants
            2) Si le nom du noeud match, on l'ajoute avec addToken(_Token ** list, _Token * element);
            3) On passe au noeud parent suivant, tant qu'il n'est pas NULL 
*/



void print(tree_node *mytree)
{
    if (mytree->children != NULL) print(mytree->children);
    if (mytree->next_node != NULL) print(mytree->next_node);

	if (mytree->content != NULL && mytree->is_correct != 0) printf("\n Nom : %s \n Content : %s \n Is_correct : %d \n\n",mytree->name,mytree->content,mytree->is_correct);
	//else printf("\n Nom : %s \n Content : Empty \n Is_correct : %d \n\n",mytree->name,mytree->is_correct);
}



void purgeNode(tree_node *node)
{
    node->is_correct = 0;
    //freeNode(node->children);
}




void delete_all_nodes(tree_node **tree)
{
	tree_node *node = *tree;
	if((*tree)->children != NULL) delete_all_nodes(&(*tree)->children);
	if((*tree)->next_node != NULL) delete_all_nodes(&(*tree)->next_node);

	(*tree) = NULL;
	free(node);
}



