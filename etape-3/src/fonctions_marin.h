
int are_unique_headers(void *root);

//int is_http_version_ok(void *root);
int is_http_version_ok(string *http_version, string *host_header);

const char *get_mime_type(char *filename);