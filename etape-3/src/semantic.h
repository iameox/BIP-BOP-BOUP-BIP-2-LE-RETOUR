#define HEADERS { "Connection_header", "Content_Length_header", "Content_Type_header", "Cookie_header", "Transfer_Encoding_header", "Expect_header", "Host_header", "Accept_header", "Accept_Charset_header", "Accept_Encoding_header", "Accept_Language_header", "Referer_header", "User_Agent_header" }
#define NB_HEADERS 13

#define KNOWN_METHODS {"GET", "HEAD", "POST"}
#define METHODS_NUMBER 3


int are_unique_headers(void *root);

//int is_http_version_ok(void *root);
int is_http_version_ok(string *http_version, string *host_header);

/*
* Vérifie qu'une méthode donnée fait bien parties de celles qui sont implémentées par le serveur.
* La liste des méthodes implémentée est définie dans semantic.h
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