#include <stdlib.h>
#include <stdio.h>
#include "functions.h"

FILE *open_file(char *file_name) {
    FILE *file = fopen(file_name, "r");
    if (file == NULL) exit_on_error("Ouverture du fichier impossible.");

    return file;
}

int get_file_size(FILE *file) {
    int size;

    fseek(file, 0, SEEK_END);
    size = ftell(file);
    rewind(file);

    return size;
}

void read_file(FILE *file, char **buffer, int size) {
    *buffer = create_element(size + 1);
    if (fread(*buffer, 1, size, file) != size) exit_on_error("Impossible de lire le contenu du fichier.");
}

void close_file(FILE *file) {
    if (fclose(file) == EOF) exit_on_error("Fermeture du fichier impossible.");
}

void *create_element(int size) {
    void *new = malloc(size);
    if (new == NULL) exit_on_error("Impossible d'allouer la mémoire.");

    return new;
}

int is_between(unsigned char c, unsigned char c1, unsigned char c2) {
    return c1 <= c && c <= c2;
}

int to_lowercase(char c) {
    return is_between(c, 'A', 'Z') ? c - 'A' + 'a' : c;
}

int compare_insensitive(char c1, char c2) {
    return to_lowercase(c1) == to_lowercase(c2);
}

int char_to_digit(char c, int base) {
    int value = 0;
    c = to_lowercase(c);

    if (is_digit(c, base)) value = is_alpha(c) ? c - 'a' + 10 : c - '0';
    return value;
}

int is_vchar(char c) {
    return is_between(c, 0x21, 0x7E);
}

int is_alpha(char c) {
    return is_between(to_lowercase(c), 'a', 'z');
}

int is_digit(char c, int base) {
    int is_digit;

    if (base <= 10) is_digit = is_between(c, '0', '0' + base);
    else if (base <= 36) is_digit = is_between(c, '0', '9') || is_between(to_lowercase(c), 'a', 'a' + base - 10);

    return is_digit;
}

int is_wsp(char c) {
    return c == ' ' || c == '\t';
}

int is_crlf(char *s) {
    return *s == '\r' && *(s + 1) == '\n';
}

void exit_on_error(char *error_message) {
    fprintf(stderr, "Erreur : %s\nFermeture du programme.\n", error_message);
    exit(EXIT_FAILURE);
}