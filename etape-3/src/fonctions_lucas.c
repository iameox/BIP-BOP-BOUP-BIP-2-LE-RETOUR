#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "fonctions_lucas.h"
#include "normalization.h"


/*
* Vérifie qu'une méthode fait bien parties de celles implémentées par le serveur.
* Retourne 1 si la méthode est connue, 0 sinon.
*/
int validMethod(string *method) {
	int valid = false;
	char *methodes[] = KNOWN_METHODS;
	int i = 0;
	while(!valid && i < METHODS_NUMBER) {
		if(compare_strings(method, methodes[i])) {
			printf("lLa méthode c'est un %s\n", methodes[i]);
			valid = true;
		}
		i++;
	}

	return valid;
}

/*
* Indique si une ressource décrite par le couple (request_target, host) est disponible.
* On considère que la request_target est de type origin-form
* Si host vaut NULL, on considère que la requête est du HTTP 1.0
* Retourne un char* pointant vers le path de la requête et écrit dans len si la ressource existe, renvoie NULL sinon
*/

// ATTENTION il faut free le char * retourné après utilisations (si non NULL)
char *isAvailable(string *request_target, string *host, int *len) {
	int i = 0, j, size;

	char *hosts_lists[] = KNOWN_HOSTS_LIST;
	char *hosts_paths[] = KNOWN_HOSTS_PATHS;
	char *website_path = NULL;

	char *ressource_path = NULL;

	//Normalize according to ABNF
	normalize_request_target(request_target);

	while(i < KNOWN_HOSTS_COUNT && website_path == NULL && *host != NULL) {
		if(compare_strings(host, hosts_lists[i])) website_path = hosts_paths[i];
		i++;
	}

	if(website_path != NULL) {
		size = strlen(ROOT_PATH) + strlen(website_path) + request_target->length + 1;
		ressource_path = malloc(size*sizeof(char));

		//On recopie le début de l'arborescence (propre au serveur)
		for(i = 0 ; i < strlen(ROOT_PATH) ; i++) ressource_path[i] = ROOT_PATH[i];
		//host
		j = 0;
		while(j < strlen(website_path)) {
			ressource_path[i] = website_path[j];
			i++;
			j++;
		}
		//request-target
		j = 0;
		while(j < request_target->length) {
			ressource_path[i] = request_target->base[j];
			i++;
			j++;
		}

		ressource_path[i] = '\0';

		//printf("on cherche la ressource avec le path = \"");
		//for(i = 0 ; i < strlen(ROOT_PATH) + strlen(website_path) + request_target->length ; i++) printf("%c", ressource_path[i]);
		//printf("\"\n");

		FILE * ressource = fopen(ressource_path, "r");

		if(ressource != NULL) {
			if(fclose(ressource) != 0) printf("Erreur lors de la fermeture de la ressource.\n");
			if(len != NULL) *len = size;
		} else {
			printf("Ressource pas accessible\n");
			free(ressource_path);
			ressource_path = NULL;
		}

	} else if(*host == NULL) { //HTTP 1.0
			size = strlen(ROOT_PATH) + strlen(website_path) + request_target->length + 1;
			ressource_path = malloc(size*sizeof(char));

			//On recopie le début de l'arborescence (propre au serveur)
			for(i = 0 ; i < strlen(ROOT_PATH) ; i++) ressource_path[i] = ROOT_PATH[i];

			//request-target
			j = 0;
			while(j < request_target->length) {
				ressource_path[i] = request_target->base[j];
				i++;
				j++;
			}

			ressource_path[i] = '\0';

			FILE * ressource = fopen(ressource_path, "r");

			if(ressource != NULL) {
				if(fclose(ressource) != 0) printf("Erreur lors de la fermeture de la ressource.\n");
				if(len != NULL) *len = size;
			} else {
				printf("Ressource pas accessible\n");
				free(ressource_path);
				ressource_path = NULL;
			}
	} else {
			printf("Site pas trouvé\n");
	}


	return ressource_path;
}


int methodCompliance(string *method, string *body, string *content_length) {
	int content_length_int, code;

	if(method->base == NULL) {
		printf("Champ méthode non présent.\n");
		printf("RENVOYER 400 Bad Request\n");
		code = 400;
	} else if (!validMethod(method)) {
		printf("Méthode inconnue.\n");
		printf("RENVOYER 501 Not implemented\n");
		code = 501;
	} else if(body->length == 0 || body->base != NULL) { // Vérification de la présence et conformité du body
		printf("Body de longueur %d présent, vérification de la conformité avec Content-Length...\n", body->length);
		if(content_length->base != NULL) {
			//printf("Le Content-Length = ");
			//for(int i = 0 ; i < laine2 ; i++) printf("%c", chene2[i]);
			//printf("\n");

			content_length_int = string_to_int(content_length);
			//printf("le Content-Length après passage en int : %d\n", content_length_int);

			if (body->length == content_length_int) {
				printf("Le Content-Length est égal à la taille du body, la méthode est valide.\n");
				code = 0;
			} else {
				printf("Le Content-Length n'est pas égal à la taille du body.\n");
				printf("RENVOYER 400 Bad Request\n");
				code = 400;
			}
		} else {
			printf("Content-Length est pas présent alors qu'il y a un body.\n");
			printf("RENVOYER 400 Bad Request\n");
			code = 400;
		}
	} else {
		printf("Body non présent\n");
		if(compare_strings(method, "POST")) {
			printf("La requête est un POST sans body.\n");
			printf("RENVOYER 400 Bad Request\n");
			code = 400;
		} else {
			printf("Méthode valide.\n");
			code = 0;
		}
	}

	return code;
}
