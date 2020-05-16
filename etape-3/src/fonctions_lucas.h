#include "utils.h"

// Gestion du multi-site et de l'arborescence
#define ROOT_PATH "./../www/"
#define KNOWN_HOSTS_LIST {"www.example.com", "www.supersitedu47.wow"}
#define KNOWN_HOSTS_PATHS {"example", "supersitedu47"}
#define KNOWN_HOSTS_COUNT 2

int validMethod(string *method);
char *isAvailable(string *request_target, string *host, int *len);
int methodCompliance(string *method, string *body, string *content_length);