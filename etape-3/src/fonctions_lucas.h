#include "utils.h"

//Constante pour prévenir l'overflow des entiers
#define MAX_DIGITS 9

// Gestion du multi-site et de l'arborescence
#define ROOT_PATH "./../www/"
#define KNOWN_HOSTS_LIST {"www.example.com", "www.supersitedu47.wow"}
#define KNOWN_HOSTS_PATHS {"example", "supersitedu47"}
#define KNOWN_HOSTS_COUNT 2

int string_to_int(string s);
int validMethod(string *method);
int isAvailable(string *request_target, string *host);
