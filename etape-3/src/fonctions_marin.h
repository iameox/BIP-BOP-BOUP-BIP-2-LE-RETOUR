//#ifndef __FONCTIONS_MARIN__
//#define __FONCTIONS_MARIN__

#include "utils.h" // pour définir le type string

int are_unique_headers(void *root);

//int is_http_version_ok(void *root);
int is_http_version_ok(string *http_version, string *host_header);

const char *mr_mime(char *filename);

//#endif