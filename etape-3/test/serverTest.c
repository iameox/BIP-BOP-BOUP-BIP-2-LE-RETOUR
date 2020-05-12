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
				_Token *test, *test2, *body_token, *request_target_token;
				int laine, laine2, body_len, request_target_len;
				char *chene, *chene2, *body_str, *request_target;

				// Vérification de la présence du body
				/*body_token = searchTree(root,"message_body");
				body_str = (body_token != NULL)?getElementValue(body_token->node, &body_len):NULL;

				if(body_str == NULL) {
					printf("y'a pas de body, faut checker la méthode et envoyer la réponse en fonction\n");
				}*/

				//Vérification de la conformité de la méthode
				/*int content_length = -1;
				test = searchTree(root,"method");
				chene = (test != NULL)?getElementValue(test->node, &laine):NULL;

				string method = {chene, laine};
				if (chene != NULL) printf("Est ce que la méthode est valide ? %d\n", validMethod(&method));
				else printf("Méthode inconnue, faut répondre avec le code associé\n");

				if(compare_strings(&method, "POST")) {
					printf("Il faut vérifier Content-Length et sa conformité\n");
					test2 = searchTree(root,"Content_Length");
					chene2 = (test2 != NULL)?getElementValue(test2->node, &laine2):NULL;
					if(chene2 != NULL) {
						printf("Le Content-Length = ");
						for(int i = 0 ; i < laine2 ; i++) printf("%c", chene2[i]);
						printf("\n");
						string content_length_string = {chene2, laine2};

						content_length = string_to_int(content_length_string);

						printf("le content_length après passage en int : %d\n", content_length);

						if(body_str != NULL) {
							printf("Longueur du body = %d\n", body_len);
							if (body_len == content_length) {
								printf("Le Content-Length est égal à la taille du body, la méthode est conforme.\n");
							} else printf("Le Content-Length n'est pas égal à la taille du body, faut répondre en conséquence\n");
						}
					} else printf("Content-Length est pas présent, il faut rejeter la requete\n");
				}*/

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
