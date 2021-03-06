// Configuration du multi-site

//Arborescence racine (sera accedée pour tous les sites)
#define ROOT_PATH "./www/"
#define ROOT_PATH_SIZE 6

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

//Extensions gérées par le serveur
#define EXTENSIONS { "woff2", "xhtml", "docx", "epub", "html", "jpeg", "json", "midi", "mpeg", "mpkg", "pptx", "tiff", "weba", "webm", "webp", "woff", "xlsx", "3gp2", "aac", "abw", "arc", "avi", "azw", "bin", "bz2", "csh", "css", "csv", "doc", "eot", "gif", "htm", "ico", "ics", "jar", "jpg", "mid", "odp", "ods", "odt", "oga", "ogv", "ogx", "otf", "png", "pdf", "ppt", "rar", "rtf", "svg", "swf", "tar", "tif", "ttf", "vsd", "wav", "xls", "xml", "xul", "zip", "3gp", "bz", "js", "sh", "ts", "7z" }
#define EXTENSIONS_TYPES_MIME { "font/woff2", "application/xhtml+xml", "application/vnd.openxmlformats-officedocument.wordprocessingml.document", "application/epub+zip", "text/html", "image/jpeg", "application/json", "audio/midi", "video/mpeg", "application/vnd.apple.installer+xml", "application/vnd.openxmlformats-officedocument.presentationml.presentation", "image/tiff", "audio/webm", "video/webm", "image/webp", "font/woff", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet", "video/3gpp2", "audio/aac", "application/x-abiword", "application/octet-stream", "video/x-msvideo", "application/vnd.amazon.ebook", "application/octet-stream", "application/x-bzip2", "application/x-csh", "text/css", "text/csv", "application/msword", "application/vnd.ms-fontobject", "image/gif", "text/html", "image/x-icon", "text/calendar", "application/java-archive", "image/jpeg", "audio/midi", "application/vnd.oasis.opendocument.presentation", "application/vnd.oasis.opendocument.spreadsheet", "application/vnd.oasis.opendocument.text", "audio/ogg", "video/ogg", "application/ogg", "font/otf", "image/png", "application/pdf", "application/vnd.ms-powerpoint", "application/x-rar-compressed", "application/rtf", "image/svg+xml", "application/x-shockwave-flash", "application/x-tar", "image/tiff", "font/ttf", "application/vnd.visio", "audio/x-wav", "application/vnd.ms-excel", "application/xml", "application/vnd.mozilla.xul+xml", "application/zip", "video/3gpp", "application/x-bzip", "application/javascript", "application/x-sh", "application/typescript", "application/x-7z-compressed" }
#define NB_EXTENSIONS 66

// Normalise le champ request-target d'une URI
// Décode tous les "percent-encodings" et les "dot segments"
void normalize_request_target(string *request_target);

// Normalise une URI du point de vue des "percent encodings" (décode toutes les occurences)
void normalize_percent(string *str);

// Normalise une URI du point de vue des "dot segments"
// Applique l'algorithme de la partie 5.2.4 de la RFC 3986
void remove_dot_segments(string *str);

/*
* Indique si une ressource décrite par le couple (request_target, host) est disponible.
* host et request_target ne doivent PAS valoir NULL
* On considère que la request_target est de type origin-form
* Si host->base vaut NULL, on considère que la requête est du HTTP 1.0 (on utilisera un host par défaut)
* Retourne un char* pointant vers le chemin pour accéder à la ressource et écrit sa taille dans len (si elle existe), renvoie NULL sinon
*/

// ATTENTION il faut libérer la mémoire allouée au char * retourné après utilisation (si il est différent de NULL)
char *isAvailable(string *request_target, string *host, int *len);


/*
	Fonction qui renvoie le type mime d'un fichier à l'aide de la librairie magic.h
	
	Prend en argument le nom du fichier dont on veut connaitre le type mime
*/
string* get_mime_type(string *filename);

/*
	Fonction qui renvoie le type mime d'un fichier SANS l'aide de la librairie magic.h
	
	Prend en argument une string contenant le nom du fichier dont on veut connaitre le type mime
*/
string* get_mime_type_with_magic(char *filename);