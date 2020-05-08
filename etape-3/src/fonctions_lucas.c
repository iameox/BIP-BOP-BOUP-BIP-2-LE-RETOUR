#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "fonctions_lucas.h"
#include "normalization.h"



/*
* Convertit une chaine de caractères en entier.
* Si un caractère qui n'est pas un chiffre est rencontré, la fonction retourne -1
* Sinon Si il y a plus de MAX_DIGITS chiffres, le nombre est tronqué.
* Sinon, retourne la valeur de la chaine associée
*/
int string_to_int(string s) {
	int i = 0, value = 0, idigit;

	while(i < s.length && i < MAX_DIGITS && value != -1) {
		if (is_between(s.base[i], '0', '9')) {
			idigit = char_to_int(s.base[i]);
			value = value*10 + idigit;
		} else value = -1;
		i++;
	}

	return value;
}


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
			printf("la méthode c'est un %s\n", methodes[i]);
			valid = true;
		}
		i++;
	}

	if(!valid) {
		printf("Pas de méthode trouvée\n");
	}

	return valid;
}

/*
* Indique si une ressource décrite par le couple (request_target, host) est disponible.
* On considère que la request_target est de type origin-form
* Retourne 1 si la ressource est disponible, 0 sinon
*/

// A changer le return, autant return le fichier
// ATTENTION je close rien, il faudra close quand on a fini
int isAvailable(string *request_target, string *host) {
	int available = true;
	int i = 0, j;

	char *hosts_lists[] = KNOWN_HOSTS_LIST;
	char *hosts_paths[] = KNOWN_HOSTS_PATHS;
	char *website_path = NULL;

	//Normalize according to ABNF
	normalize_request_target(request_target);

	while(i < KNOWN_HOSTS_COUNT && website_path == NULL) {
		if(compare_strings(host, hosts_lists[i])) website_path = hosts_paths[i];
		i++;
	}

	if(website_path != NULL) {
		char *path = malloc((strlen(ROOT_PATH) + strlen(website_path) + request_target->length)*sizeof(char));

		//On recopie le début de l'arborescence (propre au serveur)
		for(i = 0 ; i < strlen(ROOT_PATH) ; i++) path[i] = ROOT_PATH[i];
		//host
		j = 0;
		while(j < strlen(website_path)) {
			path[i] = website_path[j];
			i++;
			j++;
		}
		//request-target
		j = 0;
		while(j < request_target->length) {
			path[i] = request_target->base[j];
			i++;
			j++;
		}

		printf("on cherche la ressource avec le path = \"");
		for(i = 0 ; i < strlen(ROOT_PATH) + strlen(website_path) + request_target->length ; i++) printf("%c", path[i]);
		printf("\"\n");

		FILE * ressource = fopen(path, "r");

		if(ressource == NULL) {
			printf("Ressource pas accessible\n");
			available = false;
		}

		free(path);
	} else {
		printf("Site pas trouvé\n");
		available = false;
	}


	return available;
}
