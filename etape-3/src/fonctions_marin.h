
int are_unique_headers(void *root);

//int is_http_version_ok(void *root);
int is_http_version_ok(string *http_version, string *host_header);


string* get_mime_type(string *filename);


string* get_mime_type_with_magic(char *filename);