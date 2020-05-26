#include <stdio.h>
#include <stdlib.h>

#include "api.h"
#include "utils.h"
#include "semantic.h"


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
	renvoie 400 si la version est HTTP/1.1 mais que le host header n'est pas présent ;
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
* Vérifie qu'une méthode donnée (passée sous forme de string) fait bien parties de celles qui sont implémentées par le serveur.
* La liste des méthodes implémentée est définie dans semantic.h
* Retourne true si la méthode est implémentée, false sinon.
*/
int validMethod(string *method) {
	int valid = false, i = 0;
	char *methods[] = KNOWN_METHODS;
	while(!valid && i < METHODS_NUMBER) {
		if(compare_strings(method, methods[i])) valid = true;
		i++;
	}
	return valid;
}



/*
* Retourne le code de réponse HTTP vérifiant la sémantique de la méthode.
* Paramètres : les champs method et body et le header Content-Length de la requête à analyser
* Si l'un de ces champs n'est pas présent, il faut envoyer une string dont la base est égale à NULL et de taille 0.
* Retourne 200 si la méthode est valide, le code d'erreur à renvoyer sinon.
*/
int methodCompliance(string *method, string *body, string *content_length) {
	int content_length_int, code;

	if(method->base == NULL) { //Champ method non présent, ne devrait pas arriver car la librequest rejette les requêtes sans méthode
		code = 400;
	} else if (!validMethod(method)) { //Méthode inconnue
		code = 501;
	} else if(body->base != NULL && body->length != 0) { // Vérification de la présence et conformité du body
		if(content_length->base != NULL) {
			content_length_int = string_to_int(content_length);
			if (body->length == content_length_int) { //Content-Length valide
				code = 200;
			} else { //Content-Length différent de la taille du body
				code = 400;
			}
		} else {
			//Comme nous n'avons pas traité Transfer-Encoding (qui est optionnel), nous rejetons les requêtes présentant un body mais pas de Content-Length en utilisant 411 Length Required.
			code = 411;
		}
	} else { //Body non présent
		code = 200;
	}

	return code;
}