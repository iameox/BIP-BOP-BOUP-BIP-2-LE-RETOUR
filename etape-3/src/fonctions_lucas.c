#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "fonctions_lucas.h"

int validMethod(char * method, int len) {
	int valid = false;
	char *methodes[] = KNOWN_METHODS;
	int i = 0;
	while(!valid && i < METHODS_NUMBER) {
		if(!strncmp(method, methodes[i], min(len, strlen(methodes[i])))) {
			printf("la méthode c'est un %s\n", methodes[i]);
			valid = true;
		}
		i++;
	}

	if(!valid) {
		printf("pas de méthode trouvée\n");
	}

	return valid;
}

/*
* Indique si une ressource décrite par le couple (request_target, host) est disponible.
* On considère que la request_target est de type origin-form
* Retourne 1 si la ressource est disponible, 0 sinon
*/
int isAvailable(string *request_target, string *host) {
	int available = true;
	int i = 0;

	char *hosts_lists[] = KNOWN_HOSTS_LIST;
	char *hosts_paths[] = KNOWN_HOSTS_PATHS;
	char *website_path = NULL;

	while(i < KNOWN_HOSTS_COUNT && website_path == NULL) {
		if(compare_strings(host, hosts_lists[i])) website_path = hosts_paths[i];
		i++;
	}

	if(website_path != NULL) {
		char *path = malloc((strlen(ROOT_PATH) + request_target->length + host->length)*sizeof(char));

		//On recopie le début de l'arborescence (propre au serveur)
		for(i = 0 ; i < strlen(ROOT_PATH) ; i++) path[i] = ROOT_PATH[i];
		//host
		for( ; i < strlen(website_path) ; i++) path[i] = website_path[i];
		//request-target
		for( ; i < request_target->length ; i++) path[i] = request_target->base[i];


		for(i = 0 ; i < strlen(ROOT_PATH) + request_target->length + host->length ; i++) printf("%c", path[i]);
		printf("\n");

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
