#include "utils.h"
#include "connection.h"
#include <stdio.h>

//#define NULL 0

// A WOOSH + .H
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
            result |= SEND_CONNECTION_HEADER;
        }

        if (close || !(major_version == 1 && (minor_version >= 1 || (minor_version == 0 && keep_alive)))) result |= CLOSE_CONNECTION;
    
    } else result = SEND_CONNECTION_HEADER | CLOSE_CONNECTION;

    return result;
}