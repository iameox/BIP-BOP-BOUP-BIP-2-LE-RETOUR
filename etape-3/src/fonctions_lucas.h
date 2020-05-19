// Gestion du multi-site et de l'arborescence
#define ROOT_PATH "./../www/"
#define KNOWN_HOSTS_LIST {"www.example.com:8080", "www.supersitedu47.wow:8080", "www.fake.com:8080", "www.toto.com:8080"}
#define KNOWN_HOSTS_PATHS {"example", "supersitedu47", "fake", "toto"}
#define KNOWN_HOSTS_COUNT 4
#define DEFAULT_FILE_PATH "index.html"
#define DEFAULT_FILE_LENGTH 10

int validMethod(string *method);
char *isAvailable(string *request_target, string *host, int *len);
int methodCompliance(string *method, string *body, string *content_length);
