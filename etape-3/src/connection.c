#include "utils.h"
#include "connection.h"

#define NULL 0

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