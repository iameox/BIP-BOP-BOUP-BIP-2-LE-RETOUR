#define CLOSE_CONNECTION 1
#define SEND_CONNECTION_HEADER 2

// Détermine si la connexion doit être fermée ou non, et si la décision doit être écrite dans la réponse ou non
// Applique l'algorithme de la partie 6.3 de la RFC 7230
int get_connection_state(string *http_version, string *connection_option);