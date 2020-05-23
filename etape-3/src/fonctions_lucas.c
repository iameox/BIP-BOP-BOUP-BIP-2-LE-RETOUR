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
			printf("Méthode : %s\n", methodes[i]);
			valid = true;
		}
		i++;
	}

	return valid;
}


/*
* Indique si une ressource décrite par le couple (request_target, host) est disponible.
* host et request_target target ne doivent PAS valoir NULL
* On considère que la request_target est de type origin-form
* Si host->base vaut NULL, on considère que la requête est du HTTP 1.0 (on utilise que la request-target)
* Retourne un char* pointant vers le path de la requête et écrit dans len si la ressource existe, renvoie NULL sinon
*/

// ATTENTION il faut free le char * retourné après utilisations (si non NULL)
char *isAvailable(string *request_target, string *host, int *len) {
	int i = 0, j, size;

	char *hosts_lists[] = KNOWN_HOSTS_LIST;
	char *hosts_paths[] = KNOWN_HOSTS_PATHS;
	int hosts_sizes[] = KNOWN_HOSTS_SIZES, website_path_size;
	char *website_path = NULL;
	char *default_file = DEFAULT_FILE_PATH;
	char *ressource_path = NULL;

	int request_size;

	//Normalize according to ABNF
	normalize_request_target(request_target);

	request_size = request_target->length;
	printf("Requete après normalisation : \"%.*s\"\n", request_target->length, request_target->base);

	if(request_target->base[request_target->length - 1] == '/') {
		request_size += DEFAULT_FILE_LENGTH;
	}

	while(i < KNOWN_HOSTS_COUNT && website_path == NULL && host->base != NULL) {
		if(compare_strings(host, hosts_lists[i])) {
			website_path = hosts_paths[i];
			website_path_size = hosts_sizes[i];
		}
		i++;
	}

	if(website_path != NULL) {
		size = ROOT_PATH_SIZE + website_path_size + request_size + 1;
		ressource_path = malloc(size*sizeof(char));

		//On recopie le début de l'arborescence (propre au serveur)
		for(i = 0 ; i < ROOT_PATH_SIZE ; i++) ressource_path[i] = ROOT_PATH[i];
		//host
		j = 0;
		while(j < website_path_size) {
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

		//On rajoute un "index.html" si besoin
		if(request_size > request_target->length) { //si on doit rajouter index.html
			for(j = 0 ; j < DEFAULT_FILE_LENGTH ; j++) {
				ressource_path[i] = default_file[j];
				i++;
			}
		}

		ressource_path[i] = '\0';

		printf("Path de la ressource après concaténation : \"%.*s\"\n", size, ressource_path);

		FILE * ressource = fopen(ressource_path, "r");

		if(ressource != NULL) {
			if(fclose(ressource) != 0) printf("Erreur lors de la fermeture de la ressource.\n");
			if(len != NULL) *len = size;
		} else {
			printf("Ressource pas accessible\n");
			free(ressource_path);
			ressource_path = NULL;
		}

	} else if(host->base == NULL) { //HTTP 1.0
			size = ROOT_PATH_SIZE + request_size + 1;
			ressource_path = malloc(size*sizeof(char));

			//On recopie le début de l'arborescence (propre au serveur)
			for(i = 0 ; i < ROOT_PATH_SIZE ; i++) ressource_path[i] = ROOT_PATH[i];

			//request-target
			j = 0;
			while(j < request_target->length) {
				ressource_path[i] = request_target->base[j];
				i++;
				j++;
			}

			//On rajoute un "index.html" si besoin
			if(request_size > request_target->length) { //si on doit rajouter index.html
				for(j = 0 ; j < DEFAULT_FILE_LENGTH ; j++) {
					ressource_path[i] = default_file[j];
				}
			}

			ressource_path[i] = '\0';

			printf("Path de la ressource après concaténation : \"%.*s\"\n", size, ressource_path);

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


/*
* Retourne le code de réponse HTTP vérifiant la sémantique de la méthode.
* Retourne 0 si la méthode est valide, le code d'erreur correspondant sinon.
*/
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
	} else if(body->base != NULL && body->length != 0) { // Vérification de la présence et conformité du body
		printf("Body de longueur %d présent, vérification de la conformité avec Content-Length...\n", body->length);
		if(content_length->base != NULL) {

			//printf("Le Content-Length = %.*s", content_length->length, content_length->base);
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
