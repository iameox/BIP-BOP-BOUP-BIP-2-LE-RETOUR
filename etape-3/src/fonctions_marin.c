

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

	/*
		Pour chaque header, tant qu'un header n'est pas présent en double
	*/
	for (index_header = 0; (to_return == true && index_header < NB_HEADERS); index_header++)
	{
		/*
			On récupère tous les headers au tag correspondant depuis la racine
		*/
		header_possible = headers_list[index_header];
		head = searchTree(root, header_possible);
		current_header = head;
		
		/*
			Puis on regarde combien on en a récupéré
		*/
		while(current_header != NULL)
		{
			size++;
			current_header = current_header->next;
		}

		/*
			Enfin, on vérifie que le header n'est pas présent (size == 0)
			OU qu'il n'est présent qu'en un seul exemplaire (size == 1)
		*/
		if (size != 0 && size != 1)
		{
			printf("\n\n Size invalide pour le header %s : %d headers trouvés. \n\n", header_possible, size);
			to_return = false;
		}
		/*
			Puis on réinitialise size pour la taille du prochain header
		*/
		size = 0;

		/*
			On n'oublie pas de libérer la mémoire allouée à chaque fois
		*/
		purgeElement(&head);
	}

	return to_return;
}


/*
	Fonction qui renvoie 505 si la version HTTP est différente de 1.0 ou 1.1 ;
	renvoie 400 si la version est HTTP/1.1 masi que le host header n'est pas présent ;
	renvoie 200 sinon

	Prend en argument :
	- une string contenant la version HTTP
	- une string contenant le host header s'il est présent, NULL sinon
*/

int is_http_version_ok(string *http_version, string *host_header)
{
	int to_return = true;
	int is_found_host_header = false;

	if (host_header->base != NULL) is_found_host_header = true;

	if (!(compare_strings(http_version, "HTTP/1.0")) && !(compare_strings(http_version, "HTTP/1.1"))) to_return = 505;

	else if (compare_strings(http_version, "HTTP/1.1") && !(is_found_host_header)) to_return = 400;

	else to_return = 200;

	return to_return;
}



/*
	Fonction qui renvoie le type mime d'un fichier à l'aide de la librairie magic.h
	
	Prend en argument le nom du fichier dont on veut connaitre le type mime
*/
string* get_mime_type_with_magic(char *filename)
{
	/*
		Crée un cookie qui "contient" l'option MAGIC_MIME_TYPE
		Evite d'inclure le flag correspondant à chaque utilisation du cookie, entre autre
	*/
	magic_t cookie = magic_open(MAGIC_MIME_TYPE);
	string *result = malloc(sizeof(string));

	if (cookie == NULL)
	{
		printf("\n\n Echec lors de la création du magic cookie. \n\n");
		magic_close(cookie);
		/*
			Retourner NULL permet de continuer l'exécution du programme
			Un serveur qui crash à cause d'une erreur qui n'est pas essentielle à son fonctionnement n'est pas très intéressant
		*/
		return NULL;
	}

	/*
		Charge tous les types mime requis pour la bonne exécution de toutes les fonctions magic suivantes
	*/
	int is_load = magic_load(cookie, "/usr/share/misc/magic.mgc");
	if (is_load == -1)
	{
		printf("\n\n Erreur lors du chargement du fichier /usr/share/misc/magic.mgc : %s. \n\n", magic_error(cookie));
		/*
			Retourner NULL permet de continuer l'exécution du programme
			Un serveur qui crash à cause d'une erreur qui n'est pas essentielle à son fonctionnement n'est pas très intéressant
		*/
		return NULL;
	}

	/*
		magic_file permet de récupérer les informations sur le fichier de nom filename
		le cookie précise que l'information récupérée est directement le type mime du fichier, en cas de succès
	*/
	const char *mime_type;
	mime_type = magic_file(cookie, filename);

	if (mime_type == NULL)
	{
		printf("\n\n Erreur lors de la récupération des informations sur le fichier donné avec magic_file : %s. \n\n", magic_error(cookie));
		magic_close(cookie);
		/*
			Retourner NULL permet de continuer l'exécution du programme
			Un serveur qui crash à cause d'une erreur qui n'est pas essentielle à son fonctionnement n'est pas très intéressant
		*/
		return NULL;
	}

	copy_to_string(mime_type, result);
	/*
		Efface toutes les données en mémoire associées au cookie. Oui, TOUTES.
	*/
	magic_close(cookie);

	return result;
}



/*
	Fonction qui renvoie le type mime d'un fichier SANS l'aide de la librairie magic.h
	
	Prend en argument une string contenant le nom du fichier dont on veut connaitre le type mime
*/
string* get_mime_type(string *filename)
{
	string *to_return;
	int extension_length = 0;

	/*
		Tant que l'on n'a pas trouvé le dernier . (marquant le début de l'extension du fichier)
		OU que l'on n'est pas arrivé au tout début du nom du fichier
		On regarde le caractère précédent
	*/
	while (filename->length - extension_length > 0 && filename->base[filename->length -1 - extension_length] != '.')
	{
		extension_length++;
	}
	/*
		Instruction utile si on a trouvé un .
		Permet de se mettre au début de l'extension, donc juste après le . trouvé
	*/
	extension_length--;

	/*
		Si on est arrivé au tout début du fichier, on réessaye avec la librairie magic.h
	*/
	if (extension_length == 0) to_return = get_mime_type_with_magic(filename->base);
	/*
		Sinon on essaye de déterminer l'extension en la comparant avec les 66 extensions les plus connues
	*/
	else
	{
		string extension = { filename->base + filename->length - 1 - extension_length, extension_length };
		char *extensions[] = EXTENSIONS;
		char *types_mime[] = EXTENSIONS_TYPES_MIME;
		
		int index_extension = 0;
		int is_found = false;

		/*
			Tant que l'extension n'a pas été trouvée, on compare notre extension avec l'extension suivante
		*/
		while (!is_found && index_extension < NB_EXTENSIONS)
		{
			is_found = compare_strings(&extension, extensions[index_extension]);
			if (!is_found) index_extension++;
		}

		/*
			Si l'instruction a été trouvée, on la renvoie
		*/
		if (is_found)
		{
			to_return = malloc(sizeof(string));
			copy_to_string(types_mime[index_extension], to_return);

		}
		/*
			Sinon on réessaye avec la librairie magic.h
		*/
		else to_return = get_mime_type_with_magic(filename->base);
	}

	return to_return;
}