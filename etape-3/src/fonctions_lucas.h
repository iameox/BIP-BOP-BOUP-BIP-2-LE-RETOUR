// Configuration du multi-site

//Arborescence racine (sera accedée pour tous les sites)
#define ROOT_PATH "./../www/"
#define ROOT_PATH_SIZE 9

//Liste des sites hébergés, et leurs dossiers correspondants
#define KNOWN_HOSTS_LIST {"www.example.com:8080", "www.supersitedu47.wow:8080", "www.fake.com:8080", "www.toto.com:8080"}
#define KNOWN_HOSTS_PATHS {"example", "supersitedu47", "fake", "toto"}
#define KNOWN_HOSTS_SIZES {7, 13, 4, 4}
#define KNOWN_HOSTS_COUNT 4

//Site par défaut (pour HTTP 1.0)
#define DEFAULT_HOST_INDEX 0

//Nom du fichier par défaut (lorsque la requête est un répertoire)
#define DEFAULT_FILE_PATH "index.html"
#define DEFAULT_FILE_LENGTH 10

/*
* Vérifie qu'une méthode donnée fait bien parties de celles qui sont implémentées par le serveur.
* La liste des méthodes implémentée est définie dans XXX.h
* Retourne 1 si la méthode est implémentée, 0 sinon.
*/
int validMethod(string *method);

/*
* Retourne le code de réponse HTTP vérifiant la sémantique de la méthode.
* Paramètres : les champs method et body et le header Content-Length de la requête à analyser
* Si l'un de ces champs n'est pas présent, il faut envoyer une string de base égale à NULL et de taille 0.
* Retourne 200 si la méthode est valide, le code d'erreur à renvoyer sinon.
*/
int methodCompliance(string *method, string *body, string *content_length);

/*
* Indique si une ressource décrite par le couple (request_target, host) est disponible.
* host et request_target ne doivent PAS valoir NULL
* On considère que la request_target est de type origin-form
* Si host->base vaut NULL, on considère que la requête est du HTTP 1.0 (on utilise que la request-target)
* Retourne un char* pointant vers le path de la requête et écrit dans len si la ressource existe, renvoie NULL sinon
*/

// ATTENTION il faut free le char * retourné après utilisation (si différent de NULL)
char *isAvailable(string *request_target, string *host, int *len);