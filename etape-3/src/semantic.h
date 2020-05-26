//Liste des headers reconnus et traités par le serveur
#define HEADERS { "Connection_header", "Content_Length_header", "Content_Type_header", "Cookie_header", "Transfer_Encoding_header", "Expect_header", "Host_header", "Accept_header", "Accept_Charset_header", "Accept_Encoding_header", "Accept_Language_header", "Referer_header", "User_Agent_header" }
#define NB_HEADERS 13

//Liste des méthodes implémentées par le serveur
#define KNOWN_METHODS {"GET", "HEAD", "POST"}
#define METHODS_NUMBER 3

/*
	Fonction qui renvoie 0 si deux headers de même type sont présents, 1 sinon
*/
int are_unique_headers(void *root);

/*
	Fonction qui renvoie 505 si la version HTTP est différente de 1.0 ou 1.1 ;
	renvoie 400 si la version est HTTP/1.1 mais que le host header n'est pas présent ;
	renvoie 200 sinon

	Prend en argument :
	- une string contenant la version HTTP
	- une string contenant le host header s'il est présent, NULL sinon
*/
int is_http_version_ok(string *http_version, string *host_header);

/*
* Vérifie qu'une méthode donnée (passée sous forme de string) fait bien parties de celles qui sont implémentées par le serveur.
* La liste des méthodes implémentée est définie dans semantic.h
* Retourne true si la méthode est implémentée, false sinon.
*/
int validMethod(string *method);

/*
* Retourne le code de réponse HTTP vérifiant la sémantique de la méthode.
* Paramètres : les champs method et body et le header Content-Length de la requête à analyser
* Si l'un de ces champs n'est pas présent, il faut envoyer une string dont la base est égale à NULL et de taille 0.
* Retourne 200 si la méthode est valide, le code d'erreur à renvoyer sinon.
*/
int methodCompliance(string *method, string *body, string *content_length);