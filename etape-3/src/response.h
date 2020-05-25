#define STATUS_CODES { 200, 400, 404, 411, 501, 505 }
#define NB_STATUS_CODES 6

#define RESPONSE_200 "HTTP/1.1 200 OK"
#define RESPONSE_400 "HTTP/1.1 400 Bad Request"
#define RESPONSE_404 "HTTP/1.1 404 Not Found"
#define RESPONSE_411 "HTTP/1.1 411 Length Required"
#define RESPONSE_501 "HTTP/1.1 501 Not Implemented"
#define RESPONSE_505 "HTTP/1.1 505 HTTP Version Not Supported"
#define RESPONSE_200_LENGTH 15
#define RESPONSE_400_LENGTH 24
#define RESPONSE_404_LENGTH 22
#define RESPONSE_411_LENGTH 28
#define RESPONSE_501_LENGTH 28
#define RESPONSE_505_LENGTH 39
#define RESPONSES { RESPONSE_200, RESPONSE_400, RESPONSE_404, RESPONSE_411, RESPONSE_501, RESPONSE_505 }
#define RESPONSE_LENGTHS { RESPONSE_200_LENGTH, RESPONSE_400_LENGTH, RESPONSE_404_LENGTH, RESPONSE_411_LENGTH, RESPONSE_501_LENGTH, RESPONSE_505_LENGTH }

#define RESPONSE_400_BODY "<!DOCTYPE html><html><head><title>400 - Bad Request</title></head><body><h1>400 - Bad Request</h1><p>Sorry but your request wasn't accepted by the server.</p></body></html>"
#define RESPONSE_404_BODY "<!DOCTYPE html><html><head><title>404 - Not Found</title></head><body><h1>404 - Not Found</h1><p>Sorry but the page you are requesting wasn't found.</p></body></html>"
#define RESPONSE_411_BODY "<!DOCTYPE html><html><head><title>411 - Length Required</title></head><body><h1>411 - Length Required</h1><p>The request contains a payload but no Content-Length header.</p></body></html>"
#define RESPONSE_501_BODY "<!DOCTYPE html><html><head><title>501 - Not Implemented</title></head><body><h1>501 - Not Implemented</h1><p>Sorry but this method is either not implemented by the server or malformed.</p></body></html>"
#define RESPONSE_505_BODY "<!DOCTYPE html><html><head><title>505 - HTTP Version Not Supported</title></head><body><h1>505 - HTTP Version Not Supported</h1><p>Sorry but this HTTP version isn't supported by the server. Its current version is 1.1.</p></body></html>"
#define RESPONSE_400_BODY_LENGTH 172
#define RESPONSE_404_BODY_LENGTH 166
#define RESPONSE_411_BODY_LENGTH 187
#define RESPONSE_501_BODY_LENGTH 202
#define RESPONSE_505_BODY_LENGTH 235

#define RESPONSE_BODIES { RESPONSE_400_BODY, RESPONSE_404_BODY, RESPONSE_411_BODY, RESPONSE_501_BODY, RESPONSE_505_BODY }
#define RESPONSE_BODY_LENGTHS { RESPONSE_400_BODY_LENGTH, RESPONSE_404_BODY_LENGTH, RESPONSE_411_BODY_LENGTH, RESPONSE_501_BODY_LENGTH, RESPONSE_505_BODY_LENGTH }

#define CLOSE_CONNECTION 1
#define SEND_CONNECTION_HEADER 2

// Détermine si la connexion doit être fermée ou non, et si la décision doit être écrite dans la réponse ou non
// Applique l'algorithme de la partie 6.3 de la RFC 7230
int get_connection_state(string *http_version, string *connection_option);

// Envoie la réponse générée par l'analyse de la requête
// Gère les status codes 200, 400, 404, 411, 501 et 505 
// Gère les headers Content-Type, Content-Length, Connection et Date
void send_response(string *method, int status_code, string *path, string *mime_type, int connection_state, int client_id);