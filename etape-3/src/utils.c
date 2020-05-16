#define NULL 0
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

/*
* Convertit une chaine de caractères en entier.
* Si un caractère qui n'est pas un chiffre est rencontré, la fonction retourne -1
* Sinon Si il y a plus de MAX_DIGITS chiffres, le nombre est tronqué.
* Sinon, retourne la valeur de la chaine associée
*/
int string_to_int(string *s) {
    int i = 0, value = 0, idigit;
    long int value;

    while(i < s->length && i < MAX_DIGITS && value != -1) {
        if (is_between(s->base[i], '0', '9')) {
            idigit = char_to_int(s->base[i]);
            value = value*10 + idigit;
        } else value = -1;
        i++;
    }

    return value;
}

/*char *int_to_string(int n, int *length) {
    int i = 10, l = 0;
    char *base;

    while (i < n) {
        l++;
        i *= 10;
    }

    *base = malloc(l * sizeof(char) + 1);
    base[l] = '\0';

    for (i = 0; i < l; i++) {
        base[i] = '0' + n % 10;
        n /= 10;
    }

    if (length != NULL) *length = l;
    return base;
}*/

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
