#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "request.h"
#include "utils.h"
#include "connection.h"
#include "response.h"

void send_response(string *method, int status_code, string *path, string *mime_type, int connection_state, message *request) {
    FILE *f = NULL;
    struct stat st;
    char *responses[] = RESPONSES,
         *response_bodies[] = RESPONSE_BODIES,
         *content = NULL,
         *content_length;
    int codes[] = STATUS_CODES,
        response_lengths[] = RESPONSE_LENGTHS,
        response_body_lengths[] = RESPONSE_BODY_LENGTHS,
        length, i;

    for (i = 0; i < NB_STATUS_CODES; i++) {
        if (status_code == codes[i]) {
            writeDirectClient(request->clientId, responses[i], response_lengths[i]);

            if (!is_between_int(status_code, 100, 199) && status_code != 204) {
                if ((connection_state & SEND_CONNECTION_HEADER) == SEND_CONNECTION_HEADER) {
                    printf("%d %d\n", connection_state, connection_state & CLOSE_CONNECTION);
                    if ((connection_state & CLOSE_CONNECTION) == CLOSE_CONNECTION) writeDirectClient(request->clientId, "\r\nConnection: close", 19);
                    else writeDirectClient(request->clientId, "\r\nConnection: keep-alive", 24);
                }

                writeDirectClient(request->clientId, "\r\nContent-length: ", 18);
                if (status_code == 200) {
                    f = fopen(path->base, "r");
                    stat(path->base, &st);

                    content = malloc(st.st_size * sizeof(char));
                    fread(content, 1, st.st_size, f);

                    content_length = int_to_string(st.st_size, &length);

                } else content_length = int_to_string(response_body_lengths[i - 1], &length);

                writeDirectClient(request->clientId, content_length, length);
                free(content_length);

                if (!compare_strings(method, "HEAD")) {
                    writeDirectClient(request->clientId, "\r\nContent-Type: ", 16);

                    if (status_code == 200) {
                        writeDirectClient(request->clientId, mime_type->base, mime_type->length);

                        writeDirectClient(request->clientId, "\r\n\r\n", 4);
                        writeDirectClient(request->clientId, content, st.st_size);

                    } else {
                        writeDirectClient(request->clientId, "text/html\r\n\r\n", 13);
                        writeDirectClient(request->clientId, response_bodies[i - 1], response_body_lengths[i - 1]);
                    }
                }
            }
        }
    }

    if (path->base != NULL) free(path->base);
    if (mime_type != NULL) {
        free(mime_type->base);
        free(mime_type);
    }
    if (content != NULL) free(content);
    if (f != NULL) fclose(f);

    endWriteDirectClient(request->clientId);
}
