// Gestion du multi-site et de l'arborescence
#define ROOT_PATH "./../www/"
#define KNOWN_HOSTS_LIST {"www.example.com:8080", "www.supersitedu47.wow:8080", "localhost:8080"}
#define KNOWN_HOSTS_PATHS {"example", "supersitedu47", "example"}
#define KNOWN_HOSTS_COUNT 3

int validMethod(string *method);
char *isAvailable(string *request_target, string *host, int *len);
int methodCompliance(string *method, string *body, string *content_length);