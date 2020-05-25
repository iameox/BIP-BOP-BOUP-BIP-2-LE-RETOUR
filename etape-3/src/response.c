#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "request.h"
#include "utils.h"
#include "connection.h"
#include "response.h"

void send_response(string *method, int status_code, string *path, string *mime_type, int connection_state, int client_id) {
    FILE *f = NULL;
    struct stat st;
    char *responses[] = RESPONSES,
         *response_bodies[] = RESPONSE_BODIES,
         *content = NULL,
         *content_length;
    int codes[] = STATUS_CODES,
        response_lengths[] = RESPONSE_LENGTHS,
        response_body_lengths[] = RESPONSE_BODY_LENGTHS,
        length, i = 0;

    while (codes[i] != status_code && i < NB_STATUS_CODES) i++;

    if (status_code == codes[i]) {
        writeDirectClient(client_id, responses[i], response_lengths[i]);

        if ((connection_state & SEND_CONNECTION_HEADER) == SEND_CONNECTION_HEADER) {
            if ((connection_state & CLOSE_CONNECTION) == CLOSE_CONNECTION) writeDirectClient(client_id, "\r\nConnection: close", 19);
            else writeDirectClient(client_id, "\r\nConnection: keep-alive", 24);
        }

        if (!is_between_int(status_code, 100, 199) && status_code != 204) {
            writeDirectClient(client_id, "\r\nContent-length: ", 18);
            if (status_code == 200) {
                f = fopen(path->base, "r");
                stat(path->base, &st);

                content = malloc(st.st_size * sizeof(char));
                fread(content, 1, st.st_size, f);

                content_length = int_to_string(st.st_size, &length);

            } else content_length = int_to_string(response_body_lengths[i - 1], &length);

            writeDirectClient(client_id, content_length, length);
            free(content_length);

            if (method->base == NULL || !compare_strings(method, "HEAD")) {
                if (status_code == 200) {
                    if (mime_type != NULL && mime_type->base != NULL) {
                        writeDirectClient(client_id, "\r\nContent-Type: ", 16);
                        writeDirectClient(client_id, mime_type->base, mime_type->length);
                    }

                    writeDirectClient(client_id, "\r\n\r\n", 4);
                    writeDirectClient(client_id, content, st.st_size);

                } else {
                    writeDirectClient(client_id, "\r\nContent-Type: text/html\r\n\r\n", 29);
                    writeDirectClient(client_id, response_bodies[i - 1], response_body_lengths[i - 1]);
                }
            }
        }
    }

    free(path->base);
    if (mime_type != NULL) {
        free(mime_type->base);
        free(mime_type);
    }
    free(content);
    if (f != NULL) fclose(f);

    endWriteDirectClient(client_id);
}
