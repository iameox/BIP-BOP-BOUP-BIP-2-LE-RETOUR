#include <stdio.h>
#include <stdlib.h>
#include <magic.h>

#include "utils.h"
#include "resource.h"
#include "normalization.h"


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

/*
	Fonction qui renvoie le type mime d'un fichier à l'aide de la librairie magic.h
	
	Prend en argument le nom du fichier dont on veut connaitre le type mime
*/
string* get_mime_type_with_magic(char *filename)
{
	/*
		Crée un cookie qui "contient" l'option MAGIC_MIME_TYPE
		Evite d'inclure le flag correspondant à chaque utilisation du cookie, entre autre
	*/
	magic_t cookie = magic_open(MAGIC_MIME_TYPE);
	string *result = malloc(sizeof(string));
	result->base = NULL;
	result->length = 0;

	if (cookie == NULL)
	{
		printf("\n\n Echec lors de la création du magic cookie. \n\n");
		magic_close(cookie);
		/*
			Retourner une string de base NULL permet de continuer l'exécution du programme
		*/
		return result;
	}

	/*
		Charge tous les types mime requis pour la bonne exécution de toutes les fonctions magic suivantes
	*/
	int is_load = magic_load(cookie, "/usr/share/misc/magic.mgc");
	if (is_load == -1)
	{
		printf("\n\n Erreur lors du chargement du fichier /usr/share/misc/magic.mgc : %s. \n\n", magic_error(cookie));
		/*
			Retourner une string de base NULL permet de continuer l'exécution du programme
		*/
		return result;
	}

	/*
		magic_file permet de récupérer les informations sur le fichier de nom filename
		le cookie précise que l'information récupérée est directement le type mime du fichier, en cas de succès
	*/
	const char *mime_type;
	mime_type = magic_file(cookie, filename);

	if (mime_type == NULL)
	{
		printf("\n\n Erreur lors de la récupération des informations sur le fichier donné avec magic_file : %s. \n\n", magic_error(cookie));
		magic_close(cookie);
		/*
			Retourner une string de base NULL permet de continuer l'exécution du programme
		*/
		return result;
	}

	copy_to_string(mime_type, result);
	/*
		Efface toutes les données en mémoire associées au cookie. Oui, TOUTES.
	*/
	magic_close(cookie);

	return result;
}



/*
	Fonction qui renvoie le type mime d'un fichier SANS l'aide de la librairie magic.h
	
	Prend en argument une string contenant le nom du fichier dont on veut connaitre le type mime
*/
string* get_mime_type(string *filename)
{
	string *to_return;
	int extension_length = 0;

	/*
		Tant que l'on n'a pas trouvé le dernier . (marquant le début de l'extension du fichier)
		OU que l'on n'est pas arrivé au tout début du nom du fichier
		On regarde le caractère précédent
	*/
	while (filename->length - extension_length > 0 && filename->base[filename->length -1 - extension_length] != '.')
	{
		extension_length++;
	}
	/*
		Instruction utile si on a trouvé un .
		Permet de se mettre au début de l'extension, donc juste après le . trouvé
	*/
	extension_length--;

	/*
		Si on est arrivé au tout début du fichier, on réessaye avec la librairie magic.h
	*/
	if (extension_length == 0) to_return = get_mime_type_with_magic(filename->base);
	/*
		Sinon on essaye de déterminer l'extension en la comparant avec les 66 extensions les plus connues
	*/
	else
	{
		string extension = { filename->base + filename->length - 1 - extension_length, extension_length };
		char *extensions[] = EXTENSIONS;
		char *types_mime[] = EXTENSIONS_TYPES_MIME;
		
		int index_extension = 0;
		int is_found = false;

		/*
			Tant que l'extension n'a pas été trouvée, on compare notre extension avec l'extension suivante
		*/
		while (!is_found && index_extension < NB_EXTENSIONS)
		{
			is_found = compare_strings(&extension, extensions[index_extension]);
			if (!is_found) index_extension++;
		}

		/*
			Si l'instruction a été trouvée, on la renvoie
		*/
		if (is_found)
		{
			to_return = malloc(sizeof(string));
			copy_to_string(types_mime[index_extension], to_return);

		}
		/*
			Sinon on réessaye avec la librairie magic.h
		*/
		else to_return = get_mime_type_with_magic(filename->base);
	}

	return to_return;
}