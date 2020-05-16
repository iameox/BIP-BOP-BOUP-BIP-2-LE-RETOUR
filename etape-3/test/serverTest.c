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

#include "../src/fonctions_lucas.h"
#include "../src/normalization.h"

#define ERROR "HTTP/1.0 400 SUCKA\r\n\r\n"
#define REPONSE "HTTP/1.0 200 OK\r\nContent-type: text/plain\r\n\r\nHey Bro why did you send me this:\r\n"


void getElement(_Token *root, char * name, string *s) {
	_Token t = searchTree(root,"method");
	int size;
	char * str = (t != NULL)?getElementValue(t->node, &size):NULL;
	if(s != NULL) {
		s->base = str;
		s->size = size;
	}
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
			_Token *r,*tok,*root;

			writeDirectClient(requete->clientId,REPONSE,strlen(REPONSE));
			root=getRootTree();
			r=searchTree(root,"HTTP_message");
			tok=r;
			while (tok) {

				//========================= ZONE DE TEST =============================
				printf("\n\n===================================== DÉBUT DE LA ZONE DE TEST =====================================\n\n");
				_Token *test, *test2, *method_token, *body_token, *request_target_token;
				int laine, laine2, method_len, body_len, request_target_len;
				char *chene, *chene2, *method_str, *body_str, *request_target;


				//UNICITE DES HEADERS A VERIFIER


				int code;
				string method, body, content_length_str;

				getElement(root, "method", &method);
				getElement(root, "message_body", &body);
				getElement(root, "Content_Length", &content_length_str);

				code = methodCompliance(string *method, string *body, string *content_length);

				printf("Code à répondre = %s (0 = on continue)\n", code);

				/*if(code == 0) {
					//on fé d'autres trucs
				} else {
					//on répond direct
				}*/

				// VERIFICATION DE LA VERSION

				// Disponibilité de la ressource
				/*test = searchTree(root,"request_target");
				test2 = searchTree(root,"Host");
				chene = (test != NULL)?getElementValue(test->node, &laine):NULL;
				chene2 = (test2 != NULL)?getElementValue(test2->node, &laine2):NULL;


				if (chene != NULL && chene2 != NULL) {
					string target = {chene, laine};
					string host = {chene2, laine2};

					printf("Est ce que la ressource est disponible? %d\n", isAvailable(&target, &host));
				}*/

				// Normalisation d'URI
				/*request_target_token = searchTree(root, "request_target");
				request_target = request_target_token != NULL ? getElementValue(request_target_token->node, &request_target_len) : NULL;
				if (request_target != NULL) {
					string s = { request_target, request_target_len };

    				normalize_request_target(&s);
    				printf("URI après normalisation : %s\nLongueur après normalisation : %d\n", request_target, request_target_len );
				}*/

				printf("\n\n===================================== FIN DE LA ZONE DE TEST =====================================\n\n");
				//=====================================================================

				int l;
				char *s = getElementValue(tok->node, &l);
				if (s != NULL) writeDirectClient(requete->clientId,s,l);

				tok=tok->next;
			}
			purgeElement(&r);
		purgeTree(root);
		} else {
			writeDirectClient(requete->clientId,ERROR,strlen(ERROR));
		}
		endWriteDirectClient(requete->clientId);
		requestShutdownSocket(requete->clientId);
	// on ne se sert plus de requete a partir de maintenant, on peut donc liberer...
	freeRequest(requete);
	}
	return (1);
}
