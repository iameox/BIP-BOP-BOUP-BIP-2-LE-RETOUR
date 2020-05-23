

#include <stdio.h>
#include <stdlib.h>
#include <magic.h>

#include "api.h"
#include "utils.h"
#include "fonctions_marin.h"


/*
	Fonction qui renvoie 0 si deux headers de même type sont présents, 1 sinon
*/

int are_unique_headers(void *root)
{
	int to_return = true;
	int size = 0;

	int index_header;
	char *headers_list[] = HEADERS;
	char *header_possible;
	_Token *head, *current_header;

	for (index_header = 0; (to_return == true && index_header < NB_HEADERS); index_header++)
	{
		header_possible = headers_list[index_header];
		head = searchTree(root, header_possible);
		current_header = head;
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
			to_return = false;
		}
		size = 0;

		purgeElement(&head);
	}

	return to_return;
}


/*
	Fonction qui renvoie 1 si la start-line se termine par HTTP/1.0  OU
	si la start-line se termine par HTTP/1.1 ET un unique host-header est présent ; 0 sinon
*/

/*
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
*/



// Les deux strings existent forcément, mais c'est leur champ ->base qui peut être null
int is_http_version_ok(string *http_version, string *host_header)
{
	int to_return = true;
	int is_found_host_header = false;

	//  && host_header->next == NULL mais un type string n'a pas de ->next 	
	if (host_header->base != NULL) is_found_host_header = true;

	// http_version->base existe forcément sinon le parsing crash
	if (!(compare_strings(http_version, "HTTP/1.0")) && !(compare_strings(http_version, "HTTP/1.1"))) to_return = 505;

	else if (compare_strings(http_version, "HTTP/1.1") && !(is_found_host_header)) to_return = 400;

	else to_return = 200;

	return to_return;
}



string* get_mime_type_with_magic(char *filename)
{
	// Crée un cookie qui "contient" l'option MAGIC_MIME_TYPE
	// Evite d'inclure le flag correspondant à chaque utilisation du cookie, entre autre
	magic_t cookie = magic_open(MAGIC_MIME_TYPE);
	string *result = malloc(sizeof(string));

	if (cookie == NULL)
	{
		printf("\n\n Echec lors de la création du magic cookie. \n\n");
		magic_close(cookie);
		// Retourner null ça permet de continuer l'exécution du programme, c'est pas très intéressant un serveur qui crash à cause d'une erreur qui est pas essentielle à son fonctionnement
		return NULL;
	}
	
	int is_load = magic_load(cookie, "/usr/share/misc/magic.mgc");
	if (is_load == -1)
	{
		printf("\n\n Erreur lors du chargement du fichier /usr/share/misc/magic.mgc : %s. \n\n", magic_error(cookie));
		// Retourner null ça permet de continuer l'exécution du programme, c'est pas très intéressant un serveur qui crash à cause d'une erreur qui est pas essentielle à son fonctionnement
		return NULL;
	}

	const char *mime_type;
	mime_type = magic_file(cookie, filename);

	if (mime_type == NULL)
	{
		printf("\n\n Erreur lors de la récupération des informations sur le fichier donné avec magic_file : %s. \n\n", magic_error(cookie));
		magic_close(cookie);
		// Retourner null ça permet de continuer l'exécution du programme, c'est pas très intéressant un serveur qui crash à cause d'une erreur qui est pas essentielle à son fonctionnement
		return NULL;
	}

	copy_to_string(mime_type, result);
	// Efface toutes les données en mémoire associées au cookie. Oui, TOUTES.
	magic_close(cookie);

	return result;
}


string* get_mime_type(string *filename)
{
	string *to_return;
	int extension_length = 0;
	while (filename->length - extension_length > 0 && filename->base[filename->length -1 - extension_length] != '.') {
		//printf("\n %d %c \n", index, filename->base[filename->length -2 - index]);
		extension_length++;
	}
	extension_length--;
	//printf("\n Extension : %s.\n Taille de l'extension : %d \n\n", filename->base + filename->length-1 - index, index);

	if (extension_length == 0) to_return = get_mime_type_with_magic(filename->base);
	else {
		//printf("\n\n ENTREE DANS LE ELSE \n\n");
		//printf("\n\n %s. %d, %d \t %d %c \n\n", filename->base, filename->length, index, filename->length - index, *(filename->base + filename->length - index));
		string extension = { filename->base + filename->length - 1 - extension_length, extension_length };
		char *extensions[] = EXTENSIONS;
		char *types_mime[] = EXTENSIONS_TYPES_MIME;
		
		int index_extension = 0;
		int is_found = false;

		//printf("\n\n TAILLE DE L EXTENSION == 2 \n\n");
		while (!is_found && index_extension < NB_EXTENSIONS) {
			is_found = compare_strings(&extension, extensions[index_extension]);
			if (!is_found) index_extension++;
		}

		if (is_found) {
			to_return = malloc(sizeof(string));
			copy_to_string(types_mime[index_extension], to_return);

		} else to_return = get_mime_type_with_magic(filename->base);
	}

	return to_return;
}