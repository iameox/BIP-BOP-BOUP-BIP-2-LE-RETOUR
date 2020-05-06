#include "utils.h"

#define ROOT_PATH "./../www/"
#define KNOWN_HOSTS_LIST {"www.example.com", "www.supersitedu47.wow"}
#define KNOWN_HOSTS_PATHS {"example", "supersitedu47"}
#define KNOWN_HOSTS_COUNT 2


int validMethod(char * method, int len);
int isAvailable(string *request_target, string *host);
