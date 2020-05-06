#include <stdlib.h>
#include "utils.h"
#include "normalization.h"

// A WOOSH + .H
void normalize_request_target(char *request_target, int *length) {
    normalize_percent(request_target, length);
    remove_dot_segments(request_target, length);
}

// A WOOSH
void normalize_percent(char *string, int *length) {
    // real_index représente l'index de parcourt de la chaîne d'entrée
    // virtual_index représente l'index de parcourt de la chaîne finale, c'est la position du prochain caractère à écrire
    int real_index = 0,
        virtual_index = 0,
        nb_to_write, value, digit, i;
    char *s, c;

    while (real_index < *length) {
        s = string + real_index;
        nb_to_write = 1;
        value = 0;

        if (s[0] == '%') {
            for (i = 1; i < 3; i++) {
                if (is_between(s[i], 'a', 'f')) s[i] += 'A' - 'a';
                c = s[i];

                // Toutes les vérifications ont déjà été faites au parsing, il est inutile d'en faire davantage pour cette conversion
                digit = c >= 'A' ? 10 + c - 'A' : c - '0';
                value = 16 * value + digit;
            }

            if (is_between(value, 'A', 'Z') || is_between(value, 'a', 'z') || is_between(value, '0', '9') || value == '-' || value == '.' || value == '_' || value == '~') {
                string[virtual_index] = value;
                real_index += 3;
                virtual_index++;

                nb_to_write = 0;
                
            } else nb_to_write = 3;
        }

        for (i = 0; i < nb_to_write; i++) {
            string[virtual_index] = string[real_index];
            real_index++;
            virtual_index++;
        }
    }

    // Positionne la nouvelle fin de la chaîne
    string[virtual_index] = '\0';
    *length = virtual_index;
}

// A WOOSH
// Normalise une URI du point de vue des "dot segments"
// Applique l'algorithme de la partie 5.2.4 de la RFC 3986
void remove_dot_segments(char *string, int *length) {
    int_stack *index_stack = NULL;
    // real_index représente l'index de parcourt de la chaîne d'entrée
    // virtual_index représente l'index de parcourt de la chaîne finale, c'est la position du prochain caractère à écrire
    int real_index = 0,
        virtual_index = 0,
        p_len;
    char *s;

    while (real_index < *length) {
        s = string + real_index;
        p_len = *length - real_index;

        if (match_prefix(s, "../", &p_len) || match_prefix(s, "./", &p_len) || ((match_prefix(s, "..", &p_len) || match_prefix(s, ".", &p_len)) && *length == p_len)) real_index += p_len;
        else if ((match_prefix(s, "/./", NULL) || compare_strings(s, "/.", p_len))) {
            // Le motif doit être remplacé par "/"
            real_index += 2;
            string[real_index] = '/';

        // Il faut revenir un pas en arrière dans l'arborescence du répertoire
        } else if ((match_prefix(s, "/../", NULL) || compare_strings(s, "/..", p_len))) {
            // Retrouve l'index du "/" précédent (ignore tout ce qui a été réécrit ensuite)
            virtual_index = pop_stack(&index_stack);

            // Le motif doit être remplacé par "/"
            real_index += 3;
            string[real_index] = '/';

        } else {
            // Empile la position actuelle du dernier "/" rencontré, au cas où il y ait un motif "/../" ou "/.." dans la suite de la chaîne
            push_stack(&index_stack, virtual_index);
            
            // Décale tous les caractères jusqu'au prochain "/" exclus à leur position dans la chaîne finale
            do {
                string[virtual_index] = string[real_index];
                real_index++;
                virtual_index++;

            } while (real_index < *length && *(string + real_index) != '/');
        }
    }

    // Positionne la nouvelle fin de la chaîne
    string[virtual_index] = '\0';
    *length = virtual_index;

    // Libère la mémoire associée à la pile
    clear_stack(&index_stack);
}

// Empile top au sommet de stack
void push_stack(int_stack **stack, int top) {
    int_stack *new = malloc(sizeof(int_stack));
    if (new == NULL) exit(EXIT_FAILURE);

    new->top = top;
    new->next = *stack;

    *stack = new;
}

// Dépile et renvoie la tête de stack si elle existe, 0 sinon
int pop_stack(int_stack **stack) {
    int_stack *element = *stack;
    int top = 0;

    if (element != NULL) {
        *stack = element->next;
        top = element->top;

        // Libère la mémoire associée à l'ancienne tête
        free(element);
    }

    return top;
}

// Libère la mémoire associée à la totalité de stack
void clear_stack(int_stack **stack) {
    while (*stack != NULL) pop_stack(stack);
}