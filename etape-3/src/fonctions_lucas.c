#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "constants.h"

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
int isAvailable(char * request_target, int request_target_length, char * host, int host_size) {
	int available = true;
	int i = 0;

	char *name = malloc((strlen(ROOT) + request_target_length + host_size)*sizeof(char));

	//On recopie le début de l'arborescence (propre au serveur)
	for(i = 0 ; i < strlen(ROOT) ; i++) name[i] = ROOT[i];
	//host
	for( ; i < request_target_length ; i++) name[i] = request_target[i];
	//request-target
	for( ; i < host_size ; i++) name[i] = host[i];

	//On considère que c'est du origin-form
	FILE * ressource = fopen(name, "r");
	
	if(ressource == NULL) {
		available = false;
	}

	free(name);
	return available;
}