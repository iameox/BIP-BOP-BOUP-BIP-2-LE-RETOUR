
#include "test.h"

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 

#include "api.h"

/*
	typedef struct _token
	{
		void *node; 
		struct _token *next;
	} _Token;

*/

// Fonction qui recherche dans l'arbre tous les noeuds dont l'etiquette est egale à la chaine de caractères en argument.   
// Par convention si start == NULL alors on commence à la racine 
// sinon on effectue une recherche dans le sous-arbre à partir du noeud start 
// _Token *searchTree(void *start,char *name); 

int are_unique_headers(void *root)
{
	int to_return = 1;
	int size = 0;

	int index_header;
	char *headers_list[] = {"Connection_header", "Content_Length_header", "Content_Type_header", "Cookie_header", "Transfer_Encoding_header", "Expect_header", "Host_header", "Accept_header", "Accept_Charset_header", "Accept_Encoding_header", "Accept_Language_header", "Referer_header", "User_Agent_header"};
	char *header_possible;
	_Token *current_header;
	char *header_tmp;

	for (index_header = 0; (to_return == 1 && index_header < 13); index_header++)
	{
		header_possible = headers_list[index_header];
		current_header = searchTree(root, header_possible);
		// current_header = searchTree(root, "Host__________________header");
		
		while(current_header != NULL)
		{
			size++;
			current_header = current_header->next;
		}
		// size peut valoir 0 s'il n'y a pas le header dans le message http !!!
		if (size != 0 && size != 1)
		{
			printf("\n\n Size invalide pour le header %s : %d headers trouvés. \n\n", header_possible, size);
			to_return = 0;
		}
		size = 0;
	}

	return to_return;
}

/*
void afficherSearchTree(void * root)
{
	int index;
	_Token *header;
	header = searchTree(root, "header_field");
	while (header != NULL)
	{
		printf("\n\n Search Tree retourné : \n\n");
		for (index = 0; index < header->node->len; index++)
		{
			printf("%c", header->node->value[j]);
		}
		printf("\n\n");
		header = header->next;
	}
}
*/