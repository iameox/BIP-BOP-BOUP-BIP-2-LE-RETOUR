#include <stdlib.h>
#include "utils.h"

// Renvoie true si n est compris entre n1 et n2 (inclus), false sinon
int is_between_int(int n, int n1, int n2) {
    return n1 <= n && n <= n2;
}

// Renvoie true si c est compris entre c1 et c2 (inclus), false sinon
// La comparaison est non-signée pour pouvoir traiter toutes les valeurs possibles
int is_between(unsigned char c, unsigned char c1, unsigned char c2) {
    return c1 <= c && c <= c2;
}

// Convertit une lettre majuscule en lettre minuscule
// Si le caractère n'est pas une lettre majuscule, ne fait rien
char to_lowercase(char c) {
    return is_between(c, 'A', 'Z') ? c - 'A' + 'a' : c;
}

// Convertit une chaîne contenant un chiffre en nombre
// Si le caractère n'est pas un chiffre, ne fait rien
int char_to_int(char c) {
    return is_between(c, '0', '9') ? c - '0' : c;
}

// Convertit une chaine de caractères en entier
// Si un caractère qui n'est pas un chiffre est rencontré, la fonction retourne -1
// Sinon s'il y a plus de MAX_DIGITS chiffres, le nombre est tronqué
// Sinon, retourne la valeur de la chaine associée
int string_to_int(string *s) {
    int i = 0, idigit;
    long int value = 0;

    while(i < s->length && i < MAX_DIGITS && value != -1) {
        if (is_between(s->base[i], '0', '9')) {
            idigit = char_to_int(s->base[i]);
            value = value*10 + idigit;
        } else value = -1;
        i++;
    }

    return value;
}

// Convertit un entier en une chaine de caractères
// Si length != null, la longueur de la chaîne lui est affectée
// Sinon s'il y a plus de MAX_DIGITS chiffres, le nombre est tronqué
char *int_to_string(long int n, int *length) {
    long int i = 1;
    int l = 0;
    char *base;

    // Détermination de la taille de la chaîne
    while (l == 0 || (i <= n && l < MAX_DIGITS)) {
        l++;
        i *= 10;
    }

    base = malloc(l * sizeof(char) + 1);
    base[l] = '\0';

    // Écriture caractère par caractère, en commençant par le chiffre de poids le plus faible
    for (i = l - 1; i >= 0; i--) {
        base[i] = '0' + n % 10;
        n /= 10;
    }

    if (length != NULL) *length = l;
    return base;
}

void copy_to_string(const char *s1, string *s2) {
    int length, i = 0;
    char *base;

    while (s1[i] != '\0') i++;

    length = i;
    base = malloc(length * sizeof(char));

    for (i = 0; i < length; i++) base[i] = s1[i];

    s2->base = base;
    s2->length = length;
}

// Retourne true si s1 et s2 contiennent les mêmes caractères, false sinon
int compare_strings(string *s1, char *s2) {
    int p_len;
    return match_prefix(s1, s2, &p_len) && p_len == s1->length;
}

// Retourne true si p est un préfixe de s, false sinon
// Si len != NULL, sa valeur devient la longueur de p, si c'est un préfixe de s
int match_prefix(string *s, char *p, int *len) {
    int stop = false,
        i = 0;

    // Comparaison caractère par caractère, tant que la chaîne ou le préfixe n'a pas été consumé
    while (i < s->length && p[i] != '\0' && !stop) {
        if (s->base[i] != p[i]) stop = true;
        else i++;
    }

    if (p[i] == '\0' && len != NULL) *len = i;
    return p[i] == '\0';
}
