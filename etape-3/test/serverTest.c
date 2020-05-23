#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// for librequest
#include "request.h"

// for parser

// this will declare internal type used by the parser
#include "api.h"

#include "utils.h"
#include "fonctions_lucas.h"
#include "fonctions_marin.h"
#include "normalization.h"
#include "response.h"

_Token *getElement(_Token *root, char *name, string *s) {
	_Token *t = searchTree(root, name);
	int size;
	char *str = (t != NULL)?getElementValue(t->node, &size):NULL;
	if(s != NULL) {
		s->base = str;
		s->length = size;
	}

	return t;
}

int main(int argc, char *argv[])
{
	message *requete;
	int res;
	while ( 1 ) {
		// on attend la reception d'une requete HTTP requete pointera vers une ressource allouée par librequest.
		if ((requete=getRequest(8080)) == NULL ) return -1;

		// Affichage de debug
		printf("#########################################\nDemande recue depuis le client %d\n",requete->clientId);
		printf("Client [%d] [%s:%d]\n",requete->clientId,inet_ntoa(requete->clientAddress->sin_addr),htons(requete->clientAddress->sin_port));
		printf("Contenu de la demande %.*s\n\n",requete->len,requete->buf);
		if (res=parseur(requete->buf,requete->len)) {
			_Token *tok, *r, *root, *t1 = NULL, *t2 = NULL, *t3 = NULL, *t4 = NULL, *t5 = NULL, *t6 = NULL;

			//writeDirectClient(requete->clientId,REPONSE,strlen(REPONSE));
			root=getRootTree();
			r=searchTree(root,"HTTP_message");
			tok = r;
			while (tok) {

				//========================= ZONE DE TEST =============================
				printf("\n\n===================================== DÉBUT DE LA ZONE DE TEST =====================================\n\n");
				int code, headers_uniques, version_ok, path_len;
				string *type_mime = NULL, path = { NULL, 0 }, method, body, content_length, http_version, request_target, host;
				t1 = getElement(root, "method", &method);

				//Unicité des headers
				headers_uniques = are_unique_headers(root);
				printf("Unicité : %d\n", headers_uniques);

				if(headers_uniques) {
					code = 200;
					printf("Les headers sont uniques, on peut continuer\n");
				} else {
					code = 400;
					printf("Un header est présent en double.\n");
					printf("RENVOYER 400 Bad Request\n");
				}

				printf("Code à répondre = %d (200 = pas de pb)\n", code);

				//Conformité de la méthode
				if(code == 200) {
					t2 = getElement(root, "message_body", &body);
					t3 = getElement(root, "Content_Length", &content_length);

					code = methodCompliance(&method, &body, &content_length);

					printf("Code à répondre = %d (200 = pas de pb)\n", code);
				}

				//Vérification de la version
				if(code == 200) {
					t4 = getElement(root, "HTTP_version", &http_version);
					t5 = getElement(root, "Host", &host);

					version_ok = is_http_version_ok(&http_version, &host);

					if(version_ok) {
						code = 200;
						printf("La version est OK\n");
					} else {
						code = 400;
						printf("Problème de version\n");
						printf("RENVOYER 400 Bad Request\n");
					}
				}

				// Disponibilité de la ressource
				if(code == 200) {
					t6 = getElement(root, "request_target", &request_target);

					//if(host.base != NULL) printf("host = \"%s\" l=%d\n", host.base, host.length);
					//else printf("Pas de host\n");
					char *p = isAvailable(&request_target, &host, &path_len);
					path.base = p;
					path.length = path_len;

					if (p == NULL) code = 404;
					else type_mime = get_mime_type(&path);
				}

				send_response(&method, code, &path, type_mime, requete);

				printf("\n\n===================================== FIN DE LA ZONE DE TEST =====================================\n\n");
				//=====================================================================

				//int l;
				//char *s = getElementValue(tok->node, &l);
				//if (s != NULL) writeDirectClient(requete->clientId,s,l);

				if(t1 != NULL) purgeElement(&t1);
				if(t2 != NULL) purgeElement(&t2);
				if(t3 != NULL) purgeElement(&t3);
				if(t4 != NULL) purgeElement(&t4);
				if(t5 != NULL) purgeElement(&t5);
				if(t6 != NULL) purgeElement(&t6);
				tok=tok->next;
			}
			purgeElement(&r);
		purgeTree(root);
		} else {
			//writeDirectClient(requete->clientId,ERROR,strlen(ERROR));
		}
		endWriteDirectClient(requete->clientId);
		requestShutdownSocket(requete->clientId);
	// on ne se sert plus de requete a partir de maintenant, on peut donc liberer...
	freeRequest(requete);
	}
	return (1);
}
