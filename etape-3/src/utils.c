#include <stdlib.h>
#include "utils.h"

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

// Retourne true si s1 et s2 contiennent les mêmes caractères, false sinon
int compare_strings(char *s1, char *s2, int len) {
    int p_len;
    return match_prefix(s1, s2, &p_len) && len == p_len;
}

// Retourne true si p est un préfixe de s, false sinon
// Si len != NULL, sa valeur devient la longueur de p, si c'est un préfixe de s
int match_prefix(char *s, char *p, int *len) {
    int matched = true,
        i = 0;

    // Comparaison caractère par caractère, tant que le préfixe n'a pas été consumé
    while (p[i] != '\0' && matched) {
        if (s[i] != p[i]) matched = false;

        i++;
    }

    if (matched && len != NULL) *len = i;
    return matched;
}