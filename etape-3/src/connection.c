#include "utils.h"
#include "connection.h"
#include <stdio.h>

//#define NULL 0

// A WOOSH + .H
// Applique l'algorithme de la partie 6.3 de la RFC 7230
int get_connection_state(string *http_version, string *connection_option) {
    int major_version = char_to_int(http_version->base[5]),
        minor_version = char_to_int(http_version->base[7]),
        close = false,
        keep_alive = false,
        result = 0;

    if (connection_option->base != NULL) {
        printf("%.*s eske c égal : %d\n", connection_option->length, connection_option->base, compare_strings(connection_option, "close"));
        if (compare_strings(connection_option, "close")) close = true;
        else if (compare_strings(connection_option, "keep-alive")) keep_alive = true;
        result |= SEND_CONNECTION_HEADER;
    }

    if (close || !(major_version == 1 && (minor_version >= 1 || (minor_version == 0 && keep_alive)))) result |= CLOSE_CONNECTION;

    return result;
}