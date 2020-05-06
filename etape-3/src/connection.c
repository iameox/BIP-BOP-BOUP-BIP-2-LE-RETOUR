#include "request.h"
#include "utils.h"

// A WOOSH + .H
// Applique l'algorithme de la partie 6.3 de la RFC 7230
void manage_connection(string *http_version, string *connection_option, int client_id) {
    int major_version = char_to_int(http_version->base[5]),
        minor_version = char_to_int(http_version->base[7]),
        close = false,
        keep_alive = false;

    if (connection_option != NULL) {
        if (compare_strings(connection_option, "host")) close = true;
        else if (compare_strings(connection_option, "keep-alive")) keep_alive = true;
    }

    if (close || !(major_version == 1 && (minor_version >= 1 || (minor_version == 0 && keep_alive)))) requestShutdownSocket(client_id);
}