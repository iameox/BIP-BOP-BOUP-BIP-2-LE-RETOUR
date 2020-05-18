#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "request.h"
#include "utils.h"

void send_response(string *method, int status_code, string *path, message *request) {
    FILE *f = fopen(path->base, "r");
    struct stat st;
    int length;
    char *content;

    stat(path->base, &st);
    content = malloc(st.st_size * sizeof(char));
    fread(content, 1, st.st_size, f);

    writeDirectClient(request->clientId, "HTTP/1.1 200 OK\r\n", 18);

    if (!is_between_int(status_code, 100, 199) && status_code != 204) {
        writeDirectClient(request->clientId, "Content-length: ", 16);

        char *s = int_to_string(st.st_size, &length);
        writeDirectClient(request->clientId, s, length);
        free(s);
        writeDirectClient(request->clientId, "\r\n", 2);
    }

    writeDirectClient(request->clientId, "\r\n", 2);

    if (!compare_strings(method, "HEAD")) {
        writeDirectClient(request->clientId, content, st.st_size);
    }
    
    free(content);
    fclose(f);
}