#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include "functions.h"

// Ouvre le fichier demandé et ferme le programme en cas d'erreur
FILE *open_file(char *file_name) {
    FILE *file = fopen(file_name, "r");

    if (file == NULL) exit_on_error("Impossible d'ouvrir le fichier en lecture.");
    return file;
}

// Récupère la taille du fichier demandé et ferme le programme en cas d'erreur
int get_file_size(char *file_name) {
    struct stat stats;

    if (stat(file_name, &stats) == -1) exit_on_error("Impossible de récupérer la taille du fichier.");
    return stats.st_size;
}

// Ouvre le fichier demandé, lit son contenu, le ferme et ferme le programme en cas d'erreur
void read_file(char *file_name, char **buffer) {
    FILE *file = open_file(file_name);
    int size = get_file_size(file_name);

    // Alloue un octet supplémentaire pour placer la sentinelle
    // Ce n'est pas "obligatoire", mais cela évite de sortir du bloc de mémoire lors de la lecture de la sentinelle
    *buffer = create_element(size + 1);
    *(*buffer + size) = '\0';

    if (fread(*buffer, 1, size, file) != size) exit_on_error("Impossible de lire le contenu du fichier.");
    close_file(file);
}

// Ferme le fichier demandé et ferme le programme en cas d'erreur
void close_file(FILE *file) {
    if (fclose(file) == EOF) exit_on_error("Impossible de fermer le fichier.");
}

// Alloue la quantité de mémoire demandée et ferme le programme en cas d'erreur
void *create_element(int size) {
    void *new = malloc(size);
    if (new == NULL) exit_on_error("Impossible d'allouer la mémoire.");

    return new;
}

// Affiche un message d'erreur et ferme le programme
void exit_on_error(char *error_message) {
    fprintf(stderr, "Erreur : %s\nFermeture du programme.\n", error_message);
    exit(EXIT_FAILURE);
}

// Récupère la taille d'une chaîne de caractères
int get_length(char *s) {
    int length = 0;

    // La chaîne doit être valide, c'est-à-dire se terminer par une sentinelle
    while (*s != '\0') {
        length++;
        s++;
    }

    return length;
}

// Vérifie si la valeur d'un octet est présent dans un intervalle
// La comparaison est non-signée pour pouvoir traiter toutes les valeurs possibles
int is_between(unsigned char c, unsigned char c1, unsigned char c2) {
    return c1 <= c && c <= c2;
}

// Convertit une lettre majuscule en lettre minuscule
// Si le caractère n'est pas une lettre majuscule, ne fait rien
char to_lowercase(char c) {
    return is_between(c, 'A', 'Z') ? c - 'A' + 'a' : c;
}

// Compare deux caractères sans tenir compte de la casse des lettres
int compare_insensitive(char c1, char c2) {
    return to_lowercase(c1) == to_lowercase(c2);
}

// Convertit un caractère contenant un chiffre de la base demandée en sa valeur en base 10
// Si le caractère n'est pas un chiffre de la base, ne fait rien
int char_to_digit(char c, int base) {
    int value = 0;
    c = to_lowercase(c);

    if (is_digit(c, base)) value = is_alpha(c) ? c - 'a' + 10 : c - '0';
    return value;
}

// Les fonctions suivantes reprennent simplement les règles données dans l'annexe B de la RFC 5234

// VCHAR = %x21-7E
int is_vchar(char c) {
    return is_between(c, 0x21, 0x7E);
}

// ALPHA = %x41-5A / %x61-7A
int is_alpha(char c) {
    return is_between(to_lowercase(c), 'a', 'z');
}

// BIT = "0" / "1"
// DIGIT = %x30-39
// HEXDIG = DIGIT / "A" / "B" / "C" / "D" / "E" / "F"
// Les règles sont quasiment identiques et sont donc traitées très similairement
// Cette fonction a été généralisée à l'ensemble des bases allant de 0 à 36
int is_digit(char c, int base) {
    int is_digit;

    // Si la base est inférieur ou égale à 10, ne considère que les chiffres comme valeurs possibles
    if (base <= 10) is_digit = is_between(c, '0', '0' + base);
    // Si la base est comprise entre 11 et 36 inclus, considère les chiffres et les lettres comme valeurs possibles
    else if (base <= 36) is_digit = is_between(c, '0', '9') || is_between(to_lowercase(c), 'a', 'a' + base - 10);

    return is_digit;
}

// WSP = SP / HTAB
// SP = %x20
// HTAB = %x09
// Ces règles se simplifient en WSP = %x20 / %x09 et c'est cette version qui est considérée
int is_wsp(char c) {
    return c == ' ' || c == '\t';
}

// CRLF = CR LF
// CR = %x0D
// LF = %x0A
// Ces règles se simplifient en CRLF = %x0D %x0A et c'est cette version qui est considérée
int is_crlf(char *s) {
    return *s == '\r' && *(s + 1) == '\n';
}