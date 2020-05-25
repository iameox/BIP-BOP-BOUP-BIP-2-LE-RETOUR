#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "request.h"
#include "utils.h"
#include "response.h"

// Détermine si la connexion doit être fermée ou non, et si la décision doit être écrite dans la réponse ou non
// Applique l'algorithme de la partie 6.3 de la RFC 7230
int get_connection_state(string *http_version, string *connection_option) {
    int close = false,
        keep_alive = false,
        result = 0,
        major_version, minor_version;

    if (http_version->base != NULL) {
        major_version = char_to_int(http_version->base[5]);
        minor_version = char_to_int(http_version->base[7]);

        if (connection_option->base != NULL) {
            if (compare_strings(connection_option, "close")) close = true;
            else if (compare_strings(connection_option, "keep-alive")) keep_alive = true;
            // Dans le cas où la requête contient un champ Connection-header, on indique la décision prise dans la réponse
            result |= SEND_CONNECTION_HEADER;
        }

        if (close || !(major_version == 1 && (minor_version >= 1 || (minor_version == 0 && keep_alive)))) result |= CLOSE_CONNECTION;
    
    // Dans le cas où le parsing échoue et qu'il n'y a pas de champ HTTP-version, on ferme la connexion
    // Le client doit en être informé puisqu'il souhaitait peut-être que la connexion persiste
    } else result = SEND_CONNECTION_HEADER | CLOSE_CONNECTION;

    return result;
}

// Envoie la réponse générée par l'analyse de la requête
// Gère les status codes 200, 400, 404, 411, 501 et 505 
// Gère les headers Content-Type, Content-Length, Connection et Date
void send_response(string *method, int status_code, string *path, string *mime_type, int connection_state, int client_id) {
    FILE *f = NULL;
    char *responses[] = RESPONSES,
         *response_bodies[] = RESPONSE_BODIES,
         *content = NULL,
         *content_length, date[29];
    int codes[] = STATUS_CODES,
        response_lengths[] = RESPONSE_LENGTHS,
        response_body_lengths[] = RESPONSE_BODY_LENGTHS,
        length, i = 0;
    time_t t;
    struct tm *tm;
    struct stat st;

    // Récupère l'index dans le tableau des status codes d'erreur
    while (codes[i] != status_code && i < NB_STATUS_CODES) i++;

    if (status_code == codes[i]) {
        // Affiche la response-line correspondant au status code
        writeDirectClient(client_id, responses[i], response_lengths[i]);
        
        // Récupère la date et envoie le header correspondant
        t = time(NULL);
        tm = gmtime(&t);
        strftime(date, 80, "%a, %d %h %Y %H:%M:%S GMT", tm);
        writeDirectClient(client_id, "\r\nDate: ", 8);
        writeDirectClient(client_id, date, 29);

        // N'envoie le header Connection que si c'est nécessaire
        if ((connection_state & SEND_CONNECTION_HEADER) == SEND_CONNECTION_HEADER) {
            if ((connection_state & CLOSE_CONNECTION) == CLOSE_CONNECTION) writeDirectClient(client_id, "\r\nConnection: close", 19);
            else writeDirectClient(client_id, "\r\nConnection: keep-alive", 24);
        }

        // Si le code à renvoyer est un 1xx ou un 204 (Not Modified), il ne faut pas envoyer de header Content-length (et donc de body)
        if (!is_between_int(status_code, 100, 199) && status_code != 204) {
            // Récupère le taille de contenu et envoie le header correspondant
            writeDirectClient(client_id, "\r\nContent-length: ", 18);
            
            // Dans le cas d'un status code de 200, le fichier existe forcément (il a été préalablement testé)
            // On récupère donc sa longueur
            if (status_code == 200) {
                f = fopen(path->base, "r");
                stat(path->base, &st);

                content = malloc(st.st_size * sizeof(char));
                fread(content, 1, st.st_size, f);

                content_length = int_to_string(st.st_size, &length);

            // Sinon, on envoie la longueur du body correspondant au status code
            } else content_length = int_to_string(response_body_lengths[i - 1], &length);

            writeDirectClient(client_id, content_length, length);
            free(content_length);

            // Récupère le type MIME et envoie éventuellement le header correspondant
            // Dans le cas d'un status code de 200, le fichier existe forcément (il a été préalablement testé)
            // On récupère donc son type mime
            if (status_code == 200) {
                // Si le type MIME ne peut pas être déterminé, mieux vaut ne rien envoyer
                if (mime_type != NULL && mime_type->base != NULL) {
                    writeDirectClient(client_id, "\r\nContent-Type: ", 16);
                    writeDirectClient(client_id, mime_type->base, mime_type->length);
                }

            // Sinon, on envoie le type mime de la page HTMl d'erreur
            } else writeDirectClient(client_id, "\r\nContent-Type: text/html", 25);

            // Envoie le body de la réponse
            // Si la méthode est un HEAD, il ne faut pas envoyer de body
            if (method->base == NULL || !compare_strings(method, "HEAD")) {
                writeDirectClient(client_id, "\r\n\r\n", 4);

                if (status_code == 200) writeDirectClient(client_id, content, st.st_size);
                else writeDirectClient(client_id, response_bodies[i - 1], response_body_lengths[i - 1]);
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

    // Fermeture de la connexion en écriture
    endWriteDirectClient(client_id);
}
