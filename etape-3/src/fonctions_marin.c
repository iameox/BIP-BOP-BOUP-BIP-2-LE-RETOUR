
#include "fonctions_marin.h"

#include <stdio.h> 
#include <stdlib.h> 

#include "api.h"
#include "utils.h"

/*
	Fonction qui renvoie 0 si deux headers de même type sont présents, 1 sinon
*/

int are_unique_headers(void *root)
{
	int to_return = 1;
	int size = 0;

	int index_header;
	char *headers_list[] = {"Connection_header", "Content_Length_header", "Content_Type_header", "Cookie_header", "Transfer_Encoding_header", "Expect_header", "Host_header", "Accept_header", "Accept_Charset_header", "Accept_Encoding_header", "Accept_Language_header", "Referer_header", "User_Agent_header"};
	char *header_possible;
	_Token *current_header;

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

		purgeElement(&current_header);
	}

	return to_return;
}


/*
	Fonction qui renvoie 1 si la start-line se termine par HTTP/1.0  OU
	si la start-line se termine par HTTP/1.1 ET un unique host-header est présent ; 0 sinon
*/

/*
		IMPORTANT 

		Utiliser HTTP_version plutôt que start_line
*/
int is_http_version_ok(void *root)
{
	int to_return = 0;
	int is_found_host_header = 0;
	int length;

	_Token *http_version;
	_Token *host_header;

	host_header = searchTree(root, "Host_header");
	// host_header = searchTree(root, "Host__________________header");
	if (host_header != NULL && host_header->next == NULL) is_found_host_header = 1;
	printf("\n\n Is found host header : %d \n\n", is_found_host_header);


	http_version = searchTree(root, "HTTP_version");
	// http_version = searchTree(root, "HTTP__________________version");
	printf("\n\n HTTP version found : %d \n\n", http_version == NULL);

	if (http_version != NULL)
	{
		// Récupérer ici ce que contient la http_version pour trouver la version HTTP
		char *http_version_string = getElementValue(http_version->node, &length);
		
		if (http_version_string != NULL)
		{
			printf("\n\n Ligne récupérée : %s. de longueur %d \n\n", http_version_string, length);

			string my_string = {http_version_string, length};
			// Comparer cette version HTTP à ce qui est attendu
			to_return = (compare_strings(&my_string, "HTTP/1.0") || (compare_strings(&my_string, "HTTP/1.1") && is_found_host_header) );
		}
		else printf("\n\n LIGNE NULLE \n\n");
	}
	else printf("\n\n HTTP VERSION NULLE \n\n");

	purgeElement(&host_header);
	purgeElement(&http_version);
	
	return to_return;
}