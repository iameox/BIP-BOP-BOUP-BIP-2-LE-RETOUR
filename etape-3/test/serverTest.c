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
			_Token *tok, *r, *root, *t1, *t2, *t3, *t4, *t5;

			//writeDirectClient(requete->clientId,REPONSE,strlen(REPONSE));
			root=getRootTree();
			r=searchTree(root,"HTTP_message");
			tok = r;
			while (tok) {
			
				//========================= ZONE DE TEST =============================
				printf("\n\n===================================== DÉBUT DE LA ZONE DE TEST =====================================\n\n");
				int code;

				//UNICITE DES HEADERS A VERIFIER


				
				string method, body, content_length;

				t1 = getElement(root, "method", &method);
				t2 = getElement(root, "message_body", &body);
				t3 = getElement(root, "Content_Length", &content_length);

				code = methodCompliance(&method, &body, &content_length);

				printf("Code à répondre = %d (0 = on continue)\n", code);

				// VERIFICATION DE LA VERSION

				//if(code == 0) {
					// Disponibilité de la ressource
					string request_target, host;
					t4 = getElement(root, "request_target", &request_target);
					t5 = getElement(root, "Host", &host);

					int path_len;
					printf("%s\n", host.base);
					char *path = isAvailable(&request_target, &host, &path_len);
					if(path == NULL) {
						printf("Ressource indisponible.\n");
					} else {
						printf("Est ce que la ressource est disponible? %s\n", path);
						
						string p = { path, path_len };
						send_response(&method, 200, &p, requete);
						free(path);
					}
				//}

				printf("\n\n===================================== FIN DE LA ZONE DE TEST =====================================\n\n");
				//=====================================================================

				//int l;
				//char *s = getElementValue(tok->node, &l);
				//if (s != NULL) writeDirectClient(requete->clientId,s,l);
				purgeElement(&t1);
				purgeElement(&t2);
				purgeElement(&t3);
				purgeElement(&t4);
				purgeElement(&t5);
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
