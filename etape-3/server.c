#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

// for librequest
#include "request.h"

// for parser

// this will declare internal type used by the parser
#include "api.h"

#include "utils.h"
#include "semantic.h"
#include "resource.h"
#include "response.h"


/*
* Fonction permettant de faciliter l'usage de l'api
* Paramètres : l'arbre généré par une requête, un nom de champ à chercher dans cet arbre, et une string à remplir
* Remplit la chaine s avec la valeur et la longueur du champ "name" trouvé dans l'arbre root. 
* Si root vaut NULL ou que le champ name n'est pas trouvé, s n'est pas modifiée.
* Retourne le pointeur vers token, pour libérer la mémoire affectée à ce dernier.
*/
_Token *getElement(_Token *root, char *name, string *s) {
	_Token *t = NULL;
	if(root != NULL) {
		t = searchTree(root, name);
		int size;
		char *str = (t != NULL)?getElementValue(t->node, &size):NULL;
		if(s != NULL) {
			s->base = str;
			s->length = size;
		}
	}

	return t;
}

int main(int argc, char *argv[])
{
	message *requete;
	while ( 1 ) {
		int connection_state = CLOSE_CONNECTION;

		// on attend la reception d'une requete HTTP requete pointera vers une ressource allouée par librequest.
		if ((requete=getRequest(8080)) == NULL ) return -1;

		_Token *message_token = NULL, *root = NULL, *t1 = NULL, *t2 = NULL, *t3 = NULL, *t4 = NULL, *t5 = NULL, *t6 = NULL, *t7 = NULL;
		int code, headers_uniques, version_ok, path_len;
		string *type_mime = NULL, path = { NULL, 0 }, method = { NULL, 0 }, body = { NULL, 0 }, content_length = { NULL, 0 }, http_version = { NULL, 0 }, connection_option = { NULL, 0 }, request_target = { NULL, 0 }, host = { NULL, 0 };

		if (parseur(requete->buf,requete->len)) { //Si le parseur génère bien un arbre
			root=getRootTree();
			message_token=searchTree(root,"HTTP_message");

			if (message_token != NULL) { //Si il y a bien un message
				
				t1 = getElement(root, "method", &method);

				//Unicité des headers
				headers_uniques = are_unique_headers(root);

				if(headers_uniques) {
					code = 200;
				} else {
					code = 400;
				}

				//Conformité de la méthode
				if(code == 200) {
					t2 = getElement(root, "message_body", &body);
					t3 = getElement(root, "Content_Length", &content_length);

					code = methodCompliance(&method, &body, &content_length);
				}

				//Vérification de la version
				if(code == 200) {
					t4 = getElement(root, "HTTP_version", &http_version);
					t5 = getElement(root, "Host", &host);

					code = is_http_version_ok(&http_version, &host);
				}

				// Disponibilité de la ressource
				if(code == 200) {
					t6 = getElement(root, "request_target", &request_target);
					char *p = isAvailable(&request_target, &host, &path_len);
					path.base = p;
					path.length = path_len;

					if (p == NULL) code = 404;
					else type_mime = get_mime_type(&path); //Type mime
				}
			} else {
				code = 400;
			}
		} else {
			code = 400;
		}

		t7 = getElement(root, "connection_option", &connection_option);
		connection_state = get_connection_state(&http_version, &connection_option); //Gestion de la connection
		send_response(&method, code, &path, type_mime, connection_state, requete->clientId); //Envoi de la réponse

		if(message_token != NULL) purgeElement(&message_token);
		if(t1 != NULL) purgeElement(&t1);
		if(t2 != NULL) purgeElement(&t2);
		if(t3 != NULL) purgeElement(&t3);
		if(t4 != NULL) purgeElement(&t4);
		if(t5 != NULL) purgeElement(&t5);
		if(t6 != NULL) purgeElement(&t6);
		if(t7 != NULL) purgeElement(&t7);
		if(root != NULL) purgeTree(root);

		// La condition provoque des fuites de mémoire lorsque la connexion est persistante se fait timeout
		// Sans le if, il n'y a normalement aucune fuite de mémoire
		if ((connection_state & CLOSE_CONNECTION) == CLOSE_CONNECTION) {
			requestShutdownSocket(requete->clientId);
        	freeRequest(requete);
        }
	}
	return (1);
}
