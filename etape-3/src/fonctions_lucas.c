#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "fonctions_lucas.h"
#include "normalization.h"


/*
* Vérifie qu'une méthode donnée fait bien parties de celles qui sont implémentées par le serveur.
* La liste des méthodes implémentée est définie dans XXX.h
* Retourne 1 si la méthode est implémentée, 0 sinon.
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
* Si l'un de ces champs n'est pas présent, il faut envoyer une string de base égale à NULL et de taille 0.
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


/*
* Indique si une ressource décrite par le couple (request_target, host) est disponible.
* host et request_target ne doivent PAS valoir NULL
* On considère que la request_target est de type origin-form
* Si host->base vaut NULL, on considère que la requête est du HTTP 1.0 (on utilise que la request-target)
* Retourne un char* pointant vers le path de la requête et écrit dans len si la ressource existe, renvoie NULL sinon
*/

// ATTENTION il faut free le char * retourné après utilisation (si différent de NULL)
char *isAvailable(string *request_target, string *host, int *len) {
	int i, j, final_path_size, 
	    hosts_sizes[] = KNOWN_HOSTS_SIZES,
	    website_path_size, 
	    request_size;
	char *hosts_lists[] = KNOWN_HOSTS_LIST,
	     *hosts_paths[] = KNOWN_HOSTS_PATHS,
	     *website_path = NULL, 
	     *default_file = DEFAULT_FILE_PATH, 
	     *final_path = NULL;

	normalize_request_target(request_target); //Normalisation 
	request_size = request_target->length;

	if(request_target->base[request_target->length - 1] == '/') { //Si la request-target est un répertoire, on rajoute un fichier par défaut ("index.html")
		request_size += DEFAULT_FILE_LENGTH;
	}

	if(host->base != NULL) { // Récupération du nom de dossier correspondant au site
		i = 0;
		while(i < KNOWN_HOSTS_COUNT && website_path == NULL) {
			if(compare_strings(host, hosts_lists[i])) {
				website_path = hosts_paths[i];
				website_path_size = hosts_sizes[i];
			}
			i++;
		}
	} else { // Si le champ Host n'est pas présent, on utilise un site par défaut
		website_path = hosts_paths[DEFAULT_HOST_INDEX];
		website_path_size = hosts_sizes[DEFAULT_HOST_INDEX];
	}


	if(website_path != NULL) {
		final_path_size = ROOT_PATH_SIZE + website_path_size + request_size + 1; // On rajoute 1 emplacement pour la sentinelle
		final_path = malloc(final_path_size*sizeof(char));

		//On recopie le début de l'arborescence (propre au serveur)
		for(i = 0 ; i < ROOT_PATH_SIZE ; i++) final_path[i] = ROOT_PATH[i];

		//On recopie le dossier qui correspond à l'host
		for(j = 0 ; j < website_path_size ; j++) {
			final_path[i] = website_path[j];
			i++;
		}

		//On recopie le champ request-target
		for(j = 0 ; j < request_target->length ; j++) {
			final_path[i] = request_target->base[j];
			i++;
		}

		//On rajoute le fichier par défaut si besoin
		if(request_size > request_target->length) {
			for(j = 0 ; j < DEFAULT_FILE_LENGTH ; j++) {
				final_path[i] = default_file[j];
				i++;
			}
		}
		final_path[i] = '\0'; //Ajout d'une sentinelle (nécessaire pour fopen)

		FILE *ressource = fopen(final_path, "r"); //On ouvre le fichier pour tester l'existence

		if(ressource != NULL) {
			if(fclose(ressource) != 0) printf("Erreur lors de la fermeture de la ressource.\n");
			if(len != NULL) *len = final_path_size;
		} else { //Si la ressource n'est pas disponible, on libère tout et on retourne NULL
			free(final_path); 
			final_path = NULL;
		}
	}

	return final_path;
}